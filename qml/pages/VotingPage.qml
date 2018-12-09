import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Page {
    property int appId: 0
    property int userVote: 0
    property string appName: ""

    id: page
    opacity: 0.0
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Active) {
            placeholder.opacity = networkManager.online ? 0.0 : 1.0
            loader.opacity = networkManager.online ? 1.0 : 0.0
            opacity = 1.0
        }
    }

    Behavior on opacity { FadeAnimation { } }

    Timer {
        id: timer
        interval: 1000
        running: false
        repeat: false
        onTriggered: pageStack.pop()
    }

    SequentialAnimation {
        id: animation

        FadeAnimation {
            target: networkManager.online ? placeholder : loader
            to: 0.0
        }
        FadeAnimation {
            target: networkManager.online ? loader : placeholder
            to: 1.0
        }
    }

    Connections {
        target: networkManager
        onOnlineChanged: if (!timer.running) animation.start()
    }

    PageHeader {
        id: votingPageHeader
        //% "Rate the application"
        title: qsTrId("orn-rate-app")
    }

    InfoLabel {
        id: placeholder
        anchors.verticalCenter: parent.verticalCenter
        visible: opacity > 0.0
        text: qsTrId("orn-network-idle")
    }

    Loader {
        id: loader
        anchors.fill: parent
        visible: opacity > 0.0
        sourceComponent: OrnClient.authorised ? rateComponent : loginComponent
    }

    Component {
        id: loginComponent

        Item {

            BackgroundItem {
                id: loginItem
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                height: loginLabel.implicitHeight + Theme.paddingSmall * 2
                onClicked: pageStack.push(Qt.resolvedUrl("AuthorisationDialog.qml"))

                Label {
                    id: loginLabel
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Theme.horizontalPageMargin
                        verticalCenter: parent.verticalCenter
                    }
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeExtraLarge
                    color: loginItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    //% "Log in to rate the application"
                    text: qsTrId("orn-login2rate")
                }
            }
        }
    }

    Component {
        id: rateComponent

        Item {
            anchors.fill: parent

            Row {
                id: starBox
                anchors.centerIn: parent
                spacing: Theme.paddingMedium

                Repeater {
                    model: 5

                    Image {
                        readonly property int _vote: (index + 1) * 20
                        readonly property bool _golden: _vote <= userVote

                        width: Theme.iconSizeMedium
                        height: Theme.iconSizeMedium
                        opacity: _golden ? 1.0 : 0.5
                        source: "image://theme/icon-m-favorite-selected?" +
                                (_golden ? "gold" : Theme.secondaryColor)

                        MouseArea {
                            anchors.fill: parent
                            preventStealing: true
                            onClicked: {
                                // Block the UI at first
                                blocker.enabled = true
                                if (userVote !== parent._vote) {
                                    userVote = parent._vote
                                    OrnClient.vote(appId, userVote)
                                    //% "Your vote has been sent"
                                    notification.show(qsTrId("orn-vote-send"), "image://theme/icon-s-like")
                                    timer.start()
                                } else {
                                    pageStack.pop()
                                }
                            }
                        }
                    }
                }
            }

            InfoLabel {
                anchors {
                    top: starBox.bottom
                    topMargin: Theme.paddingLarge * 2
                }
                text: appName
            }

            TouchBlocker {
                id: blocker
                anchors.fill: parent
                enabled: false
            }
        }
    }
}
