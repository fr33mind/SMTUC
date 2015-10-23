import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1
import Smtuc 1.0
import "ui"

MainView {
    id: mainView
    width: units.gu(40)
    height: units.gu(50)
    Component.onCompleted: needsUpdate() ? PopupUtils.open(updateWarnPopover) : null

    function needsUpdate()
    {
        var updatedAt = settings.value("updatedAt", null),
            maxTimePassed = 1000 * 3600 * 24 * 30, //1 month in milliseconds
            timePassed = maxTimePassed,
            now = new Date();

        if (updatedAt)
            timePassed = now - new Date(updatedAt);

        if (timePassed >= maxTimePassed)
            return true;
        return false;
    }

    Database {
        id: database
        databaseName: "smtuc/smtuc.sqlite"
    }

    Settings {
        id: settings
        database: database
        autoSave: true
    }

    Component {
        id: updateWarnPopover
        MessagePopup {
            title:   i18n.tr("Update")
            message: i18n.tr("The local database is old and may be out of date. Please consider updating it.")
        }
    }

    PageStack {
        id: pageStack
        Component.onCompleted: {
            push(rootComponent);
        }

        Component {
            id: rootComponent
            Tabs {
                id: root
                Tab {
                    id: routesTab
                    title: i18n.tr("Routes")
                    page: RoutesPage {
                        onRouteClicked: pageStack.push(routePage, {routeId: id})
                    }
                }

                Tab {
                    id: ticketsTab
                    title: i18n.tr("Tickets")
                    page: TicketsPage {
                        id: ticketsPage
                    }
                }

                Tab {
                    id: outletsTab
                    title: i18n.tr("Outlets")
                    page: OutletsPage {
                        id: outletsPage
                    }
                }
            }
        }

        RoutePage {
            id: routePage
            parent: pageStack
            visible: false
        }
    }
}



