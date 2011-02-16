#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTime>
#include <stdlib.h>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QObject>


void myMessageOutput(QtMsgType type, const char *msg)
{
    printf("%s\n", msg);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMsgHandler(myMessageOutput);
    if( !QSystemTrayIcon::isSystemTrayAvailable() )
    {
        QMessageBox::critical( 0, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system.") );
        return 1;
    }


    MainWindow w;
    w.show();
    return a.exec();
}
