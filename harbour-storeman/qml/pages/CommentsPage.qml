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
        target: OrnClient
        onCommentAdded: commentsModel.addComment(cid)
        onCommentEdited: commentsModel.editComment(cid)
    }

    Timer {
        id: createdUpdateTimer
        interval: 30000
        repeat: true
        running: true
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {

            RefreshMenuItem {
                model: commentsModel
            }

            MenuStatusLabel { }
        }

        PageHeader {
            id: pageHeader
            // Hide header when typing comment
            height: orientation === Orientation.Landscape && commentField.item.isActive ?
                        0.0 : _preferredHeight + Theme.paddingMedium
            visible: height > 0.0
            //% "Comments"
            title: qsTrId("orn-comments")
            description: userName

            Behavior on height { NumberAnimation { } }
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

            header: Loader {
                width: parent.width
                source: OrnClient.authorised && OrnClient.cookieIsValid ?
                            Qt.resolvedUrl("../components/CommentField.qml") :
                            Qt.resolvedUrl("../components/CommentLabel.qml")
            }

            model: OrnCommentsModel {
                id: commentsModel
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
        function moveTo(pos) {
            stop()
            from = commentsList.contentY
            commentsList.positionViewAtIndex(pos, ListView.End)
            // FIXME: Sometimes it's not defined
            to = commentsList.contentY
            start()
        }

        id: moveAnimation
        target: commentsList
        property: "contentY"
    }
}
