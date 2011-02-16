#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkProxy>
#include <QStringList>
#include <QTimer>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAction>
#include <QLabel>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include "mywebview.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onOpenBloggerList( bool val );
    void onOpenProxyList( bool val );

    bool loadBloggerList( QString filename );
    bool loadBloggerListOverUrl(QString url);
    void onFinishedLoadBloggerList();
    void clearBloggerList();

    bool loadProxyList( QString filename );
    void clearProxyList();

    void adjustTitle();
    void setProgress(int p);

    void doQuery();
private slots:
    void onFinishLoading( bool finish );
    void doSomthing();
    void printError( QNetworkReply::NetworkError error );
    void startStop( bool e);
    void onTrayActivated(QSystemTrayIcon::ActivationReason r );


protected:
    void changeEvent(QEvent *e);

private:
    void randomizeBlogUpdateTimer();
    void randomizeBlogActivityTimer();
    bool loadJQuery();
    int loadBlogsFromData( QString data );

    QSettings settings;

    Ui::MainWindow *ui;
    MyNetworkAccessManager nam;
    QAction *enable;
    QLabel *version;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;


    QString jQuery;

    QString currentUserAgent;
    QString currentReferer;

    QList<QNetworkProxy> proxyList;
    QStringList bloggerList;
    QStringList refererList;
    QStringList useragentList;

    QTimer timerBlogUpdate;
    QTimer timerBlogActivity;
    int progress;

    int maximumBlogUpdateTime;
    int minimumBlogUpdateTime;

    int maximumBlogActivityTime;
    int minimumBlogActivityTime;

    float trash;

    int minimumDeep;
    int maximumDeep;

    int deep;

    QString domain;

    QNetworkAccessManager qnam;
    int tmp;
    bool active;
};

#endif // MAINWINDOW_H
