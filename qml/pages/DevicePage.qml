import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            PageHeader {
                //% "This Device"
                title: qsTrId("orn-thisdevice")
                description: OrnPm.deviceModel
            }

            ListMenuItem {
                iconSource: OrnClient.userIconSource ?
                                OrnClient.userIconSource : "image://theme/icon-m-person"
                text: OrnClient.authorised ?
                          //% "Logged in as %0"
                          qsTrId("orn-loggedin-menu-item").arg(OrnClient.userName) :
                          //% "Log in to OpenRepos.net"
                          qsTrId("orn-login-menu-item")
                menu: ContextMenu {
                    MenuItem {
                        visible: OrnClient.isPublisher
                        //% "My applications"
                        text: qsTrId("orn-myapps")
                        onClicked: pageStack.push(Qt.resolvedUrl("UserAppsPage.qml"), {
                                                      userId: OrnClient.userId,
                                                      userName: OrnClient.userName,
                                                      userIcon: OrnClient.userIconSource
                                                  })
                    }

                    MenuItem {
                        //: Menu item
                        //% "Log out"
                        text: qsTrId("orn-logout-action")
                        onClicked: {
                            if (OrnClient.authorised) {
                                //: Remorse text
                                //% "Logging out"
                                Remorse.popupAction(page, qsTrId("orn-logout-remorse"), OrnClient.logout)
                            }
                        }
                    }
                }

                onClicked: {
                    if (OrnClient.authorised) {
                        openMenu()
                    } else if (networkManager.online) {
                        pageStack.push(Qt.resolvedUrl("AuthorisationDialog.qml"))
                    }
                }
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-developer-mode"
                text: qsTrId("orn-settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }

            ListMenuItem {
                enabled: OrnPm.initialised
                iconSource: "image://theme/icon-m-document"
                text: qsTrId("orn-repositories")
                onClicked: pageStack.push(Qt.resolvedUrl("RepositoriesPage.qml"))
            }

            ListMenuItem {
                enabled: OrnPm.initialised
                iconSource: "image://theme/icon-m-sailfish"
                text: qsTrId("orn-installed-apps")
                onClicked: pageStack.push(Qt.resolvedUrl("InstalledAppsPage.qml"))
            }

            ListMenuItem {
                enabled: networkManager.online
                iconSource: "image://theme/icon-m-favorite-selected"
                text: qsTrId("orn-bookmarks")
                onClicked: pageStack.push(Qt.resolvedUrl("BookmarksPage.qml"))
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-backup"
                text: qsTrId("orn-backups")
                onClicked: pageStack.push(Qt.resolvedUrl("BackupsPage.qml"))
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-file-rpm"
                text: qsTrId("orn-local-rpms")
                onClicked: pageStack.push(Qt.resolvedUrl("LocalRpmsPage.qml"))
            }
        }

        VerticalScrollDecorator { }
    }
}
