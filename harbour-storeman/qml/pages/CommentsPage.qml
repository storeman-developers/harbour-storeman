import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias appId: commentsModel.appId
    property string userName

    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: commentsList
        anchors.fill: parent

        header: PageHeader {
            id: header
            //% "Comments"
            title: qsTrId("orn-comments")
            description: userName
        }

        model: OrnCommentsModel {
            id: commentsModel
            Component.onCompleted: apiRequest.networkManager = dataAccessManager
        }

        delegate: CommentDelegate { }

        PullDownMenu {
            RefreshMenuItem {
                model: commentsModel
            }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: commentsList.count === 0
        }
    }

    NumberAnimation {
        id: moveAnimation
        target: commentsList
        property: "contentY"
    }
}
