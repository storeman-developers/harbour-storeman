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
            sortRole: OrnUserAppsModel.TitleRole
            sourceModel: OrnUserAppsModel {
                id: appsModel
                onRowsInserted: proxyModel.sort(Qt.AscendingOrder)
                Component.onCompleted: apiRequest.networkManager = dataAccessManager
            }
        }

        delegate: AppListDelegate {
            returnToUser: true
            showUser: false
            previousAppId: page.previousAppId
        }

        PullDownMenu {
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
