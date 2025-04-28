#pragma once

#include <QtWidgets>
#include <qmediaplayer.h>
#include <QAudioOutput>
#include <QTcpServer>
#include <QTcpSocket>
#include <QIcon>

#include "QResizeableLabel.h"
#include "CommandEnum.h"

class QScaryMan : public QWidget
{
    Q_OBJECT

public:
    QScaryMan(QWidget *parent = nullptr);
    ~QScaryMan();

private:
    QResizeableLabel* mainImg;
    QMediaPlayer* player;
    QTcpServer* server;
    QTcpSocket* client;
    QSystemTrayIcon* tray;

    bool readCommand = true;
    bool readData = false;
    bool readSize = false;
    bool isPlaying = false;
    int packetSize = 0;
    const int port = 8081;
    CommandEnum command;

    QByteArray audio;
    QBuffer* audioBuf;

private:
    void playAudio();

protected:
    virtual void keyPressEvent(QKeyEvent* e) override;
    virtual void resizeEvent(QResizeEvent* e) override;

public slots:
    bool showScaryMan();
    void hideScaryMan();
    void connected();
    void disconnected();
    void readSocket();
};
