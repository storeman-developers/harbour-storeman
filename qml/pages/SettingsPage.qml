import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"


Page {
    id: page
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            Storeman.showRecentOnStart = showRecentOnStartSwitch.checked
            Storeman.smartUpdate = smartUpdateSwitch.checked
            Storeman.showUpdatesNotification = showUpdatesNotificationSwitch.checked
            Storeman.updateInterval = checkUpdatesIntervalButton.interval
            Storeman.checkForUpdates = checkForUpdatesSwitch.checked
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            PageHeader {
                //% "Settings"
                title: qsTrId("orn-settings")
            }

            ListMenuItem {
                iconSource: OrnClient.userIconSource ?
                                OrnClient.userIconSource : "image://theme/icon-m-person"
                text: OrnClient.authorised ?
                          //% "Logged in as %0"
                          qsTrId("orn-loggedin-menu-item").arg(OrnClient.userName) :
                          //% "Log in to OpenRepos.net"
                          qsTrId("orn-login-menu-item")
                menu: ContextMenu {
                    MenuItem {
                        visible: OrnClient.isPublisher
                        //% "My applications"
                        text: qsTrId("orn-myapps")
                        onClicked: pageStack.push(Qt.resolvedUrl("UserAppsPage.qml"), {
                                                      userId: OrnClient.userId,
                                                      userName: OrnClient.userName,
                                                      userIcon: OrnClient.userIconSource
                                                  })
                    }

                    MenuItem {
                        //: Menu item
                        //% "Log out"
                        text: qsTrId("orn-logout-action")
                        onClicked: {
                            if (OrnClient.authorised) {
                                //: Remorse text
                                //% "Logging out"
                                Remorse.popupAction(page, qsTrId("orn-logout-remorse"), OrnClient.logout)
                            }
                        }
                    }
                }

                onClicked: {
                    if (OrnClient.authorised) {
                        openMenu()
                    } else if (networkManager.online) {
                        pageStack.push(Qt.resolvedUrl("AuthorisationDialog.qml"))
                    }
                }
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-backup"
                text: qsTrId("orn-backups")
                onClicked: pageStack.push(Qt.resolvedUrl("BackupsPage.qml"))
            }

            SectionHeader {
                //% "Main page"
                text: qsTrId("orn-mainpage")
            }

            TextSwitch {
                id: showRecentOnStartSwitch
                //% "Show recently updated on start"
                text: qsTrId("orn-show-recent-switch")
                //% "Switch to the page of recently updated packages on start"
                description: qsTrId("orn-show-recent-switch-descr")
                checked: Storeman.showRecentOnStart
            }

            ListMenuItem {
                iconSource: "image://theme/icon-m-transfer"
                text: qsTrId("orn-mainpage-order")
                onClicked: pageStack.push(Qt.resolvedUrl("MainPageOrderDialog.qml"))
            }

            SectionHeader {
                //% "Updates"
                text: qsTrId("orn-updates")
            }

            TextSwitch {
                id: checkForUpdatesSwitch
                checked: Storeman.checkForUpdates
                //% "Check for updates"
                text: qsTrId("orn-check-for-updates-switch")
                //% "Updates are checked only when the Storeman is running"
                description: qsTrId("orn-check-for-updates-descr")
            }

            TextSwitch {
                id: smartUpdateSwitch
                checked: Storeman.smartUpdate
                //% "Smart check"
                text: qsTrId("orn-smart-check-switch")
                //% "Use the OpenRepos.net API to determine if there are new updates"
                description: qsTrId("orn-smart-check-descr")

                DisappearAnimation {
                    target: smartUpdateSwitch
                    show: checkForUpdatesSwitch.checked
                }
            }

            TextSwitch {
                id: showUpdatesNotificationSwitch
                checked: Storeman.showUpdatesNotification
                //% "Show updates notification"
                text: qsTrId("orn-updates-notification-switch")
            }

            ValueButton {
                property int interval: Storeman.updateInterval

                id: checkUpdatesIntervalButton
                //% "Updates check interval"
                label: qsTrId("orn-updates-check-interval")
                value: {
                    var m = interval / 60000
                    if (m < 60) {
                        //% "%n minute(s)"
                        return qsTrId("orn-interval-m", m)
                    } else {
                        var h = Math.floor(m / 60)
                        m -= (h * 60)
                        //% "%n hour(s)"
                        var res = qsTrId("orn-interval-h", h)
                        if (m > 0) {
                            res += " " + qsTrId("orn-interval-m", m)
                        }
                        return res
                    }
                }

                onClicked: {
                    var m = interval / 60000
                    var h = Math.floor(m / 60)
                    m -= (h * 60)
                    var d = pageStack.push("Sailfish.Silica.TimePickerDialog", {
                        hour: h,
                        minute: m
                    })
                    d.accepted.connect(function() {
                        var res = (d.hour * 60 + d.minute) * 60000
                        if (res >= 600000) {
                            interval = res
                        } else {
                            //% "The interval must be at least 10 minutes"
                            notification.show(qsTrId("orn-updates-check-interval-invalid"),
                                              "image://theme/icon-s-high-importance")
                        }
                    })
                }

                DisappearAnimation {
                    target: checkUpdatesIntervalButton
                    show: checkForUpdatesSwitch.checked
                }
            }
        }
    }
}
