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
                description: ornZypp.deviceModel()
            }

            ListMenuItem {
                iconSource: ornClient && ornClient.userIconSource ? ornClient.userIconSource :
                                "image://theme/icon-m-person?" +
                                (pressed ? Theme.highlightColor : Theme.primaryColor)
                text: ornClient && ornClient.authorised ?
                          //% "Logged in as %0"
                          qsTrId("orn-loggedin-menu-item").arg(ornClient.userName) :
                          //% "Log in to OpenRepos.net"
                          qsTrId("orn-login-menu-item")
                menu: ContextMenu {
                    MenuItem {
                        //: Menu item
                        //% "Log out"
                        text: qsTrId("orn-logout-action")
                        onClicked: {
                            if (ornClient.authorised) {
                                //: Remorse text
                                //% "Logging out"
                                Remorse.popupAction(page, qsTrId("orn-logout-remorse"), ornClient.logout)
                            }
                        }
                    }
                }

                onClicked: ornClient.authorised ?
                               showMenu() :
                               pageStack.push(Qt.resolvedUrl("AuthorisationDialog.qml"))

            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-document?" +
                            (pressed ? Theme.highlightColor : Theme.primaryColor)
                text: qsTrId("orn-repositories")
                onClicked: pageStack.push(Qt.resolvedUrl("RepositoriesPage.qml"))
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-sailfish?" +
                            (pressed ? Theme.highlightColor : Theme.primaryColor)
                text: qsTrId("orn-installed-apps")
                onClicked: pageStack.push(Qt.resolvedUrl("InstalledAppsPage.qml"))
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-favorite-selected?" +
                            (pressed ? Theme.highlightColor : Theme.primaryColor)
                text: qsTrId("orn-bookmarks")
                onClicked: pageStack.push(Qt.resolvedUrl("BookmarksPage.qml"))
            }
        }

        VerticalScrollDecorator { }
    }
}
