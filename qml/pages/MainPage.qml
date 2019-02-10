import QtQuick 2.5
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    readonly property real cellWidth: width / (isPortrait ? 2 : 4)
    readonly property int gridColumns: isPortrait ? 2 : 4
    readonly property int gridRows: isPortrait ? 3 : 2
    readonly property real gridHeight: repeater.count > 0 ? Theme.itemSizeSmall * gridRows : 0
    property bool _initialLoad: true

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Active) {
            Storeman.checkRepos()
            if (_initialLoad && Storeman.showRecentOnStart) {
                _initialLoad = false
                pageStack.push(Qt.resolvedUrl("RecentAppsPage.qml"), {
                                   model: recentAppsModel
                               }, PageStackAction.Immediate)
            }
        }
    }

    OrnRecentAppsModel {
        id: recentAppsModel
    }

    OrnBookmarksModel {
        id: bookmarksModel
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            PullDownMenu {
                id: menu

                MenuItem {
                    text: qsTrId("orn-about")
                    onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                }

                MenuItem {
                    text: qsTrId("orn-settings")
                    onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
                }

                MenuSearchItem {}

                MenuStatusLabel {}
            }

            MainPageButton {
                enabled: networkManager.online
                text: qsTrId("orn-recently-updated")
                onClicked: pageStack.push(Qt.resolvedUrl("RecentAppsPage.qml"), {
                                              model: recentAppsModel
                                          })

                BusyIndicator {
                    anchors {
                        left: parent.left
                        leftMargin: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }
                    running: !repeater.count && recentAppsModel.fetching
                    size: BusyIndicatorSize.Small
                }
            }

            Grid {
                id: recentGrid
                width: parent.width
                height: networkManager.online ? gridHeight : 0.0
                columns: gridColumns
                rows: gridRows
                opacity: height === gridHeight ? 1.0 : 0.0

                Behavior on height { NumberAnimation {} }
                OpacityAnimator on opacity {}

                Repeater {
                    id: repeater
                    model: OrnProxyModel {
                        limit: gridColumns * gridRows
                        sourceModel: recentAppsModel
                    }

                    delegate: MainPageAppGridDelegate {}
                }
            }

            MainPageButton {
                enabled: networkManager.online
                text: qsTrId("orn-categories")
                onClicked: pageStack.push(Qt.resolvedUrl("CategoriesPage.qml"))
            }

            MainPageButton {
                enabled: networkManager.online
                text: qsTrId("orn-bookmarks")
                onClicked: pageStack.push(Qt.resolvedUrl("BookmarksPage.qml"), {
                                              model: bookmarksModel
                                          })
            }

            MainPageButton {
                enabled: OrnPm.initialised
                text: qsTrId("orn-repositories")
                onClicked: pageStack.push(Qt.resolvedUrl("RepositoriesPage.qml"))
            }

            MainPageButton {
                enabled: OrnPm.initialised
                text: qsTrId("orn-installed-apps")
                onClicked: pageStack.push(Qt.resolvedUrl("InstalledAppsPage.qml"))
            }

            MainPageButton {
                text: qsTrId("orn-local-rpms")
                onClicked: pageStack.push(Qt.resolvedUrl("LocalRpmsPage.qml"))
            }
        }
    }
}
