import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

BackgroundItem {
    property bool _expanded: false

    id: infoItem
    width: parent.width
    height: content.height
    clip: true
    highlighted: down && !highlightDelayTimer.running
    _showPress: highlighted
    propagateComposedEvents: true

    onPressedChanged: {
        if (pressed) {
            highlightDelayTimer.restart()
        }
    }

    onClicked: _expanded = !_expanded

    Behavior on height { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }

    Timer {
        // Idea is from the official store client
        id: highlightDelayTimer
        interval: 50
    }

    Column {
        id: content
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }

        Item {
            width: parent.width
            height: bodyLabel.height

            Label {
                id: bodyLabel
                width: parent.width
                maximumLineCount: _expanded ? 0 : 7
                text: app.body
                color: infoItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                linkColor: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }

            OpacityRampEffect {
                sourceItem: bodyLabel
                enabled: !_expanded
                direction: OpacityRamp.TopToBottom
            }
        }

        Item {
            id: spacer
            visible: _expanded
            height: Theme.paddingMedium
        }

        AppInfoLabel {
            visible: _expanded && app.installedVersion
            //% "Installed version"
            label: qsTrId("orn-version-installed")
            value: app.installedVersion
        }

        AppInfoLabel {
            visible: _expanded
            //% "Available version"
            label: qsTrId("orn-version-available")
            value: {
                if (app.repoStatus === OrnApplication.RepoEnabled) {
                    if (app.availableVersion) {
                        return app.availableVersion
                    } else {
                        //% "No versions available"
                        return qsTrId("orn-version-noavailable")
                    }
                } else {
                    //% "Enable the repository first"
                    return qsTrId("orn-version-repo-disabled")
                }
            }
        }

        AppInfoLabel {
            visible: _expanded && app.created
            //% "Last updated"
            label: qsTrId("orn-version-updated")
            //: Output format for the date labels. For details visit http://doc.qt.io/qt-5/qdate.html#toString
            //% "yyyy-dd-MM hh:mm"
            value: app.created.toLocaleString(locale, qsTrId("orn-dt-format"))
        }

        Image {
            anchors.right: parent.right
            source: "image://theme/icon-lock-more"
        }
    }
}
