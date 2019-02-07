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
                networkManager.online) {
                categoriesModel.reset()
            }
        }
    }

    SilicaListView {
        id: categoriesList
        anchors.fill: parent

        header: PageHeader {
            //% "Categories"
            title: qsTrId("orn-categories")
        }

        delegate: MoreButton {
            height: Theme.itemSizeExtraSmall
            text: model.name
            depth: model.depth
            textAlignment: Text.AlignLeft

            onClicked: pageStack.push(Qt.resolvedUrl("CategoryPage.qml"), {
                                          categoryId: model.categoryId,
                                          categoryName: model.name
                                      })
        }

        PullDownMenu {
            MenuSearchItem {}
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
            text: networkManager.online ? "" : qsTrId("orn-network-idle")
        }
    }
}
