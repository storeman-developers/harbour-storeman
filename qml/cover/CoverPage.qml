import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

CoverBackground {

    function _activatePage(name, callback) {
        var namelength = name.length
        const pageOnStack = pageStack.find(function(p) {
            return p.toString().substr(0, namelength) === name
        })
        if (pageOnStack) {
            if (callback) {
                callback(pageOnStack)
            }
            pageStack.pop(pageOnStack, PageStackAction.Immediate)
        } else {
            pageStack.push(Qt.resolvedUrl("../pages/%1.qml".arg(name)), {}, PageStackAction.Immediate)
        }
        __silica_applicationwindow_instance.activate()
    }

    Image {
        anchors.fill: parent
        source: Qt.resolvedUrl("./background.svg")
        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

    Label {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: Theme.paddingMedium
        }
        color: networkManager.connected
            ? Theme.highlightColor
            : Theme.highlightDimmerColor
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Theme.fontSizeLarge
        text: "Storeman"
    }

    Label {
        visible: OrnPm.updatesAvailable
        anchors.centerIn: parent
        width: parent.width - 2 * (Screen.sizeCategory > Screen.Medium
                                       ? Theme.paddingMedium
                                       : Theme.paddingLarge)
        color: Theme.primaryColor
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        //% "Updates available"
        text: qsTrId("orn-cover-updates-available")
    }

    CoverActionList {
        id: coverAction
        enabled: networkManager.connected

        CoverAction {
            iconSource: "image://theme/icon-cover-search"
            onTriggered: _activatePage("SearchPage", function(p) {
                p.reset()
            })
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-" + (OrnPm.updatesAvailable ? "next" : "sync")
            onTriggered: OrnPm.updatesAvailable
                ? _activatePage("InstalledAppsPage")
                : OrnPm.refreshRepos()
        }
    }
}

