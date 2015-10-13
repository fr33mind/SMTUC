import QtQuick 2.2
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1
import Smtuc 1.0

Page {
    id: ticketsPage
    flickable: null

    SqlQueryModel {
        id: ticketsModel
        database: Database{}
        query: "select * from ticket_prices"
    }

    Component {
        id: popupComponent
        MessagePopup {
            id: messagePopup
            title: i18n.tr("Information")
        }
    }

    UbuntuListView  {
        anchors.fill: parent
        model: ticketsModel
        clip: true

        delegate: ListItem.Standard {
                id: ticketItem
                height: units.gu(6)
                text: name
                control: Label {
                    width: implicitWidth
                    text: {
                        var price = parseFloat(total_price);
                        if (price === price)
                            return "€" + total_price;
                        return "*";
                    }
                }

                onClicked: {
                    var text = desc;
                    var price = parseFloat(total_price);
                    if (price !== price) //if is NaN
                        text += "\n\n" + total_price;

                    var dialog = PopupUtils.open(popupComponent);
                    dialog.title = name;
                    dialog.message = text;
                }
        }
    }
}
