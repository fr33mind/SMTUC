#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QObject>

#include "database.h"

class Database;

class DatabaseConnection : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseConnection(const QString& name="",QObject *parent = 0);
    QString name() const;
    void addDatabase(Database*);
    void removeDatabase(Database*);
    bool isActive() const;

signals:
    void databaseUpdated();

public slots:

private:
    QString mName;
    QList<Database*> mDatabases;

};

#endif // DATABASECONNECTION_H
