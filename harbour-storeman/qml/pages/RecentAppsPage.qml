import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    id: page
    allowedOrientations: Orientation.All

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: networkManager.state === "idle" ?
                             flickableComponent : appListComponent
    }

    Component {
        id: appListComponent

        SilicaListView {
            id: appsList
            anchors.fill: parent

            header: PageHeader {
                //% "Recently updated"
                title: qsTrId("orn-recently-updated")
            }

            model: OrnProxyModel {
                id: proxyModel
                sortRole: OrnRecentAppsModel.DateRole
                sourceModel: OrnRecentAppsModel {
                    id: appsModel
                    onRowsInserted: proxyModel.sort(Qt.DescendingOrder)
                    Component.onCompleted: apiRequest.networkManager = dataAccessManager
                }
            }

            delegate: AppListDelegate { }

            section {
                property: "appData.sinceUpdate"
                delegate: SectionHeader {
                    text: section
                }
            }

            PullDownMenu {

                MenuItem {
                    //% "Repositories"
                    text: qsTrId("orn-repositories")
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/RepositoriesPage.qml"))
                }

                RefreshMenuItem {
                    model: appsModel
                }
            }

            VerticalScrollDecorator {}

            BusyIndicator {
                size: BusyIndicatorSize.Large
                anchors.centerIn: parent
                running: appsList.count === 0
            }
        }
    }

    Component {
        id: flickableComponent

        SilicaFlickable {
            anchors.fill: parent

            PullDownMenu {

                MenuItem {
                    text: qsTrId("orn-repositories")
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/RepositoriesPage.qml"))
                }

                RefreshMenuItem {
                    model: appsModel
                }
            }

            ViewPlaceholder {
                enabled: true
                //% "Network is unavailable"
                text: qsTrId("orn-network-idle")
            }
        }
    }
}
