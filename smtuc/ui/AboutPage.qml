import QtQuick 2.2
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1

Page {
    id: aboutPage
    property string name: "SMTUC"
    property string version: "1.1"
    property string description: i18n.tr("Unofficial SMTUC application for Ubuntu Touch.")
    property string author: "Carlos Pais"
    property string repositoryUrl: "https://github.com/fr33mind/SMTUC"

    Item {
        anchors.fill: parent
        anchors.margins: units.gu(2)

        Flickable {
            clip: true
            anchors.fill: parent
            contentWidth: parent.width
            contentHeight: contentItem.childrenRect.height
            width: parent.width
            height: parent.height

            Column {
                width: parent.width
                spacing: units.gu(1)
                height: childrenRect.height

                Image {
                    id: logo
                    source: "../images/smtuc.svg"
                    width: units.gu(20)
                    height: units.gu(20)
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {
                    width: parent.width
                    height: units.gu(1)
                }

                Row {
                    spacing: units.gu(1)
                    anchors.horizontalCenter: parent.horizontalCenter

                    Label {
                        id: nameLabel
                        text: aboutPage.name
                        font.pixelSize: FontUtils.sizeToPixels("x-large")
                        font.bold: true
                    }

                    Label {
                        id: versionLabel
                        text: aboutPage.version
                        font.pixelSize: FontUtils.sizeToPixels("large")
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Label {
                    id: descLabel
                    text: aboutPage.description
                    font.pixelSize: FontUtils.sizeToPixels("large")
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Item {
                    width: parent.width
                    height: units.gu(1)
                }

                Label {
                    id: copyrightLabel
                    text: "Copyright (c) 2015-2016 " + aboutPage.author
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: FontUtils.sizeToPixels("small")
                }

                Label {
                    id: licenseLabel
                    text: i18n.tr("Released under the GNU GPLv3.")
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: FontUtils.sizeToPixels("small")
                    wrapMode: Text.WordWrap
                }


                Label {
                    id: repositoryLabel
                    text: i18n.tr("Source code available at %1.").arg('<a href="'+aboutPage.repositoryUrl+'">GitHub</a>')
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: FontUtils.sizeToPixels("small")
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
