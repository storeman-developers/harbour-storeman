import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"


Dialog {
    allowedOrientations: defaultAllowedOrientations

    onAccepted: {
        Storeman.smartUpdate = smartUpdateSwitch.checked
        Storeman.showUpdatesNotification = showUpdatesNotificationSwitch.checked
        Storeman.updateInterval = checkUpdatesIntervalButton.interval
        Storeman.checkForUpdates = checkForUpdatesSwitch.checked
    }

    DialogHeader {
        id: header
        //% "Save"
        acceptText: qsTrId("orn-save")
    }

    SilicaFlickable {
        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: parent.bottom
        }
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
                horizontalAlignment: Qt.AlignRight
                //% "Settings"
                text: qsTrId("orn-settings")
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
