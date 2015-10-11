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
    void start();
    void clear();

signals:
    void finished();
    void replyFinished(QNetworkReply*);

public slots:

private slots:
    void onReplyFinished(QNetworkReply*);

private:
    QList<QUrl> mQueuedUrls;
    QList<QUrl> mFinishedUrls;
    int mFinishedCount;
    QNetworkAccessManager* mNetworkManager;

};

#endif // FILEDOWNLOADER_H
