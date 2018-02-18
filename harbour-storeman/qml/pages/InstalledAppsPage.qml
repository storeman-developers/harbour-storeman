import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool _working: false

    id: page
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
            //% "Installed Applications"
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
            onClicked: pageStack.push(Qt.resolvedUrl("SearchPage.qml"),
                                      { initialSearch: packageName })

            menu: ContextMenu {

                MenuItem {
                    visible: updateAvailable
                    text: qsTrId("orn-update")
                    onClicked: OrnPm.updatePackage(packageName)
                }

                MenuItem {
                    text: qsTrId("orn-remove")
                    onClicked: Remorse.itemAction(item, qsTrId("orn-removing"), function() {
                        // FIXME
                        OrnPm.removePackage(packageId)
                    })
                }
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
                    source: packageIcon ? packageIcon : "qrc:/images/appicon.png"
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
                        text: packageVersion
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
                //% "Update all"
                text: qsTrId("orn-update-all")
                onClicked: OrnPm.updateAll()
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
