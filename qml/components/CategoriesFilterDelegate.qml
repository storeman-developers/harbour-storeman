import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    property bool categoryVisible
    property alias text: label.text
    property alias textAlignment: label.horizontalAlignment
    property int depth: 0
    readonly property color _color: categoryVisible ? highlighted ? Theme.highlightColor : Theme.primaryColor : Theme.secondaryColor

    height: Theme.itemSizeSmall
    opacity: categoryVisible ? 1.0 : Theme.opacityLow

    Label {
        id: label
        anchors {
            left: parent.left
            right: image.left
            verticalCenter: parent.verticalCenter
            leftMargin: Theme.horizontalPageMargin + depth * Theme.paddingLarge
            rightMargin: Theme.paddingMedium
        }
        horizontalAlignment: Text.AlignRight
        truncationMode: TruncationMode.Fade
        color: _color
    }

    Image {
        id: image
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: Theme.horizontalPageMargin
        }
        source: (categoryVisible ? "image://theme/icon-m-accept?" : "image://theme/icon-m-cancel?") + _color
    }
}
