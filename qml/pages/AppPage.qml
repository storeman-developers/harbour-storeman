import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool returnToUser: false
    property int appId: 0
    readonly property OrnApplication app: Storeman.cachedApp(appId)
    readonly property int _packageStatus: app.packageStatus
    property int _commentsCount: app.commentsCount

    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        // Wait until page loads to prevent lagging
        // And don't call ornRequest() if the app was already loaded
        if (status === PageStatus.Active) {
            if (_packageStatus === OrnPm.PackageUnknownStatus) {
                app.ornRequest()
            } else {
                flickable.visible = true
            }
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
            flickable.visible = true

            // Show rating hint
            if (Storeman.showHint(Storeman.ApplicationRateAndBookmarkHint)) {
                var shComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanTapHint.qml"))
                var shObj = shComp.createObject(packageInfo.likeButton)

                var shlComp = Qt.createComponent(Qt.resolvedUrl("../components/StoremanHintLabel.qml"))
                var shlObj = shlComp.createObject(page, {
                    hint: shObj,
                    //% "Tap to rate the application"
                    text: qsTrId("orn-hint-rating")
                })

                var secondHint = false

                shlObj.finished.connect(function() {
                    if (secondHint) {
                        Storeman.setHintShowed(Storeman.ApplicationRateAndBookmarkHint)
                        shlObj.destroy()
                        shObj.destroy()
                    } else {
                        secondHint = true
                        //% "Tap to bookmark the application"
                        shlObj.text = qsTrId("orn-hint-bookmark")
                        shObj.parent = packageInfo.starButton
                        shObj.start()
                    }
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
        visible: false
        contentHeight: content.height

        AppPageMenu {
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
                onClicked: {
                    // try to extract the url of the first link from the changelog
                    var regex = /^(.*)<a href="(http[s]?:\/\/[^\s]*)"[>]*>[^<]*<\/a>(.*)$/
                    var parts = regex.exec(app.changelog)
                    if (parts
                            && parts.length === 4
                            && (parts[1].length + parts[3].length) < 32) {
                        // if an url link found in the changelog
                        // and the non-URL part of the changelog is short
                        // for e.g. "see: <a href="https://github.com/">...
                        // then open the URL automatically
                        Qt.openUrlExternally(parts[2])
                    } else {
                        pageStack.push(Qt.resolvedUrl("ChangelogPage.qml"), {
                                           appName: app.title,
                                           appIconSource: app.iconSource,
                                           changelog: app.changelog
                                       })
                    }
                }
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
                                       appName: app.title,
                                       appIconSource: app.iconSource,
                                       userId: app.userId,
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
                               pageStack.push(Qt.resolvedUrl("RepositoryPage.qml"), {
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
