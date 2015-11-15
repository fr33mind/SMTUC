#include "database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QSqlDriver>
#include <QStandardPaths>

static QHash<QString, DatabaseConnection*> mDatabaseConnections;

Database::Database(QObject *parent) :
    QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (! db.isValid())
        db = QSqlDatabase::addDatabase("QSQLITE");
    mSqlDatabase = db;
    mConnection = Database::connection(connectionName());
}

Database::~Database()
{
    Database::removeDatabase(this);
    emit destroyed(this);
}

void Database::classBegin()
{
}

void Database::componentComplete()
{
    Database::addDatabase(this);
    mConnection = Database::connection(connectionName());
}

QSqlDatabase Database::database() const
{
    return mSqlDatabase;
}

QString Database::connectionName() const
{
    return mSqlDatabase.connectionName();
}

DatabaseConnection* Database::connection() const
{
    return mConnection;
}

QString Database::test()
{
    if (mSqlDatabase.isOpen()) {
        qDebug() << "db did open";
        QSqlQuery query = mSqlDatabase.exec("select * from routes where id = 1");
        qDebug() << query.first();
        return query.value("name").toString();
    }

    return "";
}

void Database::setDatabaseName(const QString & name)
{
    QString dbname = setupDatabase(name);
    mSqlDatabase.setDatabaseName(dbname);
    mSqlDatabase.open();
}

QString Database::databaseName() const
{
    return mSqlDatabase.databaseName();
}

bool Database::isReady() const
{
    return (mSqlDatabase.isValid() && mSqlDatabase.isOpen());
}

QVariant Database::exec(const QString & q)
{
    QVariant result;

   if (! isReady())
       return result;

   qDebug() << "exec" << q;
   QSqlQuery query = mSqlDatabase.exec(q);
   QSqlError error = query.lastError();
   return ! error.isValid();
}

QString Database::setupDatabase(const QString & name)
{
    QFileInfo info(name);
    QString destFile;
    QString path;
    QDir dir;
    bool ok;

    path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    dir = QDir(path);
    destFile = dir.absoluteFilePath(info.fileName());
    if (QFile::exists(destFile))
        return destFile;

    if (! dir.exists())
        dir.mkpath(".");

    ok = QFile::copy(name, destFile);
    if (ok)
        return destFile;

    return name;
}

void Database::addDatabase(Database * db)
{
    if (! db)
        return;

    QString connectionName = db->connectionName();
    if (mDatabaseConnections.contains(connectionName)) {
        DatabaseConnection* dbConnection = mDatabaseConnections.value(connectionName);
        dbConnection->addDatabase(db);
    }
    else {
        mDatabaseConnections.insert(connectionName, new DatabaseConnection(connectionName));
    }
}

void Database::removeDatabase(Database * db)
{
    if (! db)
        return;

    QString connectionName = db->connectionName();
    if (mDatabaseConnections.contains(connectionName)) {
        DatabaseConnection* dbConnection = mDatabaseConnections.value(connectionName);
        dbConnection->removeDatabase(db);
        if (! dbConnection->isActive()) {
            mDatabaseConnections.remove(connectionName);
            dbConnection->deleteLater();
        }
    }
}

DatabaseConnection* Database::connection(const QString & name)
{
    return mDatabaseConnections.value(name, 0);
}
