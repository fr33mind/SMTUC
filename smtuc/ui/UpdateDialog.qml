import QtQuick 2.0
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import Smtuc 1.0

Dialog {
    id: dialog
    title: i18n.tr("Update")
    text: i18n.tr("Update local database?\n (Requires internet connection)")

    property bool started: false
    property string error: ""
    signal closed()

    Connections {
        target: databaseUpdater
        onError: {
            dialog.error = error;
        }
    }

    Connections {
        target: databaseUpdater
        onFinished: {
            dialog.closed();
            PopupUtils.close(dialog);
        }
    }

    Connections {
        target: databaseUpdater
        onStatusMessageChanged: {
            dialog.title = message;
        }
    }

    Database {
        id: database
    }

    DatabaseUpdater {
        id: databaseUpdater
        database: database
    }

    onStartedChanged: {
        text = "";
        //hide buttons
        cancelButton.parent = null;
        updateButton.parent = null;
    }

    ProgressBar {
        value: databaseUpdater.progress
        visible: dialog.started ? true : false
    }

    Button {
        id: cancelButton
        text: i18n.tr("Cancel")
        onClicked: PopupUtils.close(dialog)
        visible: dialog.started ? false : true
    }

    Button {
        id: updateButton
        text: i18n.tr("Update")
        color: UbuntuColors.orange
        visible: dialog.started ? false: true
        onClicked: {
            databaseUpdater.update();
            dialog.started = true;
        }
    }
}
