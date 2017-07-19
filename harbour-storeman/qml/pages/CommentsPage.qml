import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias appId: commentsModel.appId
    property alias commentField: commentsList.headerItem
    property int userId
    property string userName
    property bool hasComments: false

    allowedOrientations: defaultAllowedOrientations

    Connections {
        target: ornClient
        onCommentAdded: commentsModel.addComment(cid)
        onCommentEdited: commentsModel.editComment(cid)
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            RefreshMenuItem {
                model: commentsModel
            }
        }

        PageHeader {
            id: pageHeader
            //% "Comments"
            title: qsTrId("orn-comments")
            description: userName
        }

        SilicaListView {
            id: commentsList
            anchors {
                left: parent.left
                top: pageHeader.bottom
                right: parent.right
                bottom: parent.bottom
            }
            clip: true
            verticalLayoutDirection: ListView.BottomToTop

            header: CommentField { }

            model: OrnCommentsModel {
                id: commentsModel
                Component.onCompleted: apiRequest.networkManager = dataAccessManager
            }

            delegate: CommentDelegate { }

            VerticalScrollDecorator { }

            BusyIndicator {
                size: BusyIndicatorSize.Large
                anchors.centerIn: parent
                running: commentsList.count === 0 && hasComments
            }
        }
    }

    NumberAnimation {
        id: moveAnimation
        target: commentsList
        property: "contentY"
    }
}
