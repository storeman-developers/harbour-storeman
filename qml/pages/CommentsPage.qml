import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property alias commentField: commentsList.headerItem
    property int appId
    property string appName
    property string appIconSource
    property int userId
    property bool hasComments: false
    property bool _hintMode: Storeman.showHint(Storeman.CommentDelegateHint)
    property OrnCommentsModel commentsModel: null

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
                var shComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanTapHint.qml"))
                var shObj = shComp.createObject(label)

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

    Connections {
        target: commentsModel

        onRowsInserted: hasComments = commentsModel.rowCount() > 0
        onRowsRemoved: hasComments = commentsModel.rowCount() > 0
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
            visible: pageHeader.height > 0.0

            RefreshMenuItem {
                model: commentsModel
            }

            MenuStatusLabel { }
        }

        FancyPageHeader {
            id: pageHeader
            //% "Comments"
            title: qsTrId("orn-comments")
            description: appName
            iconSource: appIconSource

            // Hide header when typing comment
            states: State {
                name: "hideOnTyping"
                when: page.isLandscape && commentField.item.isActive
                PropertyChanges {
                    target: pageHeader
                    explicit: true
                    height: 0
                    opacity: 0.0
                    visible: false
                }
            }

            transitions: Transition {
                NumberAnimation {
                    properties: "height,opacity"
                }
            }
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
                if (!networkManager.connected) {
                    return qsTrId("orn-network-idle")
                }
                if (commentsModel.networkError) {
                    hintText = qsTrId("orn-pull-refresh")
                    return qsTrId("orn-network-error")
                }
                if (commentsList.count === 0 && !hasComments) {
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
                source: networkManager.connected && OrnClient.authorised && OrnClient.cookieIsValid ?
                            Qt.resolvedUrl("../components/CommentField.qml") :
                            Qt.resolvedUrl("../components/CommentLabel.qml")
            }

            delegate: CommentDelegate { }

            VerticalScrollDecorator { }
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
