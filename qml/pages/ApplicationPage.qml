import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool returnToUser: false
    property int appId: 0
    property OrnApplication app: Storeman.cachedApp(appId)
    readonly property int _packageStatus: app.packageStatus
    property int _commentsCount: app && app.commentsCount

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        // Wait until page loads to prevent lagging
        // And don't call ornRequest() if the app was already loaded
        if (!app.packageName && status === PageStatus.Active) {
            app.ornRequest()
        }
    }

    OrnCommentsModel {
        id: commentsModel

        onRowsInserted: _commentsCount = rowCount()
        onRowsRemoved: _commentsCount = rowCount()
    }

    Connections {
        target: app
        onOrnRequestFinished: {
            // Show rating hint
            if (Storeman.showHint(Storeman.ApplicationRatingHint)) {
                var shComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanHint.qml"))
                var shObj = shComp.createObject(packageInfo.ratingBox, {
                    distance: 0.0,
                    "anchors.centerIn": packageInfo.ratingBox
                })

                var shlComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanHintLabel.qml"))
                var shlObj = shlComp.createObject(page, {
                    hint: shObj,
                    //% "Tap to rate the application"
                    text: qsTrId("orn-hint-rating"),
                    "anchors.bottom": page.bottom
                })

                shlObj.finished.connect(function() {
                    Storeman.setHintShowed(Storeman.ApplicationRatingHint)
                    shlObj.destroy()
                    shObj.destroy()
                })

                shObj.start()
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
        visible: app.packageName && !app.running
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
                visible: app.commentsOpen && (OrnClient.authorised || app.commentsCount)
                //% "Comments (%0)"
                text: _commentsCount ? qsTrId("orn-comments-withnum").arg(_commentsCount) :
                                       qsTrId("orn-comments")
                onClicked: {
                    commentsModel.appId = appId
                    pageStack.push(Qt.resolvedUrl("CommentsPage.qml"), {
                                       commentsModel: commentsModel,
                                       appId: app.appId,
                                       userId: app.userId,
                                       userName: app.userName,
                                       hasComments: _commentsCount > 0
                                   })
                }
            }

            MoreButton {
                visible: app.tagIds.length
                text: qsTrId("orn-tags")
                onClicked: pageStack.push(Qt.resolvedUrl("TagsPage.qml"), {
                                              tagIds: app.tagIds,
                                              appName: app.title,
                                              appIconSource: app.iconSource,
                                              previousAppId: app.appId
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
