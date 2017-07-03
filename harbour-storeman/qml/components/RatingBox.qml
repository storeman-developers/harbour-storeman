import QtQuick 2.0
import Sailfish.Silica 1.0

Loader {
    anchors.verticalCenter: parent.verticalCenter
    sourceComponent: app.ratingCount > 0 ? starBox : notRated

    Component {
        id: starBox

        Row {

            Repeater {
                readonly property int rating: app.rating / 20.0 + 0.5

                id: ratingBox
                model: 5

                Image {
                    width: Theme.iconSizeExtraSmall
                    height: Theme.iconSizeExtraSmall
                    opacity: index < ratingBox.rating ? 1.0 : 0.5
                    source: "image://theme/icon-s-favorite?" +
                            (index < ratingBox.rating ?
                                 "gold" : Theme.secondaryColor)
                }
            }
        }
    }

    Component {
        id: notRated

        Label {
            font.pixelSize: Theme.fontSizeTiny
            color: Theme.secondaryColor
            //% "Not rated yet"
            text: qsTrId("orn-notrated")
        }
    }
}
