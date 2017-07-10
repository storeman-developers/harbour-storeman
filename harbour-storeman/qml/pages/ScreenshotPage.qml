import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property alias model: slideshow.model
    property alias currentIndex: slideshow.currentIndex

    id: page
    backNavigation: false
    allowedOrientations: defaultAllowedOrientations

    SlideshowView {
        id: slideshow
        anchors.fill: parent
        delegate: Item {
            width: slideshow.width
            height: slideshow.height

            Image {
                anchors.fill: parent
                source: modelData.url
                fillMode: Image.PreserveAspectFit
                opacity: 0.0

                onStatusChanged: {
                    if (status === Image.Ready) {
                        opacity = 1.0
                    }
                }

                Behavior on opacity { FadeAnimation { } }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: pageStack.pop()
            }
        }
    }
}
