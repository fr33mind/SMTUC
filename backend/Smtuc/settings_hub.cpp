#include "settings_hub.h"

SettingsHub::SettingsHub(QObject *parent) :
    QObject(parent)
{
}

void SettingsHub::add(Settings * settings)
{
    if (! settings)
        return;

    mSettings.insert(settings);
    connect(settings, SIGNAL(valueChanged(const QString&, const QVariant&)), this, SLOT(notifyValue(const QString&, const QVariant&)));
}

void SettingsHub::remove(Settings * settings)
{
    if (mSettings.contains(settings))
        mSettings.remove(settings);
}

void SettingsHub::notifyValue(const QString & key, const QVariant & value)
{
    Settings* sender = qobject_cast<Settings*>(this->sender());
    if (! sender)
        return;

    QSetIterator<Settings*> it(mSettings);
    bool autoSave = false;
    Settings* settings = 0;
    while(it.hasNext()) {
        settings = it.next();
        if (settings != sender) {
            //avoid unnecessary saving
            autoSave = settings->autoSave();
            settings->setAutoSave(false);
            settings->setValue(key, value);
            settings->setAutoSave(autoSave);
        }
    }
}
