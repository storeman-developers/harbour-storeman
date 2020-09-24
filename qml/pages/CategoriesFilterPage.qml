import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Active) {
            categoriesList.model = categoriesModel
        }
    }

    OrnCategoriesModel {
        id: categoriesModel
    }

    Connections {
        target: networkManager
        onStateChanged: {
            if (categoriesList.count === 0 &&
                networkManager.connected) {
                categoriesModel.reset()
            }
        }
    }

    SilicaListView {
        id: categoriesList
        anchors.fill: parent

        header: PageHeader {
            //% "Categories filter"
            title: qsTrId("orn-categories-filter")
            //% "Select which categories to show"
            description: qsTrId("orn-categories-filter-descr")
        }

        delegate: CategoriesFilterDelegate {
            categoryVisible: model.visible
            height: Theme.itemSizeExtraSmall
            text: model.name
            depth: model.depth
            textAlignment: Text.AlignLeft

            onClicked: {
                const setVisible = !model.visible
                OrnClient.toggleCategoryVisibility(model.categoryId)
                for (var i in model.children) {
                    OrnClient.setCategoryVisibility(model.children[i], setVisible)
                }
            }
        }

        PullDownMenu {
            MenuStatusLabel {}
        }

        VerticalScrollDecorator {}

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !viewPlaceholder.text &&
                     categoriesList.count === 0
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
            text: networkManager.connected ? "" : qsTrId("orn-network-idle")
        }
    }
}
