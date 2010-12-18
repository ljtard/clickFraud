#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QRegExp>
#include <QFileDialog>
#include <QDateTime>
#include <QWebFrame>
#include <QDebug>
#include <QWebElement>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    settings("config.ini", QSettings::IniFormat),
    ui(new Ui::MainWindow)
{
    this->setMouseTracking( true );
    qsrand( QDateTime::currentDateTime().toTime_t() );


    this->active = false;
    ui->setupUi(this);


    ui->webView->page()->setNetworkAccessManager( &nam );
    
    enable= new QAction( QIcon(":/img/rec.png"), "" , this);
    enable->setCheckable( true );
    enable->setChecked( false );
    connect( enable, SIGNAL(triggered(bool)), this, SLOT(startStop(bool)) );
    ui->mainToolBar->addAction( enable );

    version = new QLabel("Version 0.11",this);
    ui->mainToolBar->addWidget( version );
    
    ui->actionOpenBlogList->setEnabled( false );
    ui->actionOpen_Proxy_List->setEnabled( false );

    this->restoreState( this->settings.value("state").toByteArray() );
    this->restoreGeometry( this->settings.value("geometry").toByteArray() );

    progress = 0;

    connect( this->ui->actionOpenBlogList , SIGNAL(triggered(bool)) , this, SLOT(onOpenBloggerList(bool)) );
    connect( this->ui->actionOpen_Proxy_List , SIGNAL(triggered(bool)) , this, SLOT(onOpenProxyList(bool)) );
    connect( this->ui->actionExit , SIGNAL(triggered(bool)) , QCoreApplication::instance(), SLOT(quit()));

    connect( this->ui->webView, SIGNAL(loadFinished(bool)) , this, SLOT(onFinishLoading(bool)) );
    connect( this->ui->webView, SIGNAL(titleChanged(QString)), this, SLOT(adjustTitle()) );
    connect( this->ui->webView, SIGNAL(loadProgress(int)), this, SLOT(setProgress(int)));

    timerBlogUpdate.setSingleShot( true );
    timerBlogActivity.setSingleShot( true );

    connect( &timerBlogUpdate , SIGNAL(timeout()) , this , SLOT(doQuery()) );
    connect( &timerBlogActivity , SIGNAL(timeout()) , this , SLOT(doSomthing()) );
    connect( this->ui->actionHistory, SIGNAL(triggered(bool)) , this->ui->historyDockWidget, SLOT(setShown(bool)) );
    connect( this->ui->actionBlog_List, SIGNAL(triggered(bool)) , this->ui->blogListDockWidget, SLOT(setShown(bool)) );

    maximumBlogUpdateTime = this->settings.value("maximumBlogUpdateTime", 60000 ).toInt();
    minimumBlogUpdateTime = this->settings.value("minimumBlogUpdateTime",  0).toInt();

    maximumBlogActivityTime = this->settings.value("maximumBlogActivityTime", 30000 ).toInt();
    minimumBlogActivityTime = this->settings.value("minimumBlogActivityTime",  0).toInt();

    trash = this->settings.value("trash",  0.5 ).toFloat();

    maximumDeep = this->settings.value("maximumDeep",  3 ).toInt();
    minimumDeep = this->settings.value("minimumDeep",  1 ).toInt();

    int size = this->settings.beginReadArray("Referers");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        refererList.append( this->settings.value( "Referer" ).toString() );
    }
    this->settings.endArray();

    size = this->settings.beginReadArray("UserAgents");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        useragentList.append( this->settings.value( "UserAgent" ).toString() );
    }
    this->settings.endArray();

    domain = settings.value("domain","serdukivan.mgn-host.ru").toString();

    QString url = QString("http://%1/index.php/user/apiaccess/refriesh").arg( domain );
    this->loadBloggerListOverUrl( url );
    this->loadJQuery();
}

MainWindow::~MainWindow()
{
    this->settings.setValue( "state" , this->saveState() );
    this->settings.setValue( "geometry" , this->saveGeometry() );

    this->settings.setValue("maximumBlogUpdateTime", maximumBlogUpdateTime );
    this->settings.setValue("minimumBlogUpdateTime",  minimumBlogUpdateTime );

    this->settings.setValue("maximumBlogActivityTime", maximumBlogActivityTime );
    this->settings.setValue("minimumBlogActivityTime",  minimumBlogActivityTime);

    this->settings.setValue("maximumDeep", maximumDeep );
    this->settings.setValue("minimumDeep", minimumDeep );

    this->settings.setValue("trash", trash );

    this->settings.setValue("domain",domain);

    this->settings.beginWriteArray("Referers");
    for (int i = 0; i < this->refererList.size(); ++i)
    {
        settings.setArrayIndex(i);
        this->settings.setValue( "Referer", this->refererList.at(i) );
    }
    this->settings.endArray();

    this->settings.beginWriteArray("UserAgents");
    for (int i = 0; i < this->useragentList.size(); ++i)
    {
        settings.setArrayIndex(i);
        this->settings.setValue( "UserAgent", this->useragentList.at(i) );
    }
    this->settings.endArray();

    delete ui;
}

void MainWindow::doQuery()
{
    qDebug()<<"void MainWindow::doQuery()";
    if( !this->bloggerList.size() )
        return;

    QString blog;
    unsigned int rnd = (qrand()%100 )+1;
    qDebug()<<rnd<<((int) (trash*100));
    if( rnd <= (int) ( trash*100 ) )
    {
        blog = "http://www.blogger.com/next-blog?navBar=true&blogID=35935091";
    }
    else
    {
        int rendomBlogNumber = qrand()%this->bloggerList.size();
        this->ui->blogListWidget->setCurrentRow( rendomBlogNumber );
        blog = this->bloggerList.at( rendomBlogNumber );
    }

    this->deep = minimumDeep + ( qrand()%( maximumDeep - minimumDeep ) ) ;

    currentReferer = this->refererList.at( qrand()%refererList.size() );
    currentReferer.replace( "%blog%" , blog );


    currentUserAgent = this->useragentList.at( qrand()%useragentList.size() );

    qDebug()<<blog;

    nam.referer = currentReferer;
    nam.userAgent = currentUserAgent;

    qDebug()<<"request "<<blog<<" Referer = "<<currentReferer<<" UserAgent = "<<currentUserAgent<<" deep = "<<deep;
    this->ui->webView->load( blog );
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle( this->ui->webView->title());
    else
        setWindowTitle( QString("%1 (%2%)").arg( this->ui->webView->title() ).arg(progress));
}
void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::onOpenBloggerList( bool val )
{
    QFileDialog dialog( this , "open blogger list","","*.txt");
    if( dialog.exec() )
    {
        QString filename = dialog.selectedFiles().first();
        this->loadBloggerList( filename );
    }
}

void MainWindow::onOpenProxyList( bool val )
{
    QFileDialog dialog( this , "open blogger list","","*.txt");
    if( dialog.exec() )
    {
        QString filename = dialog.selectedFiles().first();
        this->loadProxyList( filename );
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
bool MainWindow::event(QEvent * event)
{
    switch( event->type() )
    {
    case QEvent::MouseMove:
    {
            QMouseEvent* me = (QMouseEvent *)event;
            qDebug()<<me->x()<<"x"<<me->y();
    }
    break;
    }
    return false;
}

bool MainWindow::loadJQuery()
{
    QFile file;
    file.setFileName(":/jquery-1.5.js");
    if( ! file.open(QIODevice::ReadOnly) )
        return false;
    jQuery = file.readAll();
    file.close();
    return true;
}

bool MainWindow::loadBloggerListOverUrl(QString url)
{
    QNetworkReply *rep = qnam.get( QNetworkRequest( QUrl( url ) ) );
    connect( rep, SIGNAL(finished()) , this, SLOT(onFinishedLoadBloggerList() ) );
    connect( rep, SIGNAL(error( QNetworkReply::NetworkError)) , this, SLOT(printError(QNetworkReply::NetworkError) ) );

    return true;
}
void MainWindow::printError( QNetworkReply::NetworkError error )
{
    qDebug()<<error;
}
void MainWindow::startStop( bool e)
{
    qDebug()<<e;
    if( e )
    {
        this->doQuery();
        this->active = true;
    }
    else
    {
        this->ui->webView->stop();
        this->active = false;
        this->timerBlogActivity.stop();
        this->timerBlogUpdate.stop();
        this->deep=0;
    }
}

void MainWindow::onFinishedLoadBloggerList()
{
    QNetworkReply* rep = ( (QNetworkReply*) sender() );
    QString list = rep->readAll();
    list = list.remove("\r");
    QStringList rowlist = list.split("\n");
    foreach(  QString row , rowlist )
    {
        if( row.isEmpty() || row.at(0) == '#')
            continue;
        this->bloggerList.append( row );
    }
    this->ui->blogListWidget->addItems( this->bloggerList );
}

bool MainWindow::loadBloggerList( QString filename )
{
    QFile file;
    file.setFileName( filename );
    if( ! file.open(QIODevice::ReadOnly) )
        return false;
    QString list = file.readAll();
    file.close();
    list = list.remove("\r");
    QStringList rowlist = list.split("\n");
    foreach(  QString row , rowlist )
    {
        if( row.isEmpty() || row.at(0) == '#')
            continue;
        this->bloggerList.append( row );
    }

    this->ui->blogListWidget->addItems( this->bloggerList );
    return true;
}

void MainWindow::clearBloggerList( )
{
    this->bloggerList.clear();
    this->ui->blogListWidget->clear();
}

bool MainWindow::loadProxyList( QString filename )
{
    QFile file;
    file.setFileName( filename );
    if( ! file.open(QIODevice::ReadOnly) )
        return false;
    QString list = file.readAll();
    QStringList rowlist = list.split('\n');
    foreach( QString row , rowlist )
    {
        if( row.at(0) == '#')
            continue;
        QStringList words = row.split( QRegExp("\\s+") );
        QNetworkProxy proxy( QNetworkProxy::DefaultProxy, words.at(0), words.at(1).toInt() );
        if( words.size() == 4 )
        {
            proxy.setUser( words.at(2) );
            proxy.setPassword( words.at(3) );
        }
        this->proxyList.append( proxy );
    }
    file.close();
    return true;
}

void MainWindow::clearProxyList()
{
    this->proxyList.clear();
}

void MainWindow::onFinishLoading( bool finish )
{
    qDebug()<<"MainWindow::onFinishLoading( bool "<<finish<<" )";
    if( !this->active )
        return;
    ///
    doSomthing();
    ///
    /*

    if( !finish )
    {
        randomizeBlogUpdateTimer();
        return;
    }

    this->ui->historyListWidget->addItem( this->ui->webView->url().toString() + " Referer = "+this->currentReferer +" UserAgent = "+ this->currentUserAgent );
    int last =( this->ui->historyListWidget->count() - 1 );
    foreach( QString blog, this->bloggerList )
    {
        if( blog.contains( this->ui->webView->url().host() ) )
            this->ui->historyListWidget->item( last )->setData( Qt::BackgroundColorRole , QColor(Qt::green) );
    }

    if( this->deep )
        randomizeBlogActivityTimer();
    else
        randomizeBlogUpdateTimer();

    */



}
void MainWindow::doSomthing()
{
    if( !this->active )
        return;

    qDebug()<<"doSomthing()";
    QString html = this->ui->webView->page()->mainFrame()->toHtml();
    ///
    QString code;
    ui->webView->page()->mainFrame()->evaluateJavaScript(jQuery);
    code = "$(document.elementFromPoint(x, y)).click();";
    ui->webView->page()->mainFrame()->evaluateJavaScript(code);
/*
    ///
    QString host = this->ui->webView->url().host();
    QString regexp = "http://"+host+"/\\d+/\\d+/\\S+\\.html";
    //"http://\\w+\\.blogspot\\.com/\\d+/\\d+/\\S+\\.html";
    QRegExp r( regexp );
    QStringList links;
    int pos = 0;
    while( (pos = r.indexIn( html, pos ))!= -1 )
    {
        links.append( html.mid( pos, r.matchedLength() ) );
        pos+= r.matchedLength();
    }
    if( links.size() )
    {

        QString blog = links.at( qrand()%links.size() );

        nam.referer = this->ui->webView->url().toString();
        nam.userAgent = currentUserAgent.toAscii();

        qDebug()<<"additional request "<<blog;

        this->ui->webView->load( blog );

        deep--;
    }
    else
    {
        deep = 0;
        randomizeBlogUpdateTimer();
    }
*/
}


void MainWindow::randomizeBlogUpdateTimer()
{
    int time = qrand()%(maximumBlogUpdateTime-minimumBlogUpdateTime);
    this->timerBlogUpdate.setInterval( minimumBlogUpdateTime + time );
    timerBlogUpdate.start();
}

void MainWindow::randomizeBlogActivityTimer()
{
    int time = qrand()%(maximumBlogActivityTime - minimumBlogActivityTime);
    timerBlogActivity.setInterval( minimumBlogActivityTime + time );
    timerBlogActivity.start();
}



