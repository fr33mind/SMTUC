#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QObject *parent = 0);
    virtual ~FileDownloader();
    void addUrl(const QUrl&);
    QList<QUrl> queuedUrls() const;
    QList<QUrl> finishedUrls() const;
    QList<QUrl> urls() const;
    bool hasUrl(const QUrl&) const;
    QByteArray data(const QUrl&);
    bool start();
    void clear();
    QNetworkAccessManager* networkAccessManager() const;
    bool hasErrors() const;

signals:
    void finished();
    void replyFinished(QNetworkReply*);
    void replyError(QNetworkReply*);

public slots:

private slots:
    void onReplyFinished(QNetworkReply*);
    void onReplyError(QNetworkReply*);

private:
    QList<QUrl> mQueuedUrls;
    QList<QUrl> mErrorUrls;
    QList<QUrl> mFinishedUrls;
    int mFinishedCount;
    QNetworkAccessManager* mNetworkManager;

};

#endif // FILEDOWNLOADER_H
