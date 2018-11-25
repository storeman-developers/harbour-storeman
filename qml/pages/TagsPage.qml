import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"


Page {
    property alias tagIds: tagsModel.tagIds
    property string appName
    property string appIconSource
    property int previousAppId: -1

    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: tagsList
        anchors.fill: parent        
        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnTagsModel.AppsCountRole
            sourceModel: OrnTagsModel {
                id: tagsModel
                onRowsInserted: proxyModel.sort(Qt.DescendingOrder)
            }
        }

        header: FancyPageHeader {
            id: header
            //% "Tags"
            title: qsTrId("orn-tags")
            description: appName
            iconSource: appIconSource
        }

        delegate: TagDelegate { }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: tagsModel
            }

            MenuStatusLabel { }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: tagsModel.fetching && !menu.active
        }

        ViewPlaceholder {
            enabled: text
            text: {
                hintText = ""
                if (!networkManager.online) {
                    return qsTrId("orn-network-idle")
                }
                if (tagsModel.networkError) {
                    hintText = qsTrId("orn-pull-refresh")
                    return qsTrId("orn-network-error")
                }
                return ""
            }
        }
    }
}
