import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import Smtuc 1.0

MessagePopup {
    id: dialog
    title: i18n.tr("Observations")

    property int routeId: 0
    property int seasonId: 0

    SqlQueryModel {
        id: obsModel
        database: Database {}
        query: "select * from route_schedules where id_route = " + dialog.routeId +
               " and id_season = " + dialog.seasonId + " and length(obs) > 0"
        onQueryChanged: {
            var obs = this.recordField(0, "obs");
            if (obs)
                dialog.message = obs;
        }
    }
}

