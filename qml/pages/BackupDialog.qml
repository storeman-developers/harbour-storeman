import QtQuick 2.0
import QtQml 2.2
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    property OrnBackup backup

    id: dialog
    allowedOrientations: defaultAllowedOrientations

    onAccepted: backup.backup("%0/Storeman/Storeman-%1.ini".arg(StandardPaths.documents)
                              .arg(Qt.formatDateTime(new Date(), "yyyyMMddhhmmss")))

    DialogHeader {
        id: header
        //% "Backup to a file"
        title: qsTrId("orn-backup-title")
        //% "Backup"
        acceptText: qsTrId("orn-backup")
    }

    SilicaFlickable {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: label.height

        Label {
            id: label
            anchors {
                left: parent.left
                right: parent.right
                margins: Theme.horizontalPageMargin
            }
            color: Theme.highlightColor
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeSmall
            //% "Backup allows you to save your current OpenRepos repositories, installed applications and bookmarks and "
            //% "restore them later (for example after factory reset). A backup is a local file that is saved to the<br />"
            //% "<i>~/Documents/Storeman</i> directory.<br /><br />"
            //% "<b>Attention!</b> You should copy your backups manually to some safe place before performing a factory reset. "
            //% "It could be your SD card, external device, cloud storage or something else."
            text: qsTrId("orn-backup-hint")
        }

        VerticalScrollDecorator { }
    }
}
