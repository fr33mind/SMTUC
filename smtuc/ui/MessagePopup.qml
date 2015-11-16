import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1

Popup {
    id: popup

    property int textFormat: Text.AutoText

    Flickable {
        id: textView
        width: parent.width
        height: textLabel.implicitHeight > popup.availableHeight ? popup.availableHeight : textLabel.implicitHeight
        flickableDirection: Flickable.VerticalFlick
        contentWidth: parent.width
        contentHeight: textLabel.height
        clip: true

        Label {
            id: textLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(1)
            text: popup.message
            wrapMode: Text.WordWrap
            height: implicitHeight
            textFormat: popup.textFormat
        }
    }
}
