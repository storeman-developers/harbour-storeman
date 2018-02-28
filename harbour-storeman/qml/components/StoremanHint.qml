import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property alias text: interactionHintLabel.text
    property alias hint: touchInteractionHint

    id: hintItem

    InteractionHintLabel {
        id: interactionHintLabel
        opacity: touchInteractionHint.running ? 1.0 : 0.0

        Behavior on opacity {
            FadeAnimation {
                duration: 1000

                // Unload component
                onRunningChanged: {
                    if (!running && interactionHintLabel.opacity === 0.0) {
                        hintItem.parent.active = false
                    }
                }
            }
        }
    }

    TouchInteractionHint {
        id: touchInteractionHint
    }
}
