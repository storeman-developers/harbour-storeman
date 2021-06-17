import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Dialog {
    property alias hour: hoursView.currentIndex
    property alias minute: minutesView.currentIndex

    id: dialog
    canAccept: hour > 0 || minute >= 10

    DialogHeader {
        id: header
        //% "At least 10 minutes"
        title: qsTrId("orn-updates-check-interval-minimum")
        palette.highlightColor: canAccept ? Theme.highlightColor : Theme.errorColor
    }

    IntervalView {
        id: hoursView
        anchors {
            top: header.bottom
            right: parent.horizontalCenter
            bottom: parent.bottom
            left: parent.left
            bottomMargin: Theme.paddingLarge
        }
        model: 48
    }

    IntervalView {
        id: minutesView
        anchors {
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.horizontalCenter
            bottomMargin: Theme.paddingLarge
        }
        model: 60
    }
}
