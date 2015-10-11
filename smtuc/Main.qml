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

    function needsUpdate()
    {
        //FIXME: Fix database settings
        var updatedAt = settingsModel.recordField(0, "updatedAt"),
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

    DatabaseUpdater {
        id: databaseUpdater
        database: database
    }

   SqlQueryModel {
       id: settingsModel
       query: "SELECT * FROM settings"
   }

    Component {
        id: updateProgress

        Dialog {
            id: updatePopover
            title: i18n.tr("Updating...")
            property bool finished: databaseUpdater.finished

            Component.onCompleted: {
                databaseUpdater.update()
            }

            ProgressBar {
                value: databaseUpdater.progress
            }

            onFinishedChanged: {
                if (finished)
                    PopupUtils.close(updatePopover)
            }
        }
    }

    Component {
        id: updateDialog

        Dialog {
            id: dialog
            title: i18n.tr("Update")
            text: i18n.tr("The database may be out of date. Do you want to update it?")
            Button {
                text: "cancel"
                onClicked: PopupUtils.close(dialog)
            }
            Button {
                text: "update"
                color: UbuntuColors.orange
                onClicked: {
                    PopupUtils.close(dialog)
                    PopupUtils.open(updateProgress)
                }
            }
        }
    }

    PageStack {
        id: pageStack
        Component.onCompleted: {
            push(rootComponent);
            if (needsUpdate())
                PopupUtils.open(updateDialog)
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
            }
        }

        RoutePage {
            id: routePage
            parent: pageStack
            visible: false
        }
    }
}



