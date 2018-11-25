import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias tagId: appsModel.tagId
    property string tagName
    property int previousAppId: -1

    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: appsList
        anchors.fill: parent

        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnTagAppsModel.SortRole
            sourceModel: OrnTagAppsModel {
                id: appsModel
                onRowsInserted: proxyModel.sort(Qt.AscendingOrder)
            }
        }

        header: PageHeader {
            id: header
            //% "Tagged Applications"
            title: qsTrId("orn-tag-apps")
            description: tagName
        }

        delegate: AppListDelegate {
            previousAppId: page.previousAppId
            previousStep: 2
        }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: appsModel
            }

            MenuStatusLabel { }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !viewPlaceholder.text &&
                     appsList.count === 0 &&
                     !menu.active
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
            text: {
                hintText = ""
                if (!networkManager.online) {
                    return qsTrId("orn-network-idle")
                }
                if (appsModel.networkError) {
                    hintText = qsTrId("orn-pull-refresh")
                    return qsTrId("orn-network-error")
                }
                return ""
            }
        }
    }
}
