#include "QScaryMan.h"
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QScaryMan w;
    w.hide();

    QApplication::setQuitOnLastWindowClosed(false);

    return a.exec();
}
