import QtQuick 2.0

NumberAnimation {
    property var mode: ListView.Center

    function moveTo(index) {
        stop()
        from = target.contentY
        target.positionViewAtIndex(index, mode)
        // FIXME: Sometimes it's not defined
        to = target.contentY
        start()
    }

    property: "contentY"
}
