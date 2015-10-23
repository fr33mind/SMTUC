#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QEvent>
#include <QQmlParserStatus>

#include "database.h"

class Settings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY( Database* database READ database WRITE setDatabase )
    Q_PROPERTY( bool autoSave READ autoSave WRITE setAutoSave )

public:
    explicit Settings(QObject *parent = 0);
    Settings(Database *db, QObject *parent = 0);
    virtual ~Settings();
    void setDatabase(Database*);
    Database* database() const;

    void setAutoSave(bool);
    bool autoSave() const;
    bool load();
    bool save();

    Q_INVOKABLE void setValue(const QString&, const QVariant&);
    Q_INVOKABLE QVariant value(const QString&, const QVariant& defaultValue=QVariant()) const;
    Q_INVOKABLE bool contains(const QString&) const;
    Q_INVOKABLE void remove(const QString&);
    Q_INVOKABLE void clear();

    void classBegin();
    void componentComplete();
    QVariantMap properties() const;

signals:

private slots:
    void onDatabaseDestroyed();

private:
    Database* mDatabase;
    bool mAutoSave;

    void init();
    bool _saveOne(const QString&, const QVariant&, const QSqlDatabase&);
    bool saveOne(const QString&, const QVariant&);
    bool deleteOne(const QString&);
    bool deleteAll();
};

#endif // SETTINGS_H
