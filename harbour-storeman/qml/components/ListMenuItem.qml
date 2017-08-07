import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property alias text: label.text
    property string iconSource
    readonly property string _color:
        pressed ? Theme.highlightColor :
                  enabled ? Theme.primaryColor : Theme.secondaryHighlightColor

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
            source: iconSource ? (iconSource + "?" + _color) : ""
        }

        Label {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            color: _color
        }
    }
}
