import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Smtuc 1.0

Popup {
    id: outlet
    property int outletId: 0
    property string address: ""
    property string phone: ""
    property string normalHour: ""
    property string weekendHour: ""
    property string coordinates: ""
    property int official: 0
    readonly property string googleMapsUrl: "http://maps.google.com/maps?z=12&t=m&q=loc:" + coordinates

    SqlQueryModel {
        id: varsModel
        property string name: outlet.official ? "smtuc_android_loja_descricao" :
                                                "smtuc_android_agente_descricao"
        database: Database{}
        query: "select * from vars where name = '" + name + "'"
        onQueryChanged: descriptionLabel.text = this.recordField(0, "value")
    }

    Flickable {
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        width: parent.width
        height: contentHeight > outlet.availableHeight ? outlet.availableHeight : contentHeight
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Column {
            width: parent.width
            spacing: units.gu(2)

            Label {
                id: descriptionLabel
                width: parent.width
                wrapMode: Text.WordWrap
                text: ""
            }

            CategoryItem {
                title: i18n.tr("Address")
                contentItem: Label {
                    width: parent.width
                    text: '<a href="'+googleMapsUrl+'">' + address + "</a>"
                    onLinkActivated: Qt.openUrlExternally(googleMapsUrl)
                    wrapMode: Text.WordWrap

                }
                icon: "home"
            }


            CategoryItem {
                title: i18n.tr("Telephone")
                contentItem: Label {
                    text: '<a href="tel:'+outlet.phone+'">' + outlet.phone + "</a>"
                    onLinkActivated: Qt.openUrlExternally("tel:"+outlet.phone)
                }
                icon: "call-start"
            }

            CategoryItem {
                title: i18n.tr("Schedule")
                icon: "reminder"
                contentItem: Item {
                    height: childrenRect.height
                    width: parent.width

                    Column {
                        width: parent.width
                        Label {
                            text: qsTr("<i>%1:</i> ").arg(i18n.tr("Weekdays")) + outlet.normalHour
                            width: parent.width
                            height: implicitHeight
                            wrapMode: Text.WordWrap
                            visible: outlet.normalHour ? true : false
                        }

                        Item {
                            width: parent.width
                            height: units.gu(1)
                        }

                        Label {
                            text: qsTr("<i>%1:</i> ").arg(i18n.tr("Weekends")) + outlet.weekendHour
                            width: parent.width
                            height: implicitHeight
                            wrapMode: Text.WordWrap
                            visible: outlet.weekendHour ? true : false
                        }
                    }
                }
            }
        }
    }
}
