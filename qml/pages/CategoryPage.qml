import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias categoryId: categoryModel.categoryId
    property string categoryName

    allowedOrientations: defaultAllowedOrientations

    OrnCategoryAppsModel {
        id: categoryModel
        onFetchingChanged: {
            if (!fetching && rowCount() === 0) {
                //% "Currently there are no apps in this category"
                viewPlaceholder.text = qsTrId("orn-category-noapps")
            }
        }
    }

    SilicaListView {
        id: categoryList
        anchors.fill: parent

        header: PageHeader {
            title: categoryName
        }

        model: networkManager.online ? categoryModel : null

        delegate: AppListDelegate { }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: categoryModel
            }

            MenuSearchItem {}

            MenuStatusLabel {}
        }

        VerticalScrollDecorator {}

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !viewPlaceholder.enabled &&
                     categoryList.count === 0 &&
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
                if (categoryModel.networkError) {
                    hintText = qsTrId("orn-pull-refresh")
                    return qsTrId("orn-network-error")
                }
                return ""
            }
        }
    }
}
