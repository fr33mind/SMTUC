import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1
import Smtuc 1.0

Page {
    id: outletsPage
    flickable: null

    SqlQueryModel {
        id: outletsModel
        database: Database{}
        query: "select * from outlets"
    }

    Component {
        id: alertComponent
        OutletPopup {
            id: outletPopup
            title: i18n.tr("Information")
        }
    }

    UbuntuListView  {
        anchors.fill: parent
        model: outletsModel
        clip: true

        delegate: ListItem.Standard {
                id: outletItem
                height: units.gu(6)
                text: name
                onClicked: {
                    var dialog = PopupUtils.open(alertComponent);
                    dialog.title = name;
                    dialog.address = address;
                    dialog.phone = phone;
                    dialog.normalHour = normal_hour;
                    dialog.weekendHour = weekend_hour;
                    dialog.coordinates = coordinates;
                    dialog.official = official;
                }
        }
    }
}
