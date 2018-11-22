import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    CoverPlaceholder {
        icon.source: "/usr/share/icons/hicolor/86x86/apps/harbour-storeman.png"
        text: "Storeman"
    }

    CoverActionList {
        id: coverAction
        enabled: networkManager.online

        CoverAction {
            iconSource: "image://theme/icon-cover-search"
            onTriggered: {
                const searchOnPageStack = pageStack.find(function(page) {
                    return page.toString().substr(0, 10) === "SearchPage"
                })
                if (searchOnPageStack) {
                    searchOnPageStack._reset()
                    pageStack.pop(searchOnPageStack, PageStackAction.Immediate)
                } else {
                    pageStack.push(Qt.resolvedUrl("../pages/SearchPage.qml"), {}, PageStackAction.Immediate)
                }
                __silica_applicationwindow_instance.activate()
            }
        }

    }
}

