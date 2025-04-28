#include "QScaryMan.h"

QScaryMan::QScaryMan(QWidget *parent)
    : QWidget(parent), mainImg(new QResizeableLabel(this))
{
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::MaximizeUsingFullscreenGeometryHint);
    mainImg->resize(size());

    player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    player->setVolume(100);

    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/QScaryMan/scarymanicon"));
    QMenu* trayMenu = new QMenu(this);
    trayMenu->addAction("Quit", QApplication::instance(), SLOT(quit()));
    tray->setContextMenu(trayMenu);
    tray->show();
    client = nullptr;
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port))
    {
        server->close();
        qApp->quit();
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(connected()));
    connect(player, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState && audioBuf != nullptr)
        {
            audioBuf->close();
            audioBuf->deleteLater();
            }
        });
}

QScaryMan::~QScaryMan()
{

}

void QScaryMan::keyPressEvent(QKeyEvent* e)
{
    int key = e->key();

    switch (key)
    {
    case Qt::Key_Escape:
        qApp->quit();
        break;
    case Qt::Key_Q:
        hideScaryMan();
        break;
    }
}

void QScaryMan::resizeEvent(QResizeEvent* e)
{
    mainImg->resize(e->size());
}

bool QScaryMan::showScaryMan()
{
    if (isPlaying)
        return false;

    isPlaying = true;
    playAudio();
    QTimer::singleShot(300, this, SLOT(show()));
    return true;
}

void QScaryMan::hideScaryMan()
{
    if (!isPlaying)
        return;

    isPlaying = false;
    player->stop();
    hide();
}

void QScaryMan::playAudio()
{
    if (!audio.isEmpty())
    {
        audioBuf = new QBuffer(&audio);
        audioBuf->open(QIODevice::ReadOnly);
        audioBuf->reset();
        player->setMedia(QMediaContent(), audioBuf);
        player->play();
    }
}

void QScaryMan::connected()
{
    if (client == nullptr)
    {
        qDebug() << "connected";
        client = server->nextPendingConnection();
        connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(client, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError err) {disconnected(); });
        connect(client, SIGNAL(readyRead()), this, SLOT(readSocket()));
    }
    else 
    {
        QTcpSocket* rejectedClient = server->nextPendingConnection();
        rejectedClient->close();
        rejectedClient->deleteLater();
    }
}

void QScaryMan::disconnected()
{
    client->close();
    client->deleteLater();
    client = nullptr;
}

void QScaryMan::readSocket()
{
    while (true)
    {
        if (readCommand)
        {
            if (client->bytesAvailable() < 4)
                return;

            client->read((char*)&command, 4);
            readCommand = false;
        }

        if (readSize)
        {
            if (client->bytesAvailable() < 4)
                return;

            client->read((char*)&packetSize, 4);
            readSize = false;
            readData = true;
        }

        QByteArray data;
        if (readData)
        {
            if (client->bytesAvailable() < packetSize)
                return;

            data = client->read(packetSize);
            readData = false;
        }

        CommandEnum response = NO_RESPONSE;
        switch (command)
        {
        case START:
        {
            if (mainImg->isPixmapPresent())
            {
                if (showScaryMan())
                    response = SUCCESS;
                else
                    response = ERROR_RESULT;
            }
            else 
            {
                response = NO_IMAGE;
            }
            readCommand = true;
            break;
        }
        case STOP:
        {
            hideScaryMan();
            response = SUCCESS;
            readCommand = true;
            break;
        }
        case SEND_MSG:
        {
            readSize = true;
            command = SHOW_MSG;
            break;
        }
        case SHOW_MSG:
        {
            QString message(data);
            QMessageBox::information(nullptr, "Something happened", message);
            response = SUCCESS;
            readCommand = true;
            break;
        }
        case UPLOAD_IMG:
        {
            readSize = true;
            command = SAVE_IMG;
            break;
        }
        case SAVE_IMG:
        {
            if (!isPlaying)
            {
                QPixmap p;
                if (p.loadFromData(data))
                {
                    mainImg->savePixmap(p);
                    response = SUCCESS;
                }
                else
                {
                    response = ERROR_RESULT;
                }
            }
            else
            {
                response = ERROR_RESULT;
            }
            readCommand = true;
            break;
        }
        case UPLOAD_AUDIO:
        {
            readSize = true;
            command = SAVE_AUDIO;
            break;
        }
        case SAVE_AUDIO:
        {
            if (!isPlaying)
            {
                audio = data;
                response = SUCCESS;
            }
            else
            {
                response = ERROR_RESULT;
            }
            
            readCommand = true;
            break;
        }
        default:
        {
            response = ERROR_RESULT;
            readCommand = true;
            break;
        }
        }

        if (response != NO_RESPONSE)
            client->write((char*)&response, 4);
    }
}