import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    readonly property bool _userComment: ornClient && ornClient.userId === commentData.userId
    readonly property bool _authorComment: commentData.userId === userId

    contentHeight: content.height + Theme.paddingMedium * 2
    menu: userAuthorised ? contextMenu : null
    highlighted: userAuthorised && down
    _showPress: highlighted

    ContextMenu {
        id: contextMenu

        MenuItem {
            //: Menu item to reply for a comment - should be a verb
            //% "Reply"
            text: qsTrId("orn-reply")
            onClicked: commentField.reply(commentData.commentId, commentData.userName, commentData.text)
        }

        MenuItem {
            //% "Edit"
            text: qsTrId("orn-edit")
            visible: _userComment
            onClicked: commentField.edit(commentData.commentId, commentData.text)
        }

//        MenuItem {
//            //% "Delete"
//            text: qsTrId("orn-delete")
//            visible: _userComment || ornClient.userId === userId
//            //% "Deleting"
//            onClicked: remorseAction(qsTrId("orn-deleting"), function() {
//                ornClient.deleteComment(commentData.commentId)
//                commentsModel.removeRow(index, 1)
//            })
//        }
    }

    Column {
        id: content
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }
        spacing: Theme.paddingMedium

        Item {
            width: parent.width
            height: Math.max(userImage.height, contentHeader.height)

            Image {
                id: userImage
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.iconSizeMedium
                height: Theme.iconSizeMedium
                fillMode: Image.PreserveAspectFit
                source: commentData.userIconSource ? commentData.userIconSource :
                                                     "image://theme/icon-m-person?" + Theme.highlightColor
            }

            Column {
                id: contentHeader
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: userImage.right
                    right: parent.right
                    leftMargin: Theme.paddingMedium
                }

                Label {
                    width: parent.width
                    color: highlighted ? Theme.highlightColor :
                                         _userComment ? Theme.primaryColor : Theme.secondaryColor
                    wrapMode: Text.WordWrap
                    text: (_userComment ? "<img src='image://theme/icon-s-edit'> " :
                                          _authorComment ? "<img src='image://theme/icon-s-developer'> " : "") +
                          commentData.userName
                }

                Label {
                    width: parent.width
                    color: highlighted ? Theme.highlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    text: commentData.date
                }

                Label {
                    readonly property OrnCommentListItem replyTo: commentsModel.findItem(commentData.parentId)

                    width: parent.width
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    visible: replyTo
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    //: Active label to navigate to the original comment - should be a noun
                    //% "Reply to %0"
                    text: visible ? qsTrId("orn-reply-to").arg(replyTo.userName) : ""

                    MouseArea {
                        anchors.fill: parent
                        onClicked: moveAnimation.moveTo(
                                       commentsModel.findItemRow(parent.replyTo.commentId))
                    }
                }
            }
        }

        Label {
            width: parent.width
            color: Theme.highlightColor
            linkColor: Theme.primaryColor
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            text: commentData.text
                .replace(/<a([^>]*)>([^<]+)<\/a>/g,
                         '<a$1><font color="%0">$2</font></a>'.arg(Theme.primaryColor))
                .replace(/<pre>([^<]+)<\/pre>/g,
                        '<p style="color:%0;font-family:monospace;font-size:%1px;">$1</p>'
                            .arg(Theme.secondaryColor).arg(Theme.fontSizeTiny))
            onLinkActivated: {
                var match = link.match(/https:\/\/openrepos\.net\/.*#comment-(\d*)/)
                if (match) {
                    var pos = commentsModel.findItemRow(match[1])
                    if (pos !== -1) {
                        moveAnimation.moveTo(pos)
                        return
                    }
                }
                Qt.openUrlExternally(link)
            }
        }
    }
}
