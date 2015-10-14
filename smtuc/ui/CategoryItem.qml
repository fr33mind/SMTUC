import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.0 as ListItem

Column {
    id: category
    height: childrenRect.height
    width: parent.width
    spacing: units.gu(0.5)
    property string icon: ""
    property string title: ""
    property string content: ""
    property var contentItem: null

    Row {
        id: titleRow
        width: parent.width

        Item {
            width: childrenRect.width + units.gu(0.5)
            height: childrenRect.height

            Icon {
                width: categoryTitle.implicitHeight
                height: categoryTitle.implicitHeight
                name: category.icon
            }
        }

        Label {
            id: categoryTitle
            text: category.title
            height: implicitHeight
            wrapMode: Text.WordWrap
        }
    }

    ListItem.ThinDivider {}

    Item {
        id: containerItem
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(1)
        height: childrenRect.height
        children: category.contentItem ? [category.contentItem] : []
    }
}
