import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool _working: false

    id: page
    objectName: "InstalledAppsPage"
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: appsList
        anchors.fill: parent
        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnInstalledAppsModel.SortRole
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: OrnInstalledAppsModel {
                id: installedAppsModel
                onModelAboutToBeReset: _working = true
                onModelReset: {
                    _working = false
                    proxyModel.sort(Qt.AscendingOrder)
                }
            }
        }

        header: PageHeader {
            //% "Installed applications"
            title: qsTrId("orn-installed-apps")
            //% "Only from enabled repositories"
            description: qsTrId("orn-installed-apps-description")
        }

        section {
            property: OrnPm.updatesAvailable ? "updateAvailable" : "section"
            delegate: SectionHeader {
                // If updates are available then show sections by status
                // otherwise show sections by the first letter of titles
                text: OrnPm.updatesAvailable ?
                          (section === '1' ? qsTrId("orn-update-available") :
                                             qsTrId("orn-installed")) :
                          section
            }
        }

        delegate: ListItem {
            id: item
            contentHeight: Theme.itemSizeExtraLarge
            onClicked: openMenu()

            menu: ContextMenu {

                MenuItem {
                    //% "Search on OpenRepos.net"
                    text: qsTrId("orn-search-on-openrepos")
                    onClicked: pageStack.push(Qt.resolvedUrl("SearchPage.qml"),
                                              { initialSearch: packageTitle })
                }

                MenuItem {
                    visible: updateAvailable
                    enabled: networkManager.online && !itemInProgress(packageName)
                    text: qsTrId("orn-update")
                    onClicked: OrnPm.updatePackage(packageName)
                }

                MenuItem {
                    text: qsTrId("orn-remove")
                    enabled: !itemInProgress(packageName)
                    onClicked: Remorse.itemAction(item, qsTrId("orn-removing"), function() {
                        OrnPm.removePackage(packageId)
                    })
                }
            }

            ListView.onAdd: AddAnimation {
                target: item
            }

            ListView.onRemove: RemoveAnimation {
                target: item
            }

            Row {
                id: row
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                spacing: Theme.paddingMedium

                Image {
                    id: appIcon
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.iconSizeLauncher
                    height: Theme.iconSizeLauncher
                    fillMode: Image.PreserveAspectFit
                    source: packageIcon ? packageIcon : "image://theme/icon-launcher-default"
                }

                Column {
                    id: column
                    anchors.verticalCenter: parent.verticalCenter
                    width: row.width - appIcon.width - Theme.paddingMedium
                    spacing: Theme.paddingSmall

                    Label {
                        id: titleLabel
                        width: parent.width
                        maximumLineCount: 2
                        verticalAlignment: Qt.AlignVCenter
                        font.pixelSize: Theme.fontSizeExtraSmall
                        wrapMode: Text.WordWrap
                        text: packageTitle
                    }

                    Label {
                        id: versionLabel
                        width: parent.width
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.secondaryColor
                        text: updateAvailable ? packageVersion + " ➝ " + updateVersion :
                                                packageVersion

                    }
                }
            }
        }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: installedAppsModel
            }

            MenuItem {
                visible: OrnPm.updatesAvailable
                enabled: networkManager.online && _operations && _operations.length === 0
                //% "Update all"
                text: qsTrId("orn-update-all")
                onClicked: {
                    var updates = OrnPm.updatablePackages()
                    for (var i = 0; i < updates.length; ++i) {
                        OrnPm.updatePackage(updates[i])
                    }
                }
            }

            MenuStatusLabel { }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: _working && !menu.active
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: appsList.count === 0 && !_working
            //% "Could not find any applications installed from OpenRepos"
            text: qsTrId("orn-no-installed-apps")
        }
    }
}
