import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias appId: commentsModel.appId
    property string userName

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

        delegate: ListItem {
            contentHeight: content.height + Theme.paddingLarge * 2
            highlighted: false

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
                                                             "image://theme/icon-m-person"
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
                            color: Theme.primaryColor
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

        PullDownMenu {
            RefreshMenuItem {
                model: commentsModel
            }
        }

        VerticalScrollDecorator {}

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: commentsList.count === 0
        }
    }
}
