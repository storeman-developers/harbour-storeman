import QtQuick 2.0
import Sailfish.Silica 1.0

Loader {
    property int ratingCount
    property real rating
    readonly property int _rating: rating / 20.0 + 0.5

    sourceComponent: ratingCount > 0 ? starBox : notRated

    Component {
        id: starBox

        Row {
            Repeater {
                id: ratingBox
                model: 5

                Image {
                    width: Theme.iconSizeExtraSmall
                    height: Theme.iconSizeExtraSmall
                    opacity: index < _rating ? 1.0 : 0.5
                    source: "image://theme/icon-s-favorite?" +
                            (index < _rating ?
                                 "gold" : Theme.secondaryColor)
                }
            }
        }
    }

    Component {
        id: notRated

        Label {
            height: Theme.iconSizeExtraSmall
            font.pixelSize: Theme.fontSizeTiny
            color: Theme.secondaryColor
            //% "Not rated yet"
            text: qsTrId("orn-notrated")
        }
    }
}
