import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Item {
    width: commentsList.width
    height: content.height + Theme.paddingLarge * 2

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
                    color: Theme.secondaryColor
                    wrapMode: Text.WordWrap
                    text: commentData.userName
                }

                Label {
                    width: parent.width
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    text: commentData.date
                }

                Label {
                    readonly property OrnCommentListItem replyTo: commentsModel.findItem(commentData.parentId)

                    width: parent.width
                    visible: replyTo
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    //% "Reply to %0"
                    text: visible ? qsTrId("orn-reply-to").arg(replyTo.userName) : ""

                    MouseArea {
                        anchors.fill: parent
                        // TODO: Something more elegant
                        onClicked: {
                            moveAnimation.stop()
                            moveAnimation.from = commentsList.contentY
                            commentsList.positionViewAtIndex(
                                        commentsModel.findItemRow(parent.replyTo.commentId),
                                        ListView.Beginning)
                            moveAnimation.to = commentsList.contentY
                            moveAnimation.start()
                        }
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
            text: commentData.text
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
