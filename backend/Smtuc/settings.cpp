#include "settings.h"

#include <QMetaProperty>
#include <QQmlProperty>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    mDatabase = 0;
}

Settings::Settings(Database* db, QObject *parent) :
    QObject(parent)
{
    mDatabase = 0;
    setDatabase(db);
    init();
    load();
}

Settings::~Settings()
{
    save();
}

void Settings::init()
{
}

bool Settings::load()
{
    if (! mDatabase || !mDatabase->isReady())
        return false;

    QSqlQuery query("select * from settings", mDatabase->database());
    QSqlField nameField;
    QSqlField valueField;
    QString name;

    while(query.next()) {
        QSqlRecord record = query.record();
        nameField = record.field("name");
        valueField = record.field("value");
        name = nameField.value().toString();

        setProperty(name.toLatin1().data(), valueField.value());
    }

    return true;
}

void Settings::classBegin()
{
}

void Settings::componentComplete()
{
    init();
    load();

    //save to DB the declared properties in QML
    if (autoSave())
        save();
}

QVariantMap Settings::properties() const
{
    const QMetaObject * metaObj = this->metaObject();
    QMetaProperty prop;
    QVariant data;
    QVariantMap properties;

    for(int i=0; i < metaObj->propertyCount(); i++) {
       prop = metaObj->property(i);
       if (prop.isValid() && QString(prop.name()) != "objectName") {
           data = QQmlProperty::read(this, prop.name());
           if (data.canConvert(QVariant::String))
               properties.insert(prop.name(), data);
       }
    }

    QList<QByteArray> names = dynamicPropertyNames();
    QByteArray name;
    QVariant value;

    for(int i=0; i < names.size(); i++) {
        name = names.at(i);
        value = this->property(name.data());
        properties.insert(name, value);
    }

    return properties;
}

void Settings::setDatabase(Database * db)
{
    mDatabase = db;
    if (mDatabase)
        connect(mDatabase, SIGNAL(destroyed()), this, SLOT(onDatabaseDestroyed()));
}

Database* Settings::database() const
{
    return mDatabase;
}

void Settings::setAutoSave(bool autoSave)
{
    mAutoSave = autoSave;
}

bool Settings::autoSave() const
{
    return mAutoSave;
}

bool Settings::save()
{
    if (! mDatabase || ! mDatabase->isReady())
        return false;

    QSqlDatabase db = mDatabase->database();
    QVariantMap properties = this->properties();
    QMapIterator<QString, QVariant> it(properties);

    db.transaction();

    while(it.hasNext()) {
        it.next();
        _saveOne(it.key(), it.value(), db);
    }

    return db.commit();
}

bool Settings::saveOne(const QString & key, const QVariant & value)
{
    if (! mDatabase || ! mDatabase->isReady())
        return false;

    return _saveOne(key, value, mDatabase->database());
}

bool Settings::_saveOne(const QString & key, const QVariant & value, const QSqlDatabase& db)
{
    QString q("INSERT OR REPLACE INTO settings (`name`, `value`) VALUES(?, ?)");
    QSqlQuery query("", db);

    query.prepare(q);
    query.addBindValue(key);
    query.addBindValue(value);

    return query.exec();
}

void Settings::setValue(const QString & key, const QVariant & value)
{
    setProperty(key.toLatin1().data(), value);
    if (autoSave())
        saveOne(key, value);
}

QVariant Settings::value(const QString & key, const QVariant & defaultValue) const
{
    QQmlProperty property((QObject*) this, key);
    if (property.isValid())
        return property.read();
    QList<QByteArray> names = dynamicPropertyNames();
    if (names.contains(key.toLatin1()))
        return this->property(key.toLatin1().data());
    return defaultValue;
}

bool Settings::contains(const QString & key) const
{
    QVariant value = this->value(key, QVariant::fromValue((QObject*)this));
    //if we got back the default value, it means the key doesn't exist
    if (value.type() == QMetaType::QObjectStar && value.value<QObject*>() == this)
        return false;
    return true;
}

void Settings::clear()
{
    QVariantMap properties = this->properties();
    QMapIterator<QString, QVariant> it(properties);

    while(it.hasNext()) {
        it.next();
        QQmlProperty::write(this, it.key(), QVariant());
    }

    if (autoSave())
        deleteAll();
}

bool Settings::deleteAll()
{
    if (! mDatabase || !mDatabase->isReady())
        return false;

    QString q("DELETE FROM settings");
    QSqlQuery query("", mDatabase->database());
    return query.exec(q);
}

void Settings::remove(const QString & key)
{
    if (contains(key)) {
        QQmlProperty::write(this, key, QVariant());
        if (autoSave())
            deleteOne(key);
    }
}

bool Settings::deleteOne(const QString & key)
{
    if (! mDatabase || mDatabase->isReady())
        return false;

    QString q("DELETE FROM settings where name = '?'");
    QSqlQuery query("", mDatabase->database());
    query.prepare(q);
    query.addBindValue(key);

    return query.exec();
}

void Settings::onDatabaseDestroyed()
{
    mDatabase = 0;
}
