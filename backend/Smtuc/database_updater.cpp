#include "database_updater.h"

#include <QDebug>

#include "database.h"

DatabaseUpdater::DatabaseUpdater(Database* db, QObject *parent) :
    QObject(parent)
{
    mDatabase = db;
    mWorker = 0;
    mWorkerThread = 0;
}

DatabaseUpdater::~DatabaseUpdater()
{
    if (mWorkerThread) {
        mWorkerThread->quit();
        mWorkerThread->wait();
        mWorkerThread->deleteLater();
    }

    if (mWorker)
        mWorker->deleteLater();
}

Database* DatabaseUpdater::database() const
{
    return mDatabase;
}

void DatabaseUpdater::setDatabase(Database* db)
{
    mDatabase = db;
}

void DatabaseUpdater::update()
{
    if (mWorkerThread && mWorkerThread->isRunning())
        return;

    if (mWorkerThread)
        mWorkerThread->deleteLater();
    if (mWorker)
        mWorker->deleteLater();

    mWorkerThread = new QThread(this);
    mWorker = new DatabaseUpdaterWorker(mDatabase);
    mWorker->moveToThread(mWorkerThread);
    connect(mWorker, SIGNAL(progressChanged()), this, SIGNAL(progressChanged()));
    connect(mWorkerThread, SIGNAL(finished()), this, SIGNAL(finished()));
    connect(mWorker, SIGNAL(finished()), mWorkerThread, SLOT(quit()));
    connect(mWorkerThread, SIGNAL(started()), mWorker, SLOT(start()));
    mWorkerThread->start();
    emit started();
}

double DatabaseUpdater::progress() const
{
    if (! mWorker)
        return 0;

    return mWorker->progress();
}

bool DatabaseUpdater::isFinished() const
{
    if (mWorkerThread && mWorkerThread->isFinished())
        return true;

    return false;
}
