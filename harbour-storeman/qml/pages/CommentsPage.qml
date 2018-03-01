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
    property bool _hintMode: Storeman.showHint(Storeman.CommentDelegateHint)

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        // Wait until page loads to prevent lagging
        if (status === PageStatus.Active) {
            if (!_hintMode) {
                commentsList.model = commentsModel
            } else {
                var dcmComp = Qt.createComponent(Qt.resolvedUrl("../models/DummyCommentsModel.qml"))
                commentsList.model = dcmComp.createObject()

                var label = commentsList.currentItem.replyToLabel
                var halfSize = Theme.itemSizeLarge / 2

                var shComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanHint.qml"))
                var shObj = shComp.createObject(label, {
                                                    distance: 0.0,
                                                    startX: label.x + label.implicitWidth / 2 - halfSize,
                                                    startY: label.y + label.height / 2 - halfSize
                                                })

                var shlComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanHintLabel.qml"))
                var shlObj = shlComp.createObject(page, {
                                                      hint: shObj,
                                                      //% "Tap to navigate to the replied comment"
                                                      text: qsTrId("orn-hint-commentdelegate"),
                                                      invert: true
                                                  })

                shlObj.finished.connect(function() {
                    _hintMode = false
                    Storeman.setHintShowed(Storeman.CommentDelegateHint)
                    commentsList.model = commentsModel
                    shlObj.destroy()
                    shObj.destroy()
                })

                shObj.start()
            }
        }
    }

    OrnCommentsModel {
        id: commentsModel
    }

    Connections {
        target: OrnClient
        onCommentAdded: {
            commentsModel.addComment(cid)
            hasComments = true
        }
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
            id: menu

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
                source: networkManager.online && OrnClient.authorised && OrnClient.cookieIsValid ?
                            Qt.resolvedUrl("../components/CommentField.qml") :
                            Qt.resolvedUrl("../components/CommentLabel.qml")
            }

            delegate: CommentDelegate { }

            VerticalScrollDecorator { }
        }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !viewPlaceholder.text &&
                     commentsList.count === 0 && hasComments &&
                     !menu.active
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text && !commentField.item.isActive
            text: {
                hintText = ""
                if (!networkManager.online) {
                    return qsTrId("orn-network-idle")
                }
                if (commentsModel.apiRequest.networkError) {
                    hintText = qsTrId("orn-pull-refresh")
                    return qsTrId("orn-network-error")
                }
                if (commentsList.count === 0) {
                    if (OrnClient.userId === userId) {
                        //: This will be shown to an application author
                        //% "Wait for users' feedback"
                        hintText = qsTrId("orn-comments-wait")
                    } else {
                        //: This will be shown to a normal user
                        //% "Be the first to comment"
                        hintText = qsTrId("orn-comments-bethefirst")
                    }
                    //% "There is nothing here yet"
                    return qsTrId("orn-comments-nocomments")
                }
                return ""
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
