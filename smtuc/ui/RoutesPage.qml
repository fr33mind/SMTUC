import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import QtQuick.Layouts 1.1
import Smtuc 1.0

Page {
    id: routesPage
    title: i18n.tr("Routes")
    flickable: null

    property string filter: searchInput.displayText
    property string activeFilter: ""
    property var filteredIds: []
    property int viewSelected: head.sections.selectedIndex
    signal routeClicked(int id)

    head.sections.model: [i18n.tr("All"), i18n.tr("Favorites")]

    function getStopIds(text)
    {
        var normText = searchModel.normalize(text);
        searchModel.query = "select * from stops where lower(name) glob '*"+normText+"*'";
        var stops = [];
        for(var i=0; i < searchModel.rowCount; i++) {
            stops.push(searchModel.recordField(i, "id"));
        }

        return stops;
    }

    function getRouteStopIds(stops)
    {
        var routes = [];
        if (! stops.length)
            return routes;

        searchModel.query = "select * from route_stops where id_stop in ("+stops.join(",")+")" +
                            " group by id_route";

        for(var i=0; i < searchModel.rowCount; i++) {
            routes.push(searchModel.recordField(i, "id_route"));
        }

        return routes;
    }

    function search(text) {
        var filteredIds = [];

        if (text.length) {
            var normText = searchModel.normalize(text);
            searchModel.query = "select * from routes where lower(name) glob '*"+normText+"*' or " +
                                " lower(desc) glob '*"+normText+"*' ";

            for(var i=0; i < searchModel.rowCount; i++) {
                filteredIds.push(searchModel.recordField(i, "id"));
            }

            var stopIds = getStopIds(text);
            var routeIds = getRouteStopIds(stopIds);

            for(var i=0; i < routeIds.length; i++) {
                if (filteredIds.indexOf(routeIds[i]) == -1)
                    filteredIds.push(routeIds[i]);
            }
        }

        this.filteredIds = filteredIds;
        this.activeFilter = this.filter;
    }

    SqlQueryModel {
        id: searchModel
        database: Database{}
    }

    SqlQueryModel {
        id: routesModel
        database: Database {}
        query:  {
            var favoritesCondition = " favorite >= 0 ";
            if (routesPage.viewSelected == 1)
                favoritesCondition = " favorite = 1 ";

            if (routesPage.activeFilter)
                return "select id, name, desc from routes where id in ("+routesPage.filteredIds.join(",")+
                       ") and " + favoritesCondition + " order by number asc";
            return "select id, name, desc from routes where " + favoritesCondition + " order by number asc";
        }
    }

    Timer {
        id: searchTimer
        interval: 1000
        onTriggered: routesPage.search(routesPage.filter)
    }

    Component {
        id: errorPopupComponent

        MessagePopup {
            id: errorPopup
            title: i18n.tr("Error")
            message: ""
        }
    }

    Component {
        id: updateDialogComponent
        UpdateDialog {
            id: updateDialog

            onClosed: {
                if (updateDialog.error) {
                    var popup = PopupUtils.open(errorPopupComponent);
                    popup.message = updateDialog.error;
                }
            }
        }
    }

    state: "default"
    states: [
        PageHeadState {
            name: "default"
            head: routesPage.head
            actions: [
                Action {
                    iconName: "reload"
                    text: i18n.tr("Update")
                    onTriggered: {
                        PopupUtils.open(updateDialogComponent)
                    }
                },
                Action {
                    iconName: "search"
                    text: i18n.tr("Search")
                    onTriggered: routesPage.state = "search"
                }
            ]
        },
        PageHeadState {
            id: headerState
            name: "search"
            head: routesPage.head
            backAction: Action {
                id: leaveSearchAction
                text: "back"
                iconName: "back"
                onTriggered: {
                    routesPage.state = "default"
                    searchInput.text = "";
                }
            }
            contents: TextField {
                    id: searchInput
                    width: parent ? parent.width - units.gu(1) : undefined
                    placeholderText: i18n.tr("Search...")
                    onDisplayTextChanged: searchTimer.restart();
                    onVisibleChanged: visible ? forceActiveFocus() : null;
                }
            }
    ]

    UbuntuListView  {
        anchors.fill: parent
        model: routesModel
        clip: true

        delegate:

            ListItem.Subtitled {
                id: routeItem
                height: units.gu(6)
                text: name
                subText: desc
                progression: true
                onClicked: routesPage.routeClicked(id)
            }
    }
}
