import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool returnToUser: false
    property alias appId: app.appId
    readonly property var locale: Qt.locale()

    id: page
    allowedOrientations: defaultAllowedOrientations

    OrnApplication {
        id: app

        onUpdated: flickable.visible = true

        onAppNotFound: {
            notification.showPopup(
                        //% "An error occured"
                        qsTrId("orn-error"),
                        //% "Application with such id was not found"
                        qsTrId("orn-app-not-found-body"))
            pageStack.navigateBack()
        }

        onUpdateError: {
            notification.showPopup(
                        qsTrId("orn-error"),
                        //% "An error occured while fetching application data"
                        qsTrId("orn-app-error-body"))
            pageStack.navigateBack()
        }

        //% "Package %0 was successfully installed"
        onInstalled: notification.show(qsTrId("orn-package-installed").arg(packageName))

        //% "Package %0 was successfully removed"
        onRemoved: notification.show(qsTrId("orn-package-removed").arg(packageName))
    }

    Connections {
        // Wait until page loads to prevent lagging
        target: pageStack
        onBusyChanged: {
            if (!pageStack.busy && pageStack.currentPage === page) {
                app.update()
            }
        }
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

        ApplicationPageMenu { }

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            FancyPageHeader {
                title: app.title
                description: app.userName
                iconSource: app.iconSource
            }

            PackageInformation { }

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
                visible: app.commentsCount || ornClient.authorised
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
