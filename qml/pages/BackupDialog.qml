import QtQuick 2.0
import QtQml 2.2
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"


Dialog {
    property OrnBackup backup
    // This is need for restoring options after orientation changed
    property QtObject backupOptions: QtObject {
        property string filename: "Storeman-%1.ini".arg(Qt.formatDateTime(new Date(), "yyyyMMddhhmmss"))
        property bool repos: true
        property bool installed: true
        property bool bookmarks: true
    }

    id: dialog
    allowedOrientations: defaultAllowedOrientations
    canAccept: loader.item.acceptable && (
                   backupOptions.bookmarks    ||
                   backupOptions.repos ||
                   backupOptions.installed
                )

    onAccepted: {
        var items = 0
        if (backupOptions.repos) {
            items = items | OrnBackup.BackupRepos
        }
        if (backupOptions.installed) {
            items = items | OrnBackup.BackupInstalled
        }
        if (backupOptions.bookmarks) {
            items = items | OrnBackup.BackupBookmarks
        }
        backup.backup(loader.item.path, items)
    }

    Component {
        id: portraitLayout

        SilicaFlickable {
            property alias path: options.path
            property alias acceptable: options.acceptable

            contentHeight: options.height + label.height
            clip: true

            BackupOptions {
                id: options
            }

            BackupLabel {
                id: label
                anchors {
                    top: options.bottom
                    topMargin: Theme.paddingLarge
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
            }

            VerticalScrollDecorator { }
        }
    }

    Component {
        id: landscapeLayout

        Item {
            property alias path: options.path
            property alias acceptable: options.acceptable

            clip: true

            SilicaFlickable {
                anchors {
                    top: parent.top
                    right: parent.horizontalCenter
                    bottom: parent.bottom
                    left: parent.left
                }
                contentHeight: label.height

                BackupLabel {
                    id: label
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Theme.horizontalPageMargin
                    }
                }
            }

            SilicaFlickable {
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    left: parent.horizontalCenter
                }
                contentHeight: options.height

                BackupOptions {
                    id: options
                }
            }
        }
    }

    DialogHeader {
        id: header
        //% "Backup"
        acceptText: qsTrId("orn-backup")
    }

    Loader {
        id: loader
        anchors {
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        sourceComponent: dialog.isPortrait ? portraitLayout : landscapeLayout
    }
}
