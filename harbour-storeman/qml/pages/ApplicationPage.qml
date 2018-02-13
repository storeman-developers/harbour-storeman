import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool returnToUser: false
    property alias appId: app.appId
    readonly property int _packageStatus: app.packageStatus

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        // Wait until page loads to prevent lagging
        if (status === PageStatus.Active) {
            app.ornRequest()
        }
    }

    OrnApplication {
        id: app
        onOrnRequestFinished: flickable.visible = true
    }

    BusyIndicator {
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: !flickable.visible
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        visible: false
        contentHeight: content.height

        ApplicationPageMenu {
            id: pageMenu
        }

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            FancyPageHeader {
                title: app.title
                description: app.userName
                iconSource: app.iconSource
            }

            PackageInformation {
                id: packageInfo
            }

            Item {
                // Spacer
                width: parent.width
                height: Theme.paddingSmall
            }

            AppInformation { }

            ScreenshotsBox { }

            MoreButton {
                visible: app.changelog
                //% "Changelog"
                text: qsTrId("orn-changelog")
                onClicked: pageStack.push(Qt.resolvedUrl("ChangelogPage.qml"), {
                                              changelog: app.changelog
                                          })
            }

            MoreButton {
                visible: OrnClient.authorised || app.commentsCount
                //% "Comments (%0)"
                text: app.commentsCount ? qsTrId("orn-comments-withnum").arg(app.commentsCount) :
                                          qsTrId("orn-comments")
                onClicked: pageStack.push(Qt.resolvedUrl("CommentsPage.qml"), {
                                              appId: app.appId,
                                              userId: app.userId,
                                              userName: app.userName,
                                              hasComments: app.commentsCount
                                          })
            }

            MoreButton {
                //% "More by %0"
                text: qsTrId("orn-author-apps").arg(app.userName)
                onClicked: returnToUser ?
                               // This page was openned from user page so just go back
                               pageStack.navigateBack():
                               // Open a new user page
                               pageStack.push(Qt.resolvedUrl("UserAppsPage.qml"), {
                                                  userId: app.userId,
                                                  userName: app.userName,
                                                  userIcon: app.userIconSource,
                                                  previousAppId: app.appId
                                              })
            }
        }

        VerticalScrollDecorator { }
    }
}
