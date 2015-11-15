#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QQmlParserStatus>

#include "filedownloader.h"
#include "database_connection.h"

class DatabaseConnection;

class Database : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY( QString databaseName READ databaseName WRITE setDatabaseName )

public:
    explicit Database(QObject *parent = 0);
    virtual ~Database();

    void classBegin();
    void componentComplete();

    QString databaseName() const;
    void setDatabaseName(const QString&);
    QSqlDatabase database() const;
    QString connectionName() const;
    DatabaseConnection* connection() const;
    Q_INVOKABLE QString test();
    Q_INVOKABLE QVariant exec(const QString&);
    bool isReady() const;

    static void addDatabase(Database*);
    static void removeDatabase(Database*);
    static DatabaseConnection* connection(const QString&);

Q_SIGNALS:
    void updated();
    void destroyed(Database*);

protected:
    void load();

private:
    QSqlDatabase mSqlDatabase;
    DatabaseConnection* mConnection;
    QString setupDatabase(const QString&);
};

#endif // MYTYPE_H

