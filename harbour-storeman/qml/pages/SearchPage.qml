import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property string initialSearch

    function _reset() {
        searchModel.searchKey = ""
        //% "Search results will be shown here"
        viewPlaceholder.text = qsTrId("orn-searchpage-placeholder-default")
        //% "Type some keywords in the field above"
        viewPlaceholder.hintText = qsTrId("orn-searchpage-placeholder-default-hint")
    }

    function _search(text) {
        searchModel.searchKey = text
        viewPlaceholder.text = ""
        viewPlaceholder.hintText = ""
    }

    id: page
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: if (!initialSearch) _reset()

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
        }

        header: Column {
            width: parent.width

            PageHeader {
                //: The search menu item and the search page header text - should be a noun
                //% "Search"
                title: qsTrId("orn-search")
            }

            SearchField {
                width: parent.width
                //: The search field placeholder text - should be a verb
                //% "Search"
                placeholderText: qsTrId("orn-searchfield-placeholder")

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: _search(text)

                onTextChanged: if (!text) _reset()
                Component.onCompleted: {
                    if (initialSearch) {
                        text = initialSearch
                        _search(initialSearch)
                    } else {
                        forceActiveFocus()
                    }
                }
            }
        }

        delegate: AppListDelegate { }

        VerticalScrollDecorator { }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
            verticalOffset: {
                var h = Qt.inputMethod.keyboardRectangle.height
                return h ? (parent.height - h) * 0.5 : 0
            }

            Behavior on verticalOffset {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }
        }

        BusyIndicator {
            id: busyIndicator
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: parent.count === 0 && !viewPlaceholder.enabled
        }
    }
}
