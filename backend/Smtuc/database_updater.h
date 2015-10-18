#ifndef DATABASEUPDATER_H
#define DATABASEUPDATER_H

#include <QObject>
#include <QSqlDatabase>
#include <QThread>

#include "filedownloader.h"
#include "database_updater_worker.h"

class Database;

class DatabaseUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY( Database* database READ database WRITE setDatabase )
    Q_PROPERTY( double progress READ progress NOTIFY progressChanged )
    Q_PROPERTY( bool finished READ isFinished NOTIFY finished )

public:
    DatabaseUpdater(Database *db = 0, QObject *parent = 0);
    virtual ~DatabaseUpdater();

    Database* database() const;
    void setDatabase(Database*);

    Q_INVOKABLE void update();
    double progress() const;

    bool isFinished() const;

signals:
    void started();
    void progressChanged();
    void finished();

public slots:

private:
    Database* mDatabase;
    DatabaseUpdaterWorker* mWorker;
    QThread* mWorkerThread;
};

#endif // DATABASEUPDATER_H
