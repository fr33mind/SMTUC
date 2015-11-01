#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlDatabase>

#include "database.h"

class SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY( Database* database READ database WRITE setDatabase )
    Q_PROPERTY( QString query READ queryString WRITE setQuery NOTIFY queryChanged )
    Q_PROPERTY( int rowCount READ rowCount NOTIFY rowCountChanged )

public:
    SqlQueryModel(QObject *parent = 0);
    Q_INVOKABLE QVariant recordField(int, const QString&);
    Q_INVOKABLE QVariantMap rowDataMap(int) const;
    virtual QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual void refresh();
    virtual QHash<int,QByteArray> roleNames() const;

signals:
    void rowCountChanged();
    void queryChanged();

private slots:
    void onDatabaseDestroyed();

protected:
    Database* database();
    void setDatabase(Database*);
    QString queryString();

    void setQuery(const QString &);

private:
    Database* mDatabase;
    QString mQuery;
    QHash<int, QByteArray> mRoleNames;

    void updateRoleNames();
};

#endif // SQLQUERY_H
