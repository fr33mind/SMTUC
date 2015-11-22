#include "filedownloader.h"

#include <QUrl>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QStandardPaths>

#include "network_reply_timeout.h"

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent)
{
    mNetworkManager = new QNetworkAccessManager(this);
    QNetworkDiskCache* cache = new QNetworkDiskCache();
    cache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    mNetworkManager->setCache(cache);
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

FileDownloader::~FileDownloader()
{
    clear();
}

void FileDownloader::addUrl(const QUrl& url)
{
    if (QUrl(url).isValid())
        mQueuedUrls.append(QUrl(url));
}

QList<QUrl> FileDownloader::queuedUrls() const
{
    return mQueuedUrls;
}

QList<QUrl> FileDownloader::finishedUrls() const
{
    return mFinishedUrls;
}

QList<QUrl> FileDownloader::urls() const
{
    return mFinishedUrls + mQueuedUrls;
}

void FileDownloader::clear()
{
    mQueuedUrls.clear();
    mFinishedUrls.clear();

    if (mNetworkManager->cache())
        mNetworkManager->cache()->clear();
}

QByteArray FileDownloader::data(const QUrl & url)
{
    QNetworkDiskCache* cache = qobject_cast<QNetworkDiskCache*>(mNetworkManager->cache());
    if (cache) {
        QIODevice* reader = cache->data(url);
        if (reader)
            return reader->readAll();
    }
    return QByteArray();
}

bool FileDownloader::start()
{
    if (mNetworkManager->networkAccessible() != QNetworkAccessManager::Accessible)
        return false;

    for(int i=0; i < mQueuedUrls.size(); i++) {
        QNetworkReply * reply = mNetworkManager->get(QNetworkRequest(mQueuedUrls.at(i)));
        new NetworkReplyTimeout(reply, this);
    }

    return true;
}

void FileDownloader::onReplyFinished(QNetworkReply * reply)
{
    if (mQueuedUrls.contains(reply->url())) {
        mQueuedUrls.removeOne(reply->url());
        mFinishedUrls.append(reply->url());
    }

    if (reply->error() != QNetworkReply::NoError) {
        mErrorUrls.append(reply->url());
        emit replyError(reply);
    }

    emit replyFinished(reply);

    if (mQueuedUrls.isEmpty())
        emit finished();

    reply->deleteLater();
}

bool FileDownloader::hasUrl(const QUrl& url) const
{
    QList<QUrl> urls = this->urls();
    return urls.contains(url);
}

QNetworkAccessManager* FileDownloader::networkAccessManager() const
{
    return mNetworkManager;
}

bool FileDownloader::hasErrors() const
{
    return !mErrorUrls.isEmpty();
}
