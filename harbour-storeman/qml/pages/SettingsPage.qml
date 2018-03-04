import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Page {
    allowedOrientations: defaultAllowedOrientations

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

            SectionHeader {
                //% "Updates"
                text: qsTrId("orn-updates")
            }

            TextSwitch {
                id: showUpdatesNotificationSwitch
                //% "Show updates notification"
                text: qsTrId("orn-updates-notification-switch")
                onCheckedChanged: Storeman.showUpdatesNotification = checked

                Component.onCompleted: {
                    checked = Storeman.showUpdatesNotification
                }
            }

            ValueButton {
                id: checkUpdatesIntervalButton
                //% "Updates check interval"
                label: qsTrId("orn-updates-check-interval")
                value: {
                    var m = Storeman.updateInterval / 60000
                    if (m < 60) {
                        //% "%n minute(s)"
                        return qsTrId("orn-interval-m", m).arg(m)
                    } else {
                        var h = Math.floor(m / 60)
                        m -= (h * 60)
                        //% "%n hour(s)"
                        var res = qsTrId("orn-interval-h", h).arg(h)
                        if (m > 0) {
                            res += " " + qsTrId("orn-interval-m", m).arg(m)
                        }
                        return res
                    }
                }
                //% "Updates are checked only when the Storeman is running"
                description: qsTrId("orn-updates-check-interval-descr")

                onClicked: {
                    var m = Storeman.updateInterval / 60000
                    var h = Math.floor(m / 60)
                    m -= (h * 60)
                    var d = pageStack.push("Sailfish.Silica.TimePickerDialog", {
                        hour: h,
                        minute: m
                    })
                    d.accepted.connect(function() {
                        var res = (d.hour * 60 + d.minute) * 60000
                        if (res >= 600000) {
                            Storeman.updateInterval = res
                        } else {
                            //% "The interval must be at least 10 minutes"
                            notification.show(qsTrId("orn-updates-check-interval-invalid"),
                                              "image://theme/icon-s-high-importance")
                        }
                    })
                }
            }
        }
    }
}
