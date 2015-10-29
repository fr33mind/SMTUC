#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>

#include "filedownloader.h"

class Database : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString databaseName READ databaseName WRITE setDatabaseName )

public:
    explicit Database(QObject *parent = 0);
    ~Database();
    QString databaseName() const;
    void setDatabaseName(const QString&);
    QSqlDatabase database() const;
    Q_INVOKABLE QString test();
    Q_INVOKABLE QVariant exec(const QString&);
    bool isReady() const;
    static Database* current();

Q_SIGNALS:
    void helloWorldChanged();

protected:
    void load();

private:
    QSqlDatabase mSqlDatabase;

    QString setupDatabase(const QString&);
};

#endif // MYTYPE_H

