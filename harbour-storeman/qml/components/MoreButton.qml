import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    property alias text: label.text

    height: Theme.itemSizeSmall

    Label {
        id: label
        anchors {
            left: parent.left
            right: image.left
            verticalCenter: parent.verticalCenter
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.paddingMedium
        }
        horizontalAlignment: Text.AlignRight
        truncationMode: TruncationMode.Fade
        color: parent.highlighted ? Theme.highlightColor : Theme.primaryColor
    }

    Image {
        id: image
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: Theme.horizontalPageMargin
        }
        source: "image://theme/icon-m-right?" +
                (parent.highlighted ? Theme.highlightColor : Theme.primaryColor)
    }
}
