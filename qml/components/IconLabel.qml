import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    property bool running: false
    property string icon
    property alias text: label.text

    spacing: Theme.paddingSmall

    Loader {
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: running ? busyComponent : iconComponent
    }

    Component {
        id: busyComponent
        BusyIndicator {
            size: BusyIndicatorSize.ExtraSmall
            running: true
        }
    }

    Component {
        id: iconComponent
        Image {
            source: icon ? icon + "?" + Theme.highlightColor : ""
        }
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.highlightColor
    }
}
