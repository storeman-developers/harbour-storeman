import QtQuick 2.0
import Sailfish.Silica 1.0
import Qt.labs.folderlistmodel 2.1
import Nemo.Notifications 1.0
import harbour.orn 1.0
import "../components"

Page {
    property string _restoringFileName

    id: page
    allowedOrientations: defaultAllowedOrientations
    backNavigation: backup.status === OrnBackup.Idle

    OrnBackup {
        id: backup

        onBackupError: {
            var message = ""
            switch (err) {
            case OrnBackup.DirectoryError:
                //% "Could not create directory"
                message = qsTrId("orn-backup-error-directory")
                break
            default:
                break
            }
            //% "Backup error"
            notification.showPopup(qsTrId("orn-backup-error"), message,
                                   "image://theme/icon-lock-warning")
        }

        //% "Successful backup"
        onBackedUp: notification.show(qsTrId("orn-success-backup"),
                                      "image://theme/icon-s-installed")

        //% "Successful restore"
        onRestored: notification.show(qsTrId("orn-success-restore"),
                                      "image://theme/icon-s-installed")
    }

    Loader {
        anchors.fill: parent
        sourceComponent: backup.status === OrnBackup.Idle ||
                         backup.status === OrnBackup.BackingUp ?
                             backupsList : statusView
    }

    Component {
        id: backupsList

        SilicaListView {
            id: listView
            anchors.fill: parent
            model: FolderListModel {
                id: folderModel
                folder: StandardPaths.documents + "/Storeman"
                showDirs: false
                nameFilters: [ "*.ini" ]
                sortField: FolderListModel.Time
            }

            header: PageHeader {
                //% "Backups"
                title: qsTrId("orn-backups")
                //% "Backup and restore repos and installed apps"
                description: qsTrId("orn-backup-description")
            }

            delegate: ListItem {
                id: listItem
                width: parent.width

                menu: ContextMenu {
                    MenuItem {
                        text: qsTrId("orn-remove")
                        onClicked: Remorse.itemAction(listItem, qsTrId("orn-removing"),
                                                      function() { backup.removeFile(filePath) })
                    }
                }

                onClicked: {
                    var p = pageStack.push(Qt.resolvedUrl("RestoreDialog.qml"), {
                                               backup: backup,
                                               filePath: filePath
                                           })
                    p.accepted.connect(function() { _restoringFileName = fileBaseName })
                }

                Label {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }
                    color: parent.highlighted ? Theme.highlightColor : Theme.primaryColor
                    truncationMode: TruncationMode.Fade
                    text: fileBaseName

                }
            }

            PullDownMenu {
                id: pageMenu
                busy: backup.status !== OrnBackup.Idle

                MenuItem {
                    enabled: !pageMenu.busy
                    //% "Create a backup"
                    text: qsTrId("orn-create-backup")
                    //% "Creating a backup"
                    onClicked: pageStack.push(Qt.resolvedUrl("BackupDialog.qml"), {
                                                  backup: backup
                                              })
                }
            }

            ViewPlaceholder {
                enabled: listView.count === 0 && !pageMenu.active
                //% "Backups will be shown here"
                text: qsTrId("orn-backups-placeholder")
            }

            VerticalScrollDecorator { }
        }
    }

    Component {
        id: statusView

        SilicaFlickable {
            anchors.fill: parent

            PageHeader {
                //% "Restoring"
                title: qsTrId("orn-restoring-title")
                description: _restoringFileName
            }

            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: true
            }

            Label {
                anchors {
                    top: busyIndicator.bottom
                    left: parent.left
                    right: parent.right
                    topMargin: Theme.paddingLarge
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeLarge
                text: {
                    switch (backup.status) {
                    case OrnBackup.RestoringRepos:
                        //% "Restoring repositories"
                        return qsTrId("orn-restoring-repos")
                    case OrnBackup.RefreshingRepos:
                        //% "Refreshing repositories"
                        return qsTrId("orn-refreshing-repos")
                    case OrnBackup.SearchingPackages:
                        //% "Searching packages"
                        return qsTrId("orn-searching-packages")
                    case OrnBackup.InstallingPackages:
                        //% "Installing packages"
                        return qsTrId("orn-installing-repos")
                    default:
                        return ""
                    }
                }
            }
        }
    }
}
