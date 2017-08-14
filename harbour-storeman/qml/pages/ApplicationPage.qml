import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool returnToUser: false
    property alias appId: app.appId
    property string _prevState
    readonly property var locale: Qt.locale()

    // Store the previous state to reset it on error
    function setState(newState) {
        _prevState = state
        state = newState
    }

    id: page
    allowedOrientations: defaultAllowedOrientations
    state: "notinstalled"
    states: [
        State {
            name: "notinstalled"
            when: !app.installedVersion
            PropertyChanges {
                target: packageInfo.statusLabel
                running: false
                icon: ""
                //% "Not installed"
                text: qsTrId("orn-not-installed")
            }
        },
        State {
            name: "installed"
            when: !app.updateAvailable && app.installedVersion
            PropertyChanges {
                target: packageInfo.statusLabel
                running: false
                icon: "image://theme/icon-s-installed"
                //% "Installed"
                text: qsTrId("orn-installed")
            }
        },
        State {
            name: "updateavailable"
            when: app.updateAvailable
            PropertyChanges {
                target: packageInfo.statusLabel
                running: false
                icon: "image://theme/icon-s-update"
                //% "Update available"
                text: qsTrId("orn-update-available")
            }
        },
        State {
            name: "installing"
            PropertyChanges {
                target: packageInfo.statusLabel
                running: true
                //% "Installing"
                text: qsTrId("orn-installing")
            }
        },
        State {
            name: "updating"
            PropertyChanges {
                target: packageInfo.statusLabel
                running: true
                //% "Updating"
                text: qsTrId("orn-updating")
            }
        },
        State {
            name: "removing"
            PropertyChanges {
                target: packageInfo.statusLabel
                running: true
                text: qsTrId("orn-removing")
            }
        }
    ]

    // NOTE: is it working without filtering errors?
    Connections {
        target: ornZypp
        onPkError: state = _prevState
    }

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

        onInstalled: {
            pageMenu.busy = false
            //% "Package %0 was successfully installed"
            notification.show(qsTrId("orn-package-installed").arg(packageName))
        }

        onRemoved: {
            state = "notinstalled"
            pageMenu.busy = false
            //% "Package %0 was successfully removed"
            notification.show(qsTrId("orn-package-removed").arg(packageName))
        }
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
                visible: userAuthorised || app.commentsCount
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
