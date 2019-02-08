import QtQuick 2.0
import Sailfish.Silica 1.0


MouseArea {
    property bool _doClose: false
    property var hint
    property string text
    property alias invert: label.invert

    signal finished()

    anchors.fill: parent

    onClicked: {
        if (_doClose) {
            timer.stop()
            hint.stop()
        } else {
            _doClose = true
            timer.start()
        }
    }

    Timer {
        id: timer
        interval: 2000
        onTriggered: _doClose = false
    }

    InteractionHintLabel {
        id: label
        anchors {
            top: invert ? parent.top : undefined
            bottom: invert ? undefined : parent.bottom
        }
        opacity: hint && hint.running ? 1.0 : 0.0
        //% "Tap again to close the hint"
        text: _doClose ? qsTrId("orn-hint-close") : parent.text

        Behavior on opacity {
            FadeAnimation {
                duration: 1000

                onRunningChanged: {
                    if (!running && label.opacity === 0.0) {
                        _doClose = false
                        finished()
                    }
                }
            }
        }
    }
}

