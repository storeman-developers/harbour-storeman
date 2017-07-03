import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    property string icon
    property alias text: label.text

    spacing: Theme.paddingSmall

    Image {
        anchors.verticalCenter: parent.verticalCenter
        source: icon ? icon + "?" + Theme.highlightColor : ""
    }

    Label {
        id: label
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.highlightColor
        anchors.verticalCenter: parent.verticalCenter
    }
}
