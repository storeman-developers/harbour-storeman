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
        view.headerItem.searchField.forceActiveFocus()
    }

    function _search(text) {
        searchModel.searchKey = text
        viewPlaceholder.text = ""
        viewPlaceholder.hintText = ""
        forceActiveFocus()
    }

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (initialSearch) {
                view.headerItem.searchField.text = initialSearch
                _search(initialSearch)
            } else if (!view.headerItem.searchField.text) {
                _reset()
            }
        }
    }

    SilicaListView
    {
        id: view
        anchors.fill: parent
        model: OrnSearchAppsModel {
            id: searchModel
            onFetchingChanged: {
                if (!fetching && rowCount() === 0) {
                    //% "Nothing found"
                    viewPlaceholder.text = qsTrId("orn-searchpage-placeholder-noresults")
                    //% "Try to change search keywords"
                    viewPlaceholder.hintText = qsTrId("orn-searchpage-placeholder-noresults-hint")
                }
            }
        }

        header: Column {
            property alias searchField: searchField

            width: parent.width

            PageHeader {
                //: The search menu item and the search page header text - should be a noun
                //% "Search"
                title: qsTrId("orn-search")
            }

            SearchField {
                id: searchField
                width: parent.width
                //: The search field placeholder text - should be a verb
                //% "Search"
                placeholderText: qsTrId("orn-searchfield-placeholder")

                EnterKey.enabled: text.length > 0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: _search(text)

                onTextChanged: if (!text) _reset()
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
