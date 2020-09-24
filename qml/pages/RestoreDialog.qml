import QtQuick 2.0
import QtQml 2.2
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    property OrnBackup backup
    property string filePath
    readonly property var _details: backup.details(filePath)

    id: dialog
    allowedOrientations: defaultAllowedOrientations
    canAccept: networkManager.connected

    onAccepted: backup.restore(filePath)

    DialogHeader {
        id: header
        //% "Restore from a file"
        title: qsTrId("orn-restore-title")
        //% "Restore"
        acceptText: qsTrId("orn-restore")
    }

    SilicaFlickable {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                //% "Restore OpenRepos repositories and installed apps from the selected file. This action will not affect your current repositories and will not remove installed applications."
                text: qsTrId("orn-restore-hint")
            }

            SectionHeader {
                //% "Details"
                text: qsTrId("orn-details")
            }

            DetailItem {
                //% "Created"
                label: qsTrId("orn-created")
                value: _details.created.toLocaleString(Qt.locale(), Locale.NarrowFormat)
            }

            DetailItem {
                //% "Total repositories"
                label: qsTrId("orn-total-repos")
                value: _details.repos
            }

            DetailItem {
                //% "Installed packages"
                label: qsTrId("orn-installed-packages")
                value: _details.packages
            }

            DetailItem {
                label: qsTrId("orn-bookmarks")
                value: _details.bookmarks
            }
        }

        VerticalScrollDecorator { }
    }
}
