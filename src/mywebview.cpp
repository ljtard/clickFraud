#include "mywebview.h"
#include <QDebug>


MyNetworkAccessManager::MyNetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    userAgent = "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; MRSPUTNIK 2, 4, 0, 153; .NET CLR 2.0.50727; InfoPath.2; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET4.0C; .NET4.0E)";
}
QNetworkReply *MyNetworkAccessManager::createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData )
{
    QNetworkRequest myReq(req);
    myReq.setRawHeader("User-Agent", userAgent.toAscii() );
    myReq.setRawHeader( "Referer", referer.toAscii() );
    return QNetworkAccessManager::createRequest( op, myReq, outgoingData);
}
