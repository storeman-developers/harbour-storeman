import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias userId: appsModel.userId
    property string userName
    property string userIcon
    property int previousAppId: -1

    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: appsList
        anchors.fill: parent

        header: FancyPageHeader {
            id: header
            //% "User applications"
            title: qsTrId("orn-user-apps")
            description: userName
            iconSource: userIcon
        }

        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnUserAppsModel.SortRole
            sourceModel: OrnUserAppsModel {
                id: appsModel
                onRowsInserted: proxyModel.sort(Qt.AscendingOrder)
            }
        }

        delegate: AppListDelegate {
            returnToUser: true
            showUser: false
            previousAppId: page.previousAppId
        }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: appsModel
            }

            MenuSearchItem {}

            MenuStatusLabel {}
        }

        VerticalScrollDecorator {}

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
