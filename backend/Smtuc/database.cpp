#include "database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QSqlDriver>

static Database* mInstance = 0;
static int mRefCount = 0;

Database::Database(QObject *parent) :
    QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::database();
    if (! db.isValid())
        db = QSqlDatabase::addDatabase("QSQLITE");
    mSqlDatabase = db;
    if (mSqlDatabase.isValid() && ! mSqlDatabase.isOpen())
        mSqlDatabase.open();

    mRefCount++;
    //mInstance = this;
}

Database::~Database()
{
    mRefCount--;
    if (mRefCount == 0 && mSqlDatabase.isOpen())
        mSqlDatabase.close();
}

Database* Database::current()
{
    return mInstance;
}

QSqlDatabase Database::database() const
{
    return mSqlDatabase;
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
    qDebug() << "set name " << name;
    mSqlDatabase.setDatabaseName(name);
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
