import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    function _reset() {
        searchModel.searchKey = ""
        //% "Search results will be shown here"
        viewPlaceholder.text = qsTrId("orn-searchpage-placeholder-default")
        //% "Type some keywords in the field above"
        viewPlaceholder.hintText = qsTrId("orn-searchpage-placeholder-default-hint")
    }

    id: page

    Component.onCompleted: _reset()

    SilicaListView
    {
        anchors.fill: parent
        model: OrnSearchAppsModel {
            id: searchModel
            onResultsUpdated: {
                if (rowCount() === 0) {
                    //% "Nothing found"
                    viewPlaceholder.text = qsTrId("orn-searchpage-placeholder-noresults")
                    //% "Try to change search keywords"
                    viewPlaceholder.hintText = qsTrId("orn-searchpage-placeholder-noresults-hint")
                }
            }
            Component.onCompleted: apiRequest.networkManager = dataAccessManager
        }

        header: Column {
           width: parent.width

           PageHeader {
               title: qsTrId("orn-search")
           }

           SearchField {
               width: parent.width
               //: The search field placeholder text - should be a verb
               //% "Search"
               placeholderText: qsTrId("orn-searchfield-placeholder")

               EnterKey.enabled: text.length > 0
               EnterKey.iconSource: "image://theme/icon-m-enter-accept"
               EnterKey.onClicked: {
                   searchModel.searchKey = text
                   viewPlaceholder.text = ""
                   viewPlaceholder.hintText = ""
               }

               onTextChanged: if (!text) page._reset()
               Component.onCompleted: forceActiveFocus()
           }
        }

        delegate: AppListDelegate { }

        VerticalScrollDecorator { }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
        }

        BusyIndicator {
            id: busyIndicator
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: parent.count === 0 && !viewPlaceholder.enabled
        }
    }
}
