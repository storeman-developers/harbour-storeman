import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Column {
    property alias isActive: body.activeFocus
    property string _editId
    property string _replyToId
    readonly property bool _hasText: body.text.trim()

    function reply(cid, name, text) {
        _editId = ""
        _replyToId = cid
        body.text = ""
        body.forceActiveFocus()
        _setTypeLabel(qsTrId("orn-reply-to").arg(name), text)
    }

    function edit(cid, text) {
        _editId = cid
        _replyToId = ""
        body.text = text
        body.forceActiveFocus()
        //% "Edit your comment"
        _setTypeLabel(qsTrId("orn-comment-edit-label"), text)
        body.cursorPosition = body.text.length
    }

    function _setTypeLabel(title, preview) {
        // OpenRepos returns comments in paragraph tags so it goes to the next line
        typeLabel.text = "<font color='%0'>%1</font><font color='%2'>%3</font>"
            .arg(Theme.secondaryColor).arg(title).arg(Theme.highlightColor).arg(preview)
    }

    function _reset() {
        _editId = ""
        _replyToId = ""
        body.text = ""
        typeLabel.text = ""
    }

    width: parent.width
    spacing: Theme.paddingSmall

    Item {
        id: typeItem
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }
        height: _editId || _replyToId ?
                    Math.max(typeIcon.height, typeLabel.height, cancelButton.height) : 0

        Behavior on height { NumberAnimation { } }

        Image {
            id: typeIcon
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            source: {
                if (_editId) {
                    return "image://theme/icon-m-edit?" + Theme.highlightColor
                }
                if (_replyToId) {
                    return "image://theme/icon-m-rotate-left?" + Theme.highlightColor
                }
                return ""
            }
        }

        Label {
            id: typeLabel
            anchors {
                left: typeIcon.right
                right: cancelButton.left
                leftMargin: Theme.paddingMedium
                rightMargin: Theme.paddingMedium
                verticalCenter: parent.verticalCenter
            }
            maximumLineCount: 2
            wrapMode: Text.WordWrap
            truncationMode: TruncationMode.Fade
            font.pixelSize: Theme.fontSizeExtraSmall
        }

        IconButton {
            id: cancelButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            visible: parent.height
            icon.source: "image://theme/icon-m-dismiss?" +
                  (pressed ? Theme.highlightColor : Theme.primaryColor)
            onClicked: _reset()
        }
    }

    Item {
        id: spacer
        height: Theme.paddingMedium
        width: parent.width
    }

    Flickable {
        id: tagsPanel
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }
        height: tagsRow.height + Theme.paddingMedium
        contentWidth: tagsRow.width
        clip: true
        opacity: body.activeFocus ? 1.0 : 0.0
        visible: opacity > 0.0

        Behavior on opacity { NumberAnimation { } }

        Row {
            id: tagsRow
            spacing: Theme.paddingSmall

            HtmlTagButton {
                //: Tag strong
                //% "B"
                text: "<b>%0</b>".arg(qsTrId("orn-tag-strong"))
                tag: "strong"
            }

            HtmlTagButton {
                //: Tag emphasize
                //% "I"
                text: "<i>%0</i>".arg(qsTrId("orn-tag-emphasize"))
                tag: "em"
            }

            HtmlTagButton {
                //: Tag underscore
                //% "U"
                text: "<u>%0</u>".arg(qsTrId("orn-tag-underscore"))
                tag: "u"
            }

            HtmlTagButton {
                text: "🙶"
                tag: "blockquote"
            }

            HtmlTagButton {
                text: '<font face="monospace">‹›</font>'
                tag: "code"
            }

            HtmlTagButton {
                text: '<font face="monospace">pre</font>'
                tag: "pre"
            }

            HtmlTagButton {
                text: "🔗"
                tag: "a"
                attrs: ' href=""'
            }
        }
    }

    TextArea {
        id: body
        width: parent.width - sendButton.width - Theme.horizontalPageMargin
        //% "Your comment"
        label: qsTrId("orn-comment-body")
        placeholderText: label
        font.pixelSize: Theme.fontSizeSmall
        focusOutBehavior: FocusBehavior.KeepFocus

        Component.onCompleted: _editor.textFormat = TextEdit.PlainText

        Label {
            id: sendButton
            anchors {
                verticalCenter: parent.top
                verticalCenterOffset: body.textVerticalCenterOffset + (body._editor.height - height)
            }
            x: parent.x + parent.width
            color: !_hasText ? Theme.secondaryColor :
                        sendButtonMouseArea.pressed ? Theme.highlightColor : Theme.primaryColor
            font.pixelSize: Theme.fontSizeSmall
            text: {
                if (opacity < 1.0) {
                    return ""
                }
                if (_editId) {
                    //: Update a comment
                    //% "Update"
                    return qsTrId("orn-comment-update")
                }
                if (_replyToId) {
                    return qsTrId("orn-reply")
                }
                //% "Send"
                return qsTrId("orn-comment-send")
            }

            opacity: body.text || body.activeFocus ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation { } }

            MouseArea {
                id: sendButtonMouseArea
                anchors.fill: parent
                onClicked: {
                    if (_editId) {
                        OrnClient.editComment(_editId, body.text)
                    } else if (_replyToId) {
                        OrnClient.comment(appId, body.text, _replyToId)
                    } else {
                        OrnClient.comment(appId, body.text)
                    }
                    _reset()
                }
            }
        }
    }
}
