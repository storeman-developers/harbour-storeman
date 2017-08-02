import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Item {
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
    height: userAuthorised ? body.height + typeItem.height + Theme.paddingLarge : 0

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

    TextArea {
        id: body
        anchors {
            top: typeItem.bottom
            topMargin: Theme.paddingMedium
        }
        width: parent.width - sendButton.width
        //% "Your comment"
        label: qsTrId("orn-comment-body")
        placeholderText: label
        font.pixelSize: Theme.fontSizeSmall
        focusOutBehavior: FocusBehavior.KeepFocus
    }

    Label {
        id: sendButton
        anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: body.top
            verticalCenterOffset: body.textVerticalCenterOffset + (body._editor.height - height)
        }
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
                    ornClient.editComment(_editId, body.text)
                } else if (_replyToId) {
                    ornClient.comment(appId, body.text, _replyToId)
                } else {
                    ornClient.comment(appId, body.text)
                }
                _reset()
            }
        }
    }
}
