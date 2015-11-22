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

void NetworkReplyTimeout::_timeout()
{
    if (mNetworkReply->isRunning()) {
        mNetworkReply->abort();
        emit timeout(mNetworkReply);
    }
}
