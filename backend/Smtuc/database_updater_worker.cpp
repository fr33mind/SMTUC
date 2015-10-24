#include "database_updater_worker.h"

#include <QSqlDatabase>
#include <QNetworkReply>
#include <QDebug>
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QHash>
#include <QHashIterator>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QDateTime>

#include "settings.h"

DatabaseUpdaterWorker::DatabaseUpdaterWorker(Database* db, QObject *parent) :
    QObject(parent)
{
    mFileDownloader = 0;
    mDatabase = db;
    mQueuedTaskCount = 0;
    mCompletedTaskCount = 0;
    connect(this, SIGNAL(error(const QString&)), this, SIGNAL(finished()));
}

DatabaseUpdaterWorker::~DatabaseUpdaterWorker()
{
    if (mFileDownloader)
        mFileDownloader->deleteLater();
}

void DatabaseUpdaterWorker::loadSeasons(const QByteArray& data, QSqlDatabase& db)
{
    if (! db.isValid())
        return;

    QSqlQuery query;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return;

    QJsonArray seasons = doc.array();
    qDebug() << "Season size:" << seasons.size();

    db.transaction();

    if (seasons.size()) {
        db.exec("DELETE FROM seasons");
        db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'seasons'");
    }

    for(int i=0; i < seasons.size(); i++) {
        QJsonValue value = seasons.at(i);
        if (! value.isObject())
            continue;
        QJsonObject season = value.toObject();

        query = QSqlQuery("", db);
        query.prepare("INSERT INTO seasons (id, name, start, end) VALUES(:id, :name, :start, :end)");
        query.bindValue(":id", season.value("id").toInt());
        query.bindValue(":name", season.value("epoca").toString());
        query.bindValue(":start", season.value("inicio").toString());
        query.bindValue(":end", season.value("termino").toString());
        query.exec();
    }

    db.commit();
}

QString DatabaseUpdaterWorker::routeNumber(const QString & _name)
{
    QRegularExpression pattern("[a-zA-Z ]");
    QString name = _name;
    if (name.contains(","))
        name = name.split(",").first();

    name.replace(pattern, "");
    return name;
}

void DatabaseUpdaterWorker::loadRoutes(const QByteArray& data, QSqlDatabase& db)
{
    if (! db.isValid())
        return;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return;

    QJsonArray routes = doc.array();
    QStringList allStops, stops;
    QHash<int, QStringList> routeStops;
    QSqlQuery query = QSqlQuery("", db);
    query.prepare("INSERT INTO routes (id, name, desc, favorite) VALUES(?, ?, ?, ?)");

    if (routes.isEmpty())
        return;

    db.transaction();

    qDebug() << "Deleting tables...";
    db.exec("DELETE FROM routes");
    db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'routes'");
    db.exec("DELETE FROM stops");
    db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'stops'");
    db.exec("DELETE FROM route_stops");
    db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'route_stops'");

    qDebug() << "Loading data";

    for(int i=0; i < routes.size(); i++) {
        QJsonValue value = routes.at(i);
        if (! value.isObject())
            continue;
        QJsonObject route = value.toObject();
        int id = route.value("ID_L").toInt();

        query = QSqlQuery("", db);
        query.prepare("INSERT INTO routes (id, name, desc, number, favorite) VALUES(?, ?, ?, ?, ?)");
        query.addBindValue(id);
        query.addBindValue(route.value("nome").toString().trimmed());
        query.addBindValue(route.value("descri").toString().trimmed());
        query.addBindValue(routeNumber(route.value("nome").toString()));
        query.addBindValue(0);
        query.exec();

        stops = parseStops(route.value("percurso"), allStops);
        loadStops(stops, db);
        allStops += stops;
        routeStops.insert(id, parseStops(route.value("percurso")));
    }

    db.commit();

    loadRouteStops(routeStops, db);
}

QHash<QString, int> DatabaseUpdaterWorker::stopsHash()
{
    QHash<QString, int> stop_name_ids;

    if (! mDatabase)
        return stop_name_ids;

    QSqlDatabase db = mDatabase->database();
    QSqlQuery query = db.exec("SELECT * from stops");
    QSqlRecord record;
    int id;
    QString name;

    if (query.isActive() && query.isSelect()) {
        while(query.next()) {
            record = query.record();
            id = record.value("id").toInt();
            name = record.value("name").toString();
            stop_name_ids.insert(name, id);
        }
    }

    return stop_name_ids;
}

void DatabaseUpdaterWorker::loadRouteStops(const QHash<int, QStringList> & routestops, QSqlDatabase & db)
{
    QSqlQuery query;
    QHash<QString, int> stop_name_ids = this->stopsHash();

    qDebug() << "stop ids count" << stop_name_ids.size();

    if (stop_name_ids.isEmpty())
        return;

    db.transaction();

    QHashIterator<int, QStringList> it(routestops);
    while(it.hasNext()) {
        it.next();

        qDebug() << "Loading stops for route" << it.key();

        QStringList stopnames = it.value();
        QList<int> route_stop_ids;
        for(int i=0; i < stopnames.size(); i++) {
            if (stop_name_ids.contains(stopnames.at(i)))
                route_stop_ids << stop_name_ids.value(stopnames.at(i), 0);
        }

        for(int i=0; i < route_stop_ids.size(); i++) {
            query = QSqlQuery("", db);
            query.prepare("INSERT INTO route_stops (id_route, id_stop) VALUES(?, ?)");
            query.addBindValue(it.key());
            query.addBindValue(route_stop_ids.at(i));
            query.exec();
        }
    }

    db.commit();
}

void DatabaseUpdaterWorker::loadRouteSchedule(const QJsonObject& schedule, QSqlDatabase& db)
{
    QSqlQuery query = QSqlQuery("", db);

    query.prepare("INSERT INTO route_schedules (id, id_route, id_season, id_stop, date, deactivated_at,\
                  last_modified_at, effective_date, obs, comment) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(schedule.value("id_h").toInt());
    query.addBindValue(schedule.value("id_l").toInt());
    query.addBindValue(schedule.value("id_e").toInt());
    query.addBindValue(schedule.value("id_stop").toInt());
    query.addBindValue(schedule.value("data").toString());
    query.addBindValue(schedule.value("data_desactiv").toString());
    query.addBindValue(schedule.value("data_ult_alter").toString());
    query.addBindValue(schedule.value("data_vigor").toString());
    query.addBindValue(schedule.value("observ").toString());
    query.addBindValue(schedule.value("comentario").toString());
    query.exec();
}

void DatabaseUpdaterWorker::loadRouteTimes(const QJsonObject& schedule, QSqlDatabase& db)
{
    QStringList times;
    int id_day;
    QString time_var("tempos%1");
    QSqlQuery query;
    QString obs;
    QString days_desc;

    for(int i=0; i < 3; i++) {
        id_day = i + 1;
        if (! schedule.contains(time_var.arg(id_day)))
            break;

        times = parseTimes(schedule.value(time_var.arg(id_day)));

        if (id_day == 1) {
            days_desc = schedule.value("descr_dias1").toString().trimmed().toLower();
            if (days_desc == "dias de feira dos 7 e 23")
                id_day = 4;
        }

        for(int j=0; j < times.size(); j++) {
            query = QSqlQuery("", db);
            query.prepare("INSERT INTO route_times (time, id_route, id_stop, id_day, id_season, obs, id_schedule)\
                          VALUES(?, ?, ?, ?, ?, ?, ?)");

            query.addBindValue(times.at(j));
            query.addBindValue(schedule.value("id_l").toInt());
            query.addBindValue(schedule.value("id_stop").toInt());
            query.addBindValue(id_day);
            query.addBindValue(schedule.value("id_e").toInt());
            query.addBindValue(""); //add obs!!
            query.addBindValue(schedule.value("id_h").toInt());
            query.exec();
        }
    }
}

void DatabaseUpdaterWorker::loadRouteSchedules(const QByteArray& data, QSqlDatabase& db)
{
    if (! db.isValid())
        return;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return;

    QJsonArray schedules = doc.array();
    QJsonObject schedule;
    QHash<QString, int> stops = this->stopsHash();
    qDebug() << "load route schedules";

    db.transaction();

    db.exec("DELETE FROM route_schedules");
    db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'route_schedules'");
    db.exec("DELETE FROM route_times");
    db.exec("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = 'route_times'");

    for(int i=0; i < schedules.size(); i++) {
        schedule = schedules.at(i).toObject();
        qDebug() << "loading schedule" << schedule.value("id_h").toInt();
        schedule.insert("id_stop", stops.value(schedule.value("LOCAL").toString(), 0));
        loadRouteSchedule(schedule, db);
        loadRouteTimes(schedule, db);
        //setMainStop();
    }

    db.commit();
}

QStringList DatabaseUpdaterWorker::parseTimes(const QJsonValue & time)
{
    QStringList times, parts;
    if (! time.isString())
        return times;

    QString t = time.toString().trimmed(),
            obs = "";

    if (t.startsWith(","))
        t.remove(0, 1);
    times = t.split(",");

    for(int i=0; i < times.size(); i++) {
        t = times.at(i);
        parts = t.split(" ");
        t = parts.size() ? parts.first() : "";
        obs = parts.size() >= 2 ? parts.at(1) : "";

        if (t.contains(".")) {
           t.truncate(t.indexOf("."));
           times[i] = t + " " + obs;
        }
    }

    return times;
}

QStringList DatabaseUpdaterWorker::parseStops(const QJsonValue & stops, const QStringList & checkedStops)
{
    if (! stops.isString())
        return QStringList();

    QStringList allstops = stops.toString().split(","),
                validstops;

    for(int i=0; i < allstops.size(); i++) {
        if (! checkedStops.contains(allstops.at(i)))
            validstops.append(allstops.at(i));
    }

    return validstops;
}

void DatabaseUpdaterWorker::loadStops(QStringList stops , QSqlDatabase& db)
{
    if (stops.isEmpty())
        return;

    stops.removeDuplicates();

    qDebug() << "Loading" << stops.size() << "stops";
    QSqlQuery query = QSqlQuery("", db);
    query.prepare("INSERT INTO stops (name) VALUES(?)");
    query.addBindValue(stops);
    query.execBatch();
}

void DatabaseUpdaterWorker::loadTickets(const QByteArray& data, QSqlDatabase& db)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return;

    qDebug() << "Loading tickets";

    db.transaction();
    db.exec("DELETE FROM tickets");

    QJsonArray tickets = doc.array();
    QSqlQuery query;
    QJsonObject ticket;
    QJsonValue val;

    for(int i=0; i < tickets.size(); i++) {
        val = tickets.at(i);
        if (! val.isObject())
            continue;

        ticket = val.toObject();
        query = QSqlQuery("", db);
        query.prepare("INSERT INTO tickets (id, name) VALUES(?, ?)");
        query.addBindValue(ticket.value("B_TIPO").toInt());
        query.addBindValue(ticket.value("desc"));
        query.exec();
    }

    loadTicketPrices(tickets.size(), db);

    db.commit();
}

void DatabaseUpdaterWorker::loadTicketPrices(int count, QSqlDatabase& db)
{
    QByteArray data;
    QString url;
    QJsonDocument doc;
    QJsonValue val;
    QJsonArray ticket_prices;

    qDebug() << "Loading ticket prices";

    db.exec("DELETE FROM ticket_prices");

    for(int i=0; i < count; i++) {
        url = QString(TICKET_PRICES_URL).arg(i);
        data = mFileDownloader->data(QUrl(url));
        doc = QJsonDocument::fromJson(data);
        if (! doc.isArray())
            continue;

        ticket_prices = doc.array();
        for(int j=0; j < ticket_prices.size(); j++) {
            qDebug() << "Loading tickets " << j;
            loadTicketPrice(ticket_prices.at(j), db);
        }
    }
}

void DatabaseUpdaterWorker::loadTicketPrice(const QJsonValue& val, QSqlDatabase& db)
{
    if (! val.isObject())
        return;

    QJsonObject data = val.toObject();

    QSqlQuery query = QSqlQuery("", db);
    query.prepare("INSERT INTO ticket_prices (name, total_price, trip_price, id_type, `desc`, `order`) \
                  VALUES(?, ?, ?, ?, ?, ?) ");

    query.addBindValue(data.value("B_NOME"));
    query.addBindValue(data.value("B_PRECO_TOTAL"));
    query.addBindValue(data.value("B_PRECO_VIAGEM"));
    query.addBindValue(data.value("B_TIPO").toInt());
    query.addBindValue(data.value("B_DESCRI"));
    query.addBindValue(data.value("b_order").toInt());
    query.exec();
}

void DatabaseUpdaterWorker::loadOutlets(const QByteArray& data, QSqlDatabase& db)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return;

    qDebug() << "Loading outlets";

    db.transaction();
    db.exec("DELETE FROM outlets");
    QJsonArray outlets = doc.array();

    for(int i=0; i < outlets.size(); i++) {
        loadOutlet(outlets.at(i), db);
    }

    db.commit();
}

void DatabaseUpdaterWorker::loadOutlet(const QJsonValue & val, QSqlDatabase& db)
{
    if (! val.isObject())
        return;

    QJsonObject outlet = val.toObject();
    QSqlQuery query("", db);
    query.prepare("INSERT INTO outlets (name, normal_hour, weekend_hour, address, phone,\
                  freguesia, coordinates, zone, official) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(outlet.value("p_nome").toString());
    query.addBindValue(outlet.value("p_hora_normal").toString().trimmed());
    query.addBindValue(outlet.value("p_hora_fds").toString().trimmed());
    query.addBindValue(outlet.value("p_morada").toString());
    query.addBindValue(outlet.value("p_telefone").toString());
    query.addBindValue(outlet.value("p_freguesia").toString());
    query.addBindValue(outlet.value("p_referencia").toString());
    query.addBindValue(outlet.value("p_zona").toString());
    query.addBindValue(outlet.value("p_oficial").toInt());
    query.exec();
}

void DatabaseUpdaterWorker::load()
{
    bool ticketsError = false;
    if (! mFileDownloader || ! mDatabase)
        return;

    if (! hasTicketPrices()) {
        bool started = downloadTicketPrices();
        ticketsError = ! started;
        if (started)
            return;
    }

    if (mFileDownloader->hasErrors() || ticketsError) {
        emit error(tr("Error transfering files."));
        return;
    }

    QSqlDatabase db = mDatabase->database();
    if (! db.isValid())
        return;

    setStatusMessage(tr("Installing..."));
    setTaskCount(5);

    QByteArray data;
    data = mFileDownloader->data(QUrl(SEASONS_URL));
    //data = this->loadData("~/epocas.json");
    loadSeasons(data, db);
    taskComplete();

    data = mFileDownloader->data(QUrl(ROUTES_URL));
    //data = this->loadData("~/linhaspercurso.json");
    loadRoutes(data, db);
    taskComplete();

    data = mFileDownloader->data(QUrl(ROUTE_TIMES_URL));
    //data = this->loadData("~/horariostempos.json");
    loadRouteSchedules(data, db);
    taskComplete();

    data = mFileDownloader->data(QUrl(TICKETS_URL));
    //data = this->loadData("~/horariostempos.json");
    loadTickets(data, db);
    taskComplete();

    data = mFileDownloader->data(QUrl(OUTLETS_URL));
    loadOutlets(data, db);
    taskComplete();

    QDateTime today = QDateTime::currentDateTimeUtc();
    Settings settings(mDatabase);
    settings.setValue("updatedAt", today.toString(Qt::ISODate));

    mFileDownloader->deleteLater();
    mFileDownloader = 0;
    emit finished();
}

QByteArray DatabaseUpdaterWorker::loadData(const QString & filepath)
{
    QString path = filepath;
    path.replace("~/", QDir::homePath() + "/");

    QFile file(path);
    QByteArray data;

    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return data;
    return file.readAll();
}

void DatabaseUpdaterWorker::start()
{
    if (mFileDownloader)
        mFileDownloader->deleteLater();

    qDebug() << "Updating...";

    setStatusMessage(tr("Downloading..."));
    setTaskCount(5);

    mFileDownloader = new FileDownloader;
    connect(mFileDownloader, SIGNAL(finished()), this, SLOT(load()));
    connect(mFileDownloader, SIGNAL(replyFinished(QNetworkReply*)), this, SLOT(taskComplete()));
    mFileDownloader->addUrl(QUrl(SEASONS_URL));
    mFileDownloader->addUrl(QUrl(ROUTES_URL));
    mFileDownloader->addUrl(QUrl(ROUTE_TIMES_URL));
    mFileDownloader->addUrl(QUrl(TICKETS_URL));
    mFileDownloader->addUrl(QUrl(OUTLETS_URL));
    bool started = mFileDownloader->start();
    if (! started)
        emit error(tr("Network not available."));
}

double DatabaseUpdaterWorker::progress() const
{
    int total = mQueuedTaskCount + mCompletedTaskCount;
    return (mCompletedTaskCount * 1.0) / total;
}

void DatabaseUpdaterWorker::setTaskCount(int count)
{
    mQueuedTaskCount = count;
    mCompletedTaskCount = 0;
}

QString DatabaseUpdaterWorker::statusMessage() const
{
    return mStatusMessage;
}

void DatabaseUpdaterWorker::setStatusMessage(const QString & msg)
{
    mStatusMessage = msg;
    emit statusMessageChanged();
}

bool DatabaseUpdaterWorker::downloadTicketPrices()
{
    if (! mFileDownloader)
        return false;

    qDebug() << "Downloading ticket prices...";
    QByteArray data = mFileDownloader->data(QUrl(TICKETS_URL));
    if (data.isEmpty())
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (! doc.isArray())
        return false;

    QJsonArray tickets = doc.array();
    QString url;

    for(int i=0; i < tickets.size(); i++) {
        url = QString(TICKET_PRICES_URL).arg(i);
        mFileDownloader->addUrl(QUrl(url));
    }
    bool started = mFileDownloader->start();
    if (! started)
        emit error(tr("Network not available."));
    return started;
}

bool DatabaseUpdaterWorker::hasTicketPrices() const
{
    QString firstUrl = QString(TICKET_PRICES_URL).arg("0");
    return mFileDownloader->hasUrl(firstUrl);
}

void DatabaseUpdaterWorker::replyFinished(QNetworkReply *reply)
{
    qDebug() << "reply finished";
    if (!hasTicketPrices() && reply && reply->url() == QUrl(TICKETS_URL)) {
        downloadTicketPrices();
    }

    taskComplete();
}

void DatabaseUpdaterWorker::taskComplete()
{
    if (mQueuedTaskCount > 0) {
        mQueuedTaskCount--;
        mCompletedTaskCount++;
        emit progressChanged();
    }
}
