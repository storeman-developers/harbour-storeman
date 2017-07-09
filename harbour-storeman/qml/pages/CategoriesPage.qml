import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {

    Connections {
        target: networkManager
        onStateChanged: {
            if (categoriesList.count === 0 &&
                networkManager.state === "online") {
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

        model: OrnCategoriesModel {
            id: categoriesModel
            Component.onCompleted: apiRequest.networkManager = dataAccessManager
        }

        delegate: MoreButton {
            height: Theme.itemSizeExtraSmall
            text: categoryData.name
            depth: categoryData.depth
            textAlignment: Text.AlignLeft

            onClicked: pageStack.push(Qt.resolvedUrl("CategoryPage.qml"), {
                                          categoryId: categoryData.tid,
                                          categoryName: categoryData.name
                                      })
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: categoriesList.count === 0
        }
    }
}
