#include "database_connection.h"

DatabaseConnection::DatabaseConnection(const QString& name, QObject *parent) :
    QObject(parent)
{
    mName = name;
}

QString DatabaseConnection::name() const
{
    return mName;
}

void DatabaseConnection::addDatabase(Database * db)
{
    if (! db || mDatabases.contains(db))
        return;

    connect(this, SIGNAL(databaseUpdated()), db, SIGNAL(updated()), Qt::UniqueConnection);
    mDatabases.append(db);
}

void DatabaseConnection::removeDatabase(Database * db)
{
    if (! db || ! mDatabases.contains(db))
        return;

    disconnect(db);
    mDatabases.removeOne(db);
}

bool DatabaseConnection::isActive() const
{
    return ! mDatabases.isEmpty();
}
