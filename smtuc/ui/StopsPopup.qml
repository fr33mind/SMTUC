import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import Smtuc 1.0

Popup {
    id: popup
    property int routeId: 0
    property var mainStops: []
    title: i18n.tr("Stops")

    SqlQueryModel {
        id: stopsModel
        database: Database {}
        query: "select id_stop, stops.name as stop_name from route_stops" +
               " inner join stops " +
               " on route_stops.id_stop = stops.id " +
               " where id_route = " + popup.routeId +
               " order by route_stops.id asc"
    }

    SqlQueryModel {
        id: mainStopsModel
        database: Database {}
        query: "select id_stop from route_schedules where id_route = " + popup.routeId +
               " group by id_stop"
        onQueryChanged:  {
            var mainStops = [];
            mainStops.push(this.recordField(0, "id_stop"));
            mainStops.push(this.recordField(1, "id_stop"));
            popup.mainStops = mainStops;
        }
    }

    UbuntuListView {
        model: stopsModel
        width: parent.width
        height: popup.availableHeight
        clip: true
        delegate: ListItem.Standard {
            Label {
                anchors.fill: parent
                anchors.leftMargin: units.gu(1)
                text: stop_name
                horizontalAlignment: Text.AlignJustify
                font.bold: popup.mainStops.indexOf(id_stop) != -1 ? true : false
            }
            showDivider: false
            height: units.gu(3)
        }

    }
}
