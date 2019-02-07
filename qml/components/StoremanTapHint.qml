import QtQuick 2.2
import Sailfish.Silica 1.0

Image {
    property alias running: blinkAnimation.running

    function start() {
        blinkAnimation.start()
    }

    function stop() {
        blinkAnimation.stop()
    }

    id: root
    anchors.centerIn: parent
    source: "image://theme/graphic-gesture-hint?" + Theme.primaryColor
    opacity: 0.0

    onRunningChanged: if (!running) opacity = 0.0

    SequentialAnimation {
        id: blinkAnimation
        loops: Animation.Infinite

        OpacityAnimator {
            target: root
            from: 0.0
            to: 1.0
            duration: 200
        }

        PauseAnimation {
            duration: 200
        }

        OpacityAnimator {
            target: root
            from: 1.0
            to: 0.0
            duration: 800
        }
    }
}
