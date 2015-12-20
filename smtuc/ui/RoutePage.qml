import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1
import Smtuc 1.0

Page {

    property int routeId
    property string routeName
    property int favorite: 0
    property var routeMainStop1: ({})
    property var routeMainStop2: ({})
    property int seasonIndex: 0
    property int seasonId: 0
    property int dayId: 1
    property int dayIndex: 0

    title: i18n.tr("Route %1").arg(routeName)
    id: routePage
    anchors.right: parent.right
    anchors.leftMargin: 4
    anchors.rightMargin: 4
    state: "default"

    function getTodayId() {
        var date = new Date(),
            day = date.getDay();

        if (day == 0)
            return 3;
        else if (day == 6)
            return 2;

        return 1;
    }

    onSeasonIndexChanged: {
        if (seasonIndex != seasonSelector.selectedIndex)
            seasonSelector.selectedIndex = seasonIndex;
    }

    onDayIndexChanged: {
        if (dayIndex != daySelector.selectedIndex)
            daySelector.selectedIndex = dayIndex;
    }

    Component {
        id: obsComponent
        ObservationsPopup {
            routeId: routePage.routeId
            seasonId: routePage.seasonId
        }
    }

    Component {
        id: stopsComponent
        StopsPopup {
            routeId: routePage.routeId
        }
    }

    head.sections.model: []
    head.actions: [
        Action {
            iconSource: "../images/bus.svg"
            text: i18n.tr("Stops")
            onTriggered: {
                PopupUtils.open(stopsComponent)
            }
        },
        Action {
            id: action
            iconName: "info"
            text: i18n.tr("Obs.")
            onTriggered: {
                PopupUtils.open(obsComponent)
            }
        },
        Action {
            iconName: routePage.favorite ? "starred" : "non-starred"
            text: i18n.tr("Favorite")
            onTriggered: {
                var favorite = routePage.favorite ? 0 : 1;
                var result = defaultDatabase.exec("UPDATE routes set favorite = " + favorite + " where id = " + routePage.routeId);
                if (result)
                    routePage.favorite = favorite;
            }
        }
    ]

    Database {
        id: defaultDatabase
    }

    SqlQueryModel {
        id: routeDataModel
        database: defaultDatabase
        query: "select * from routes where id = " + routePage.routeId
        onQueryChanged: {
            routePage.routeName = "";
            routePage.favorite = 0;
            if (routePage.routeId > 0) {
                routePage.routeName = this.recordField(0, "name") + "";
                routePage.favorite = this.recordField(0, "favorite");
            }
        }
    }

    SqlQueryModel {
        id: seasonsDataModel
        database: defaultDatabase
        query: " select id, name, " +
               " (strftime('%s', end) - strftime('%s','now')) as time_left, " +
               " (strftime('%s', 'now') - strftime('%s', start)) as time_passed " +
               " from seasons " +
               " where id in (select id_season from route_schedules where id_route = " + routePage.routeId +")"

        onQueryChanged: {
            var seasonIndex = 0,
                seasonId = 0;

            for(var i=0; i < this.rowCount; i++) {
                var time_left = this.recordField(i, "time_left");
                var time_passed = this.recordField(i, "time_passed");
                if (time_left >= 0 && time_passed >= 0) {
                    seasonId = this.recordField(i, "id");
                    seasonIndex = i;
                    break;
                }
            }

            if (! seasonId && this.rowCount) {
                seasonId = this.recordField(0, "id");
                seasonIndex = 0;
            }

            routePage.seasonId = seasonId;
            routePage.seasonIndex = seasonIndex;
        }
    }

    SqlQueryModel {
        id: daysDataModel
        database: defaultDatabase
        query: " select * from days " +
               " where id in (select id_day from route_times where id_route = " + routePage.routeId +
               " group by id_day )"

        onQueryChanged: {
            var todayId = getTodayId(),
                dayId = 0,
                dayIndex = 0;

            for(var i=0; i < this.rowCount; i++) {
                var id = parseInt(this.recordField(i, "id"));
                if (todayId == id) {
                    dayId = id;
                    dayIndex = i;
                    break;
                }
            }

            if (! dayId && this.rowCount) {
                dayId = parseInt(this.recordField(0, "id"));
                dayIndex = 0;
            }

            routePage.dayId = dayId;
            routePage.dayIndex = dayIndex;
        }
    }

    SqlQueryModel {
        id: routeMainStops
        database: defaultDatabase
        query: " select * from stops " +
               " where id in (select id_stop from route_schedules where id_route = " + routePage.routeId +
               " group by id_stop )"
        onQueryChanged: {
            routePage.routeMainStop1 = this.rowDataMap(0)
            routePage.routeMainStop2 = this.rowDataMap(1)
        }
    }

    SqlQueryModel {
        property int stopId: {
            if (routePage.routeMainStop1["id"])
                return routePage.routeMainStop1["id"]
            return 0
        }

        id: routeTimes1
        database: defaultDatabase
        query: " select time from route_times " +
               " where id_route = " + routePage.routeId + " and id_stop = " + stopId +
               " and id_season = " + routePage.seasonId + " and id_day = " + routePage.dayId
    }

    SqlQueryModel {
        property int stopId: {
            if (routePage.routeMainStop2["id"])
                return routePage.routeMainStop2["id"]
            return 0
        }

        id: routeTimes2
        database: defaultDatabase
        query: " select time from route_times " +
               " where id_route = " + routePage.routeId + " and id_stop = " + stopId +
               " and id_season = " + routePage.seasonId + " and id_day = " + routePage.dayId
    }

    Item {
        id: container
        anchors.fill: parent
        anchors.margins: units.gu(0.5)

        Column {
            id: comboColumn
            spacing: units.gu(0.5)
            width: parent.width
            height: childrenRect.height

            OptionSelector {
                id: seasonSelector
                expanded: false
                containerHeight: itemHeight * model.rowCount
                model: seasonsDataModel
                selectedIndex: routePage.seasonIndex
                delegate: OptionSelectorDelegate {
                    text: name
                }
                onDelegateClicked: {
                    if (routePage.seasonIndex != index) {
                        routePage.seasonIndex = index;
                        routePage.seasonId = parseInt(this.model.recordField(this.selectedIndex, "id"));
                    }
                }
            }

            OptionSelector {
                id: daySelector
                expanded: false
                containerHeight: itemHeight * model.rowCount
                model: daysDataModel
                selectedIndex: routePage.dayIndex

                delegate: OptionSelectorDelegate {
                    text: name
                }
                onDelegateClicked: {
                    if (routePage.dayIndex != index) {
                        routePage.dayIndex = index;
                        routePage.dayId = parseInt(this.model.recordField(index, "id"));
                    }
                }
            }
        }

        Row {
            anchors.top: comboColumn.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(0.5)
            clip: true

            Rectangle {
                width: parent.width / 2
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                color: "transparent"

               Label {
                   id: listLabel
                   text: routePage.routeMainStop1["name"] + ""
                   anchors.left: parent.left
                   anchors.right: parent.right
                   anchors.leftMargin: units.gu(1)
                   height: {
                       if (listLabel2.implicitHeight > this.implicitHeight)
                           return listLabel2.implicitHeight
                       return this.implicitHeight
                   }
                   wrapMode: Text.WordWrap
                   horizontalAlignment: Text.AlignLeft
                   verticalAlignment: Text.AlignVCenter
                   font.bold: true
               }

               UbuntuListView {
                   id: timesList1
                   model: routeTimes1
                   anchors.topMargin: units.gu(0.5)
                   anchors.leftMargin: units.gu(2)
                   anchors.top: listLabel.bottom
                   anchors.bottom: parent.bottom
                   anchors.left: parent.left
                   anchors.right: parent.right
                   clip: true
                   delegate: ListItem.Standard {
                       Label {
                           anchors.fill: parent
                           text: time.indexOf("_") != -1 ? time.replace("_", "") : time
                           font.underline: time.indexOf("_") != -1 ? true : false
                           horizontalAlignment: Text.AlignJustify
                           verticalAlignment: Text.AlignVCenter
                       }
                       showDivider: false
                       height: units.gu(3)
                       onClicked: {
                           if (time.indexOf(" ") != -1 && time.split(" ")[1].length)
                            PopupUtils.open(obsComponent)
                       }
                   }
               }
            }

            Rectangle {
                width: parent.width / 2
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                color: "transparent"

               Label {
                   id: listLabel2
                   text: routePage.routeMainStop2["name"] + ""
                   anchors.left: parent.left
                   anchors.right: parent.right
                   anchors.leftMargin: units.gu(1)
                   wrapMode: Text.WordWrap
                   height:  {
                       if (listLabel.implicitHeight > this.implicitHeight)
                           return listLabel.implicitHeight
                       return this.implicitHeight
                   }
                   horizontalAlignment: Text.AlignLeft
                   verticalAlignment: Text.AlignVCenter
                   font.bold: true
               }

               UbuntuListView {
                   id: timesList2
                   model: routeTimes2
                   width: parent.width
                   anchors.topMargin: units.gu(0.5)
                   anchors.leftMargin: units.gu(2)
                   anchors.top: listLabel2.bottom
                   anchors.bottom: parent.bottom
                   anchors.left: parent.left
                   anchors.right: parent.right 
                   clip: true
                   delegate: ListItem.Standard {
                       Label {
                           anchors.fill: parent
                           text: time.indexOf("_") != -1 ? time.replace("_", "") : time
                           font.underline: time.indexOf("_") != -1 ? true : false
                           horizontalAlignment: Text.AlignJustify
                           verticalAlignment: Text.AlignVCenter
                       }
                       showDivider: false
                       height: units.gu(3)
                       onClicked: {
                          if (time.indexOf(" ") != -1 && time.split(" ")[1].length)
                            PopupUtils.open(obsComponent)
                       }
                   }
               }
            }
        }
    }
}
