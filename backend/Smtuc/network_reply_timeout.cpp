#include "network_reply_timeout.h"

#include <QTimer>

NetworkReplyTimeout::NetworkReplyTimeout(QNetworkReply* reply, QObject *parent) :
    QObject(parent)
{
    mNetworkReply = reply;
    if (mNetworkReply) {
        connect(mNetworkReply, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        QTimer::singleShot(TIMEOUT*1000, this, SLOT(_timeout()));
    }
}

QNetworkReply* NetworkReplyTimeout::networkReply() const
{
    return mNetworkReply;
}

void NetworkReplyTimeout::_timeout()
{
    if (mNetworkReply->isRunning()) {
        mNetworkReply->abort();
        emit timeout(mNetworkReply);
    }
}
