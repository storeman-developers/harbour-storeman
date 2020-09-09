import QtQuick 2.5
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    readonly property real cellWidth: width / (isPortrait ? 2.0 : 4.0)
    readonly property int gridColumns: isPortrait ? 2 : 4
    readonly property int gridRows: isPortrait ? 3 : 2
    property real gridHeight: 0.0
    property bool _initialLoad: true

    function _getComponent(value) {
        switch (value) {
        case Storeman.RecentlyUpdated:
            return recentlyUpdatedComponent
        case Storeman.Categories:
            return categoriesComponent
        case Storeman.Bookmarks:
            return bookmarksComponent
        case Storeman.Repositories:
            return repositoriesComponent
        case Storeman.MyRepository:
            return myRepositoryComponent
        case Storeman.InstalledApps:
            return installedAppsComponent
        case Storeman.LocalRpms:
            return localRpmsComponent
        }
    }

    function _createComponent(value) {
        _getComponent(parseInt(value)).createObject(content)
    }

    function _render() {
        content.children = null
        menuComponent.createObject(content)
        Storeman.mainPageOrder.forEach(_createComponent)
    }

    id: page
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: _render()

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

    Connections {
        target: Storeman
        onMainPageOrderChanged: _render()
        onRecentAppsChanged: recentAppsModel.reset()
    }

    Component {
        id: menuComponent

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
    }

    Component {
        id: recentlyUpdatedComponent

        Item {
            width: content.width
            height: recentlyUpdatedButton.height + recentlyUpdatedGrid.height

            MainPageButton {
                id: recentlyUpdatedButton
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
                    running: !repeater.count && recentAppsModel.sourceModel.fetching
                    size: BusyIndicatorSize.Small
                }
            }

            Grid {
                id: recentlyUpdatedGrid
                anchors.top: recentlyUpdatedButton.bottom
                width: parent.width
                height: networkManager.online ? gridHeight : 0.0
                columns: gridColumns
                rows: gridRows
                opacity: height === gridHeight ? 1.0 : 0.0

                Behavior on height { NumberAnimation {} }

                Repeater {
                    id: repeater
                    model: OrnProxyModel {
                        limit: gridColumns * gridRows
                        sourceModel: recentAppsModel
                    }
                    delegate: MainPageAppGridDelegate {}

                    onCountChanged: gridHeight = count > 0 ? Theme.itemSizeSmall * gridRows : 0.0
                }
            }
        }
    }

    Component {
        id: categoriesComponent
        MainPageButton {
            enabled: networkManager.online
            text: qsTrId("orn-categories")
            onClicked: pageStack.push(Qt.resolvedUrl("CategoriesPage.qml"))
        }
    }

    Component {
        id: bookmarksComponent
        MainPageButton {
            enabled: networkManager.online
            text: qsTrId("orn-bookmarks")
            onClicked: pageStack.push(Qt.resolvedUrl("BookmarksPage.qml"), {
                                          model: bookmarksModel
                                      })
        }
    }

    Component {
        id: repositoriesComponent
        MainPageButton {
            enabled: OrnPm.initialised
            text: qsTrId("orn-repositories")
            onClicked: pageStack.push(Qt.resolvedUrl("RepositoriesPage.qml"))
        }
    }

    Component {
        id: myRepositoryComponent
        MainPageButton {
            visible: OrnClient.isPublisher
            enabled: OrnPm.initialised
            //% "My repository"
            text: qsTrId("orn-myrepository")
            onClicked: pageStack.push(Qt.resolvedUrl("RepositoryPage.qml"), {
                                          userId: OrnClient.userId,
                                          userName: OrnClient.userName,
                                          userIcon: OrnClient.userIconSource
                                      })
        }
    }

    Component {
        id: installedAppsComponent
        MainPageButton {
            enabled: OrnPm.initialised
            text: qsTrId("orn-installed-apps")
            onClicked: pageStack.push(Qt.resolvedUrl("InstalledAppsPage.qml"))
        }
    }

    Component {
        id: localRpmsComponent
        MainPageButton {
            text: qsTrId("orn-local-rpms")
            onClicked: pageStack.push(Qt.resolvedUrl("LocalRpmsPage.qml"))
        }
    }

    OrnProxyModel {
        id: recentAppsModel
        sourceModel: OrnAppsModel {
            resource: "apps"
        }
        filterRole: OrnAppsModel.VisibilityRole
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
        }
    }
}
