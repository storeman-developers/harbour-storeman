import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property alias iconSource: icon.source
    property alias text: label.text

    id: item
    width: parent.width
    contentHeight: Theme.itemSizeLarge

    Row {
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.paddingMedium

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.iconSizeMedium
            height: Theme.iconSizeMedium
            fillMode: Image.PreserveAspectFit
        }

        Label {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            color: item.pressed ? Theme.highlightColor : Theme.primaryColor
        }
    }
}
