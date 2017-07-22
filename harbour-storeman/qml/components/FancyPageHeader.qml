import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property alias title: header.title
    property alias description: header.description
    property alias iconSource: image.source

    width: parent.width
    height: header.height

    PageHeader {
        id: header
        width: parent.width -
               (image.visible ? image.width + Theme.paddingMedium : 0)
    }

    Image {
        id: image
        visible: source.toString()
        anchors {
            verticalCenter: header.verticalCenter
            right: parent.right
            rightMargin: Theme.paddingMedium
        }
        width: Theme.iconSizeLauncher
        height: Theme.iconSizeLauncher
        fillMode: Image.PreserveAspectFit
    }
}
