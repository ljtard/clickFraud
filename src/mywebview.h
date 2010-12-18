#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>

class MyNetworkAccessManager : public QNetworkAccessManager
{
Q_OBJECT
public:
    explicit MyNetworkAccessManager(QObject *parent = 0);

    QString userAgent;
    QString referer;

protected:
    QNetworkReply *createRequest( Operation op, const QNetworkRequest & req, QIODevice * outgoingData );
signals:

public slots:

};

#endif // MYWEBVIEW_H
