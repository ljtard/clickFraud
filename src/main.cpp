#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTime>
#include <stdlib.h>


void myMessageOutput(QtMsgType type, const char *msg)
{
    printf("%s\n", msg);
}


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
