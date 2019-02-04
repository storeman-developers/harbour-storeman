import QtQuick 2.0
import Sailfish.Silica 1.0

InteractionHintLabel {
    property var hint

    signal finished()

    id: label
    opacity: hint && hint.running ? 1.0 : 0.0

    Behavior on opacity {
        FadeAnimation {
            duration: 1000

            onRunningChanged: {
                if (!running && label.opacity === 0.0) {
                    finished()
                }
            }
        }
    }
}
