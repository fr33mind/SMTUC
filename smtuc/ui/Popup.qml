import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import QtQuick.Layouts 1.1

PopupBase {
    id: popup

    property string title: ""
    property string message: ""
    property real backgroundOpacity: 0.4
    property string backgroundColor: "black"
    property int availableHeight: contentRect.availableHeight
    default property alias contents: containerItem.data

    Rectangle {
        id: background
        anchors.fill: parent
        color: popup.backgroundColor
        opacity: popup.backgroundOpacity
    }

    Rectangle {
        id: contentRect
        //anchors.fill: parent
        anchors.margins: units.gu(2)
        anchors.centerIn: parent
        height: childrenRect.height
        width: parent.width - (anchors.leftMargin + anchors.rightMargin)
        color: Theme.palette.normal.background
        radius: 4

        property int availableHeight: calcAvailableHeight()

        function calcAvailableHeight()
        {
            var usedSpace = anchors.topMargin + anchors.bottomMargin + titleLabel.height +  closeBtn.height + column.emptySpace;
            return popup.height - usedSpace;
        }

        Column {
            id: column
            width: parent.width
            spacing: units.gu(1)

            property int emptySpace: children.length ? spacing * (children.length - 1) : 0

            Label {
                id: titleLabel
                text: popup.title
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: units.gu(1)
                anchors.rightMargin: units.gu(1)
                wrapMode: Text.WordWrap
                height: implicitHeight + units.gu(1)
                verticalAlignment: Text.AlignVCenter
                font.weight: Font.Bold
            }

            Item {
                id: containerItem
                width: parent.width
                height: childrenRect.height > popup.availableHeight ? popup.availableHeight : childrenRect.height
                clip: true
            }

            Button {
                id: closeBtn
                text: i18n.tr("OK")
                width: parent.width / 2
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: PopupUtils.close(popup)
            }
        }
    }

}
