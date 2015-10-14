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
    property alias availableHeight: contentRect.availableHeight
    default property alias contents: containerItem.data

    Rectangle {
        id: background
        anchors.fill: parent
        color: popup.backgroundColor
        opacity: popup.backgroundOpacity
    }

    Rectangle {
        id: contentRect
        anchors.margins: units.gu(2)
        anchors.centerIn: parent
        height: childrenRect.height
        width: parent.width - (anchors.leftMargin + anchors.rightMargin)
        color: Theme.palette.normal.background
        radius: 4

        property int availableHeight: calcAvailableHeight()

        function calcAvailableHeight()
        {
            if (! popup.height)
                return undefined;

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
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - units.gu(2)
                wrapMode: Text.WordWrap
                height: implicitHeight + units.gu(2)
                verticalAlignment: Text.AlignVCenter
                font.weight: Font.Bold
                font.pixelSize: FontUtils.sizeToPixels("large")
            }

            Item {
                id: containerItem
                height: childrenRect.height > contentRect.availableHeight ? contentRect.availableHeight : childrenRect.height
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - units.gu(2)
                clip: true
            }

            Item {
                width: parent.width
                height: closeBtn.height + units.gu(1)

                Button {
                    id: closeBtn
                    text: i18n.tr("OK")
                    width: parent.width / 2
                    anchors.centerIn: parent
                    onClicked: PopupUtils.close(popup)
                }
            }
        }
    }

}
