import QtQuick 2.0
import Sailfish.Silica 1.0

SilicaListView {
    id: view
    snapMode: ListView.SnapToItem
    clip: true
    quickScrollEnabled: false

    delegate: BackgroundItem {
        width: view.width
        height: currentIndex === index
            ? Theme.itemSizeLarge
            : Theme.itemSizeSmall

        Behavior on height { NumberAnimation {} }

        onClicked: {
            currentIndex = index
            animation.moveTo(currentIndex)
        }

        Label {
            text: index
            anchors.centerIn: parent
            color: highlighted
                ? Theme.highlightColor
                : Theme.primaryColor
            font.pixelSize: currentIndex === index
                ? Theme.fontSizeHuge
                : Theme.fontSizeLarge

            Behavior on font.pixelSize { NumberAnimation {} }
        }
    }

    Component.onCompleted: positionViewAtIndex(currentIndex, ListView.Center)

    ListViewPositionAnimation {
        id: animation
        target: view
    }
}
