import QtQuick 2.0
import Sailfish.Silica 1.0

// Silica button height is hardcoded
MouseArea {
    property string tag
    property string attrs: ""
    property alias text: buttonText.text
    property bool _showPress: (pressed && containsMouse) || pressTimer.running

    height: Theme.itemSizeExtraSmall * 0.75
    width: Math.max(height, buttonText.width + Theme.paddingLarge)

    onPressedChanged: if (pressed) pressTimer.start()
    onCanceled: pressTimer.stop()

    onClicked: {
        var editor = body._editor
        var selected = editor.selectedText
        if (selected) {
            editor.remove(editor.selectionStart, editor.selectionEnd)
        }
        editor.insert(editor.cursorPosition, "<%0%1>%2</%0>".arg(tag).arg(attrs).arg(selected))
        if (!selected) {
            // 3 is for "</>" length
            editor.cursorPosition = editor.cursorPosition - tag.length - 3
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: Theme.paddingSmall
        color: _showPress ? Theme.rgba(Theme.highlightBackgroundColor, Theme.highlightBackgroundOpacity)
                          : Theme.rgba(Theme.primaryColor, 0.2)

        Label {
            id: buttonText
            anchors.centerIn: parent
            horizontalAlignment: Qt.AlignHCenter
            color: _showPress ? Theme.highlightColor : Theme.primaryColor
            textFormat: Text.RichText
        }
    }

    Timer {
        id: pressTimer
        interval: Theme.minimumPressHighlightTime
    }
}
