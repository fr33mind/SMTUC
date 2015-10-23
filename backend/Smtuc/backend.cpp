#include <QtQml>
#include <QtQml/QQmlContext>
#include "backend.h"
#include "database.h"
#include "sqlquerymodel.h"
#include "database_updater.h"
#include "settings.h"

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Smtuc"));

    qmlRegisterType<Database>(uri, 1, 0, "Database");
    qmlRegisterType<DatabaseUpdater>(uri, 1, 0, "DatabaseUpdater");
    qmlRegisterType<SqlQueryModel>(uri, 1, 0, "SqlQueryModel");
    qmlRegisterType<Settings>(uri, 1, 0, "Settings");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

