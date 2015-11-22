#ifndef NETWORK_REPLY_TIMEOUT_H
#define NETWORK_REPLY_TIMEOUT_H

#include <QObject>
#include <QNetworkReply>

#define TIMEOUT 10

class NetworkReplyTimeout : public QObject
{
    Q_OBJECT
public:
    NetworkReplyTimeout(QNetworkReply*, QObject *parent = 0);
    QNetworkReply* networkReply() const;

signals:
    void timeout(QNetworkReply*);

private slots:
    void _timeout();

private:
    QNetworkReply* mNetworkReply;

};

#endif // NETWORK_REPLY_TIMEOUT_H
