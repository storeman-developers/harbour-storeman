import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property OrnRecentAppsModel model

    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: appsList
        anchors.fill: parent
        model: networkManager.online ? page.model : null

        header: PageHeader {
            //% "Recently updated"
            title: qsTrId("orn-recently-updated")
        }

        delegate: AppListDelegate { }

        section {
            property: "sinceUpdate"
            delegate: SectionHeader {
                text: section
            }
        }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: page.model
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
                    //% "Network is unavailable"
                    return qsTrId("orn-network-idle")
                }
                if (page.model.networkError) {
                    //% "Pull down to refresh"
                    hintText = qsTrId("orn-pull-refresh")
                    //% "A network error occurred"
                    return qsTrId("orn-network-error")
                }
                return ""
            }
        }
    }
}
