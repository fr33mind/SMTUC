#ifndef DATABASE_UPDATER_WORKER_H
#define DATABASE_UPDATER_WORKER_H

#include <QObject>
#include <QByteArray>
#include <QJsonValue>
#include <QSqlDatabase>
#include <QHash>

#include "filedownloader.h"
#include "database.h"

#define SEASONS_URL "http://smtuc.pt/api/epocas/"
#define ROUTES_URL "http://smtuc.pt/api/linhaspercurso/"
#define ROUTE_TIMES_URL "http://smtuc.pt/api/horariostempos/"
#define TICKETS_URL "http://smtuc.pt/api/bilhetes/"
#define TICKET_PRICES_URL "http://smtuc.pt/api/bilhetes/%1/tarifas/"
#define OUTLETS_URL "http://smtuc.pt/api/postos/"
#define VARS_URL "http://smtuc.pt/api/vars/"

class Database;

class DatabaseUpdaterWorker : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseUpdaterWorker(Database* db, QObject *parent = 0);
    virtual ~DatabaseUpdaterWorker();

    void setStatusMessage(const QString&);
    QString statusMessage() const;

    void setTaskCount(int);

    double progress() const;

public slots:
    void start();

protected:

signals:
    void progressChanged();
    void finished();
    void statusMessageChanged(const QString&);
    void error(const QString&);

private slots:
    void load();
    void replyFinished(QNetworkReply* reply);
    void taskComplete(bool success=true);

private:
    FileDownloader* mFileDownloader;
    Database* mDatabase;
    int mQueuedTaskCount;
    int mCompletedTaskCount;
    int mFailedTaskCount;
    QString mStatusMessage;
    QStringList mFavoriteRoutes;

    void loadSeasons(const QByteArray&, QSqlDatabase&);
    void loadRoutes(const QByteArray&, QSqlDatabase&);
    QString routeNumber(const QString&);
    void loadRouteStops(const QHash<int, QStringList>&, QSqlDatabase&);
    void loadStops(QStringList, QSqlDatabase&);
    QStringList parseStops(const QJsonValue&, const QStringList& checkedStops=QStringList());
    void loadRouteSchedules(const QByteArray&, QSqlDatabase&);
    void loadRouteSchedule(const QJsonObject&, QSqlDatabase&);
    void loadRouteTimes(const QJsonObject&, QSqlDatabase&);
    QStringList parseTimes(const QJsonValue&);
    QHash<QString, int> stopsHash();
    void loadTickets(const QByteArray&, QSqlDatabase&);
    void loadTicketPrices(int, QSqlDatabase&);
    void loadTicketPrice(const QJsonValue&, QSqlDatabase&);
    bool hasTicketPrices() const;
    bool downloadTicketPrices();
    void downloadOutlets();
    void loadOutlets(const QByteArray&, QSqlDatabase&);
    void loadOutlet(const QJsonValue&, QSqlDatabase&);
    void loadVars(const QByteArray&, QSqlDatabase&);
    QString _toHtml(const QString&);
    void backupFavoriteRoutes(QSqlDatabase&);
    void restoreFavoriteRoutes(QSqlDatabase&);

    QByteArray loadData(const QString&);
};

#endif // DATABASE_UPDATER_WORKER_H
