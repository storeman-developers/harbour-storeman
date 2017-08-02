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
            sortRole: OrnInstalledAppsModel.TitleRole
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: OrnInstalledAppsModel {
                id: installedAppsModel
                zypp: ornZypp
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
        }

        section {
            property: "section"
            delegate: SectionHeader {
                text: section
            }
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeExtraLarge
            onClicked: pageStack.push(Qt.resolvedUrl("SearchPage.qml"),
                                      { initialSearch: appName })

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
                    source: appIconSource ? appIconSource : "qrc:/images/appicon.png"
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
                        text: appTitle
                    }

                    Label {
                        id: versionLabel
                        width: parent.width
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.secondaryColor
                        text: appVersion
                    }

                    Label {
                        id: userNameLabel
                        width: column.width
                        horizontalAlignment: Qt.AlignRight
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.highlightColor
                        text: appAuthor
                    }
                }
            }
        }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: installedAppsModel
            }
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
