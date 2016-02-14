#ifndef SETTINGS_HUB_H
#define SETTINGS_HUB_H

#include <QObject>
#include "settings.h"

class SettingsHub : public QObject
{
    Q_OBJECT
public:
    explicit SettingsHub(QObject *parent = 0);
    void add(Settings*);
    void remove(Settings*);

signals:

public slots:
    void notifyValue(const QString&, const QVariant&);

private:
    QSet<Settings*> mSettings;

};

#endif // SETTINGS_HUB_H
