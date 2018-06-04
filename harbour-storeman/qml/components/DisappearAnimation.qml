import QtQuick 2.2


ParallelAnimation {
    property bool show: true
    property var target: null

    id: animation

    onShowChanged: {
        if (show) {
            oa.from = 0.0
            oa.to = 1.0
            na.from = 0.0
            na.to = target.implicitHeight
        } else {
            oa.from = 1.0
            oa.to = 0.0
            na.from = target.implicitHeight
            na.to = 0.0
        }
        start()
    }

    OpacityAnimator {
        id: oa
        target: animation.target
        duration: 200
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id: na
        target: animation.target
        property: "height"
        duration: 200
        easing.type: Easing.InOutQuad
    }
}
