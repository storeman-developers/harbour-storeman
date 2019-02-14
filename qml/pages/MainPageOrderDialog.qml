import QtQuick 2.0
import QtQml.Models 2.2
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"


Page {
    function _itemName(value) {
        switch (value) {
        case Storeman.RecentlyUpdated:
            return qsTrId("orn-recently-updated")
        case Storeman.Categories:
            return qsTrId("orn-categories")
        case Storeman.Bookmarks:
            return qsTrId("orn-bookmarks")
        case Storeman.Repositories:
            return qsTrId("orn-repositories")
        case Storeman.InstalledApps:
            return qsTrId("orn-installed-apps")
        case Storeman.LocalRpms:
            return qsTrId("orn-local-rpms")
        }
    }

    function _itemListElement(value) {
        value = parseInt(value)
        return {value: value, name: _itemName(value)}
    }

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            var items = visualModel.items
            var count = items.count
            var res = []
            for (var i = 0; i < count; ++i) {
                res.push(items.get(i).model.modelData.value)
            }
            Storeman.mainPageOrder = res
        }
    }

    Component {
        id: mainPageOrderDelegate

        MouseArea {
            property bool held: false
            readonly property color _highlightedColor: Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)

            id: dragArea
            anchors {
                left: parent.left
                right: parent.right
            }
            height: content.height
            drag {
                target: held ? content : undefined
                axis: Drag.YAxis
            }

            onPressAndHold: held = true
            onReleased: held = false

            Rectangle {
                id: content
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                width: dragArea.width
                height: Theme.itemSizeMedium
                color: held ? Theme.highlightBackgroundColor : pressed ? _highlightedColor : "transparent"

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                states: State {
                    when: dragArea.held

                    ParentChange {
                        target: content
                        parent: page
                    }

                    AnchorChanges {
                        target: content
                        anchors {
                            horizontalCenter: undefined
                            verticalCenter: undefined
                        }
                    }
                }

                Label {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }
                    text: modelData.name
                    color: pressed && !dragArea.held ? Theme.highlightColor : Theme.primaryColor
                }
            }

            DropArea {
                anchors {
                    fill: parent
                    margins: Theme.paddingSmall
                }
                onEntered: {
                    visualModel.items.move(
                        drag.source.DelegateModel.itemsIndex,
                        dragArea.DelegateModel.itemsIndex
                    )
                }
            }
        }
    }

    DelegateModel {
        id: visualModel
        model: Storeman.mainPageOrder.map(_itemListElement)
        delegate: mainPageOrderDelegate
    }


    SilicaListView {
        anchors.fill: parent
        model: visualModel
        header: PageHeader {
            //% "Order items"
            title: qsTrId("orn-mainpage-order")
            //% "Drag items to change their order"
            description: qsTrId("orn-mainpage-order-description")
        }

        PullDownMenu {
            MenuItem {
                //% "Reset"
                text: qsTrId("orn-reset")
                onClicked: Storeman.mainPageOrder = undefined
            }
        }

        VerticalScrollDecorator {}
    }
}
