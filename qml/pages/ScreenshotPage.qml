import QtQuick 2.0
import Sailfish.Silica 1.0

FullscreenContentPage {
    property alias model: slideshow.model
    property alias currentIndex: slideshow.currentIndex

    id: page
    allowedOrientations: defaultAllowedOrientations

    SlideshowView {
        id: slideshow
        anchors.fill: parent

        delegate: Item {
            width: slideshow.width
            height: slideshow.height

            BusyIndicator {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: image.status === Image.Loading
                color: Theme.highlightColor
            }

            Image {
                id: image
                anchors.fill: parent
                source: modelData.url
                fillMode: Image.PreserveAspectFit
                opacity: status === Image.Ready ? 1.0 : 0.0

                Behavior on opacity { FadeAnimation { } }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: pageStack.pop()
            }
        }
    }
}
