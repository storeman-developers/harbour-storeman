import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

BackgroundItem {
    property bool _expanded: false
    readonly property var _sizeTmpls: [
        //% "%0 KB"
        QT_TRID_NOOP("orn-size-kb"),
        //% "%0 MB"
        QT_TRID_NOOP("orn-size-mb"),
        //% "%0 GB"
        QT_TRID_NOOP("orn-size-gb")
    ]

    function prettySize(size) {

        // Undefined
        if (!size) {
            return ""
        }

        // Bytes
        var s = size
        if (s < 1024) {
            //% "%n byte(s)"
            return qsTrId("orn-size-bytes", s).arg(Number(s).toLocaleString(_locale))
        }

        // KB, MB, GB
        var i = -1
        while (i < 3 && s >= 1024) {
            s /= 1024
            ++i
        }

        return qsTrId(_sizeTmpls[i]).arg(Number(s).toLocaleString(_locale, 'f', 1))
    }

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
                onLinkActivated: openLink(link)
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
            id: installedVersion
            visible: _expanded && value
            //% "Installed version"
            label: qsTrId("orn-version-installed")
            value: app.installedVersion
        }

        AppInfoLabel {
            //% "Installed size"
            label: qsTrId("orn-size-installed")
            visible: installedVersion.visible
            value: prettySize(app.installedVersionSize)
        }

        AppInfoLabel {
            id: availableVersion
            visible: _expanded && app.packageName
            //% "Available version"
            label: qsTrId("orn-version-available")
            value: {
                if (app.repoStatus === OrnPm.RepoEnabled) {
                    var version = app.availableVersion
                    return version ? version :
                                     //% "No versions available"
                                     qsTrId("orn-version-noavailable")
                } else {
                    //% "Enable the repository first"
                    return qsTrId("orn-version-repo-disabled")
                }
            }
        }

        AppInfoLabel {
            //% "Download / install size"
            label: qsTrId("orn-size-download-install")
            visible: _expanded && app.availableVersionIsNewer
            value: prettySize(app.availableVersionDownloadSize) + " / " +
                   prettySize(app.availableVersionInstallSize)
        }

        AppInfoLabel {
            visible: _expanded && app.created
            //% "Last updated"
            label: qsTrId("orn-version-updated")
            //: Output format for the date labels. For details visit http://doc.qt.io/qt-5/qdate.html#toString
            //% "yyyy-MM-dd hh:mm"
            value: app.created.toLocaleString(_locale, qsTrId("orn-dt-format"))
        }

        AppInfoLabel {
            id: globalVersion
            visible: _expanded && app.globalVersionIsNewer
            //% "Available in other repositories"
            label: qsTrId("orn-version-available-global")
            value: app.globalVersion
        }

        AppInfoLabel {
            label: qsTrId("orn-size-download-install")
            visible: globalVersion.visible
            value: prettySize(app.globalVersionDownloadSize) + " / " +
                   prettySize(app.globalVersionInstallSize)
        }

        Image {
            anchors.right: parent.right
            source: "image://theme/icon-lock-more"
        }
    }
}
