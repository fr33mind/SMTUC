#include "sqlquerymodel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

SqlQueryModel::SqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    mDatabase = 0;
}

Database* SqlQueryModel::database()
{
    return mDatabase;
}

void SqlQueryModel::setDatabase(Database* db)
{
    if (mDatabase == db)
        return;

    mDatabase = db;
    if (mDatabase) {
        connect(mDatabase, SIGNAL(updated()), this, SLOT(refresh()), Qt::UniqueConnection);
        connect(mDatabase, SIGNAL(destroyed()), this, SLOT(onDatabaseDestroyed()), Qt::UniqueConnection);
    }

    if (! mQuery.isEmpty())
        setQuery(mQuery);
}

QString SqlQueryModel::queryString()
{
    return mQuery;
}

void SqlQueryModel::setQuery(const QString & q)
{
    mQuery = q;
    if (! mDatabase)
        return;

    QSqlDatabase db = mDatabase->database();
    if (! db.isValid() || ! db.isOpen())
        return;

    QSqlQueryModel::setQuery(q, db);
    updateRoleNames();

    emit rowCountChanged();
    emit queryChanged();
}

void SqlQueryModel::updateRoleNames()
{
    mRoleNames.clear();
    QSqlRecord record = this->record();
    for(int i=0; i < record.count(); i++) {
        mRoleNames.insert(Qt::UserRole+i+1, record.field(i).name().toLatin1());
    }
}

QHash<int,QByteArray> SqlQueryModel::roleNames() const
{
    QHash<int, QByteArray> rolenames = QSqlQueryModel::roleNames();
    rolenames.unite(mRoleNames);
    return rolenames;
}

QVariant SqlQueryModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if(role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    }
    else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }

    return value;
}

void SqlQueryModel::refresh()
{
    setQuery(mQuery);
}

QVariant SqlQueryModel::recordField(int row, const QString & name)
{
    QSqlRecord record = this->record(row);
    if (record.isEmpty())
        return "";

    QSqlField field = record.field(name);
    if (field.isValid())
        return field.value();
    return "";
}

QVariantMap SqlQueryModel::rowDataMap(int row) const
{
    QVariantMap data;

    QSqlRecord record = this->record(row);
    if (record.isEmpty())
        return data;

    for(int i=0; i < record.count(); i++)
        data.insert(record.fieldName(i), record.value(i));
    return data;
}

void SqlQueryModel::onDatabaseDestroyed()
{
    mDatabase = 0;
}

QString SqlQueryModel::normalize(const QString & text)
{
    QString t = text.toLower();
    t.replace("a", "[aáàãâ]");
    t.replace("e", "[eéèê]");
    t.replace("i", "[iíì]");
    t.replace("o", "[oóòõô]");
    t.replace("u", "[uúù]");
    t.replace("c", "[cç]");
    return t;
}
