#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTime>
#include <stdlib.h>


void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        printf("%s\n", msg);
        break;
    case QtWarningMsg:
        printf("Warning: %s %s\n", QTime::currentTime().toString(Qt::TextDate).toAscii().data() , msg);
        break;
    case QtCriticalMsg:
        printf("Critical: %s %s\n", QTime::currentTime().toString(Qt::TextDate).toAscii().data() , msg);
        break;
    case QtFatalMsg:
        printf("Fatal: %s %s\n",QTime::currentTime().toString(Qt::TextDate).toAscii().data() , msg);
        break;
    }
}


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
