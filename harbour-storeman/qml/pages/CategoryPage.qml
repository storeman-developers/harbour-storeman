import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias categoryId: categoryModel.categoryId
    property string categoryName

    SilicaListView {
        id: categoryList
        anchors.fill: parent

        header: PageHeader {
            title: categoryName
        }

        model: OrnCategoryAppsModel {
            id: categoryModel
            Component.onCompleted: apiRequest.networkManager = dataAccessManager
            onReplyProcessed: {
                if (rowCount() === 0) {
                    //% "Currently there are no apps in this category"
                    viewPlaceholder.text = qsTrId("orn-category-noapps")
                }
            }
        }

        delegate: AppListDelegate { }

        PullDownMenu {
            RefreshMenuItem {
                model: categoryModel
            }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: categoryList.count === 0 && !viewPlaceholder.enabled
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
        }
    }
}
