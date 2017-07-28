import QtQuick 2.0
import Sailfish.Silica 1.0

IconButton {
    property bool _bookmarked

    icon.source: (_bookmarked ? "image://theme/icon-m-favorite-selected?" :
                                "image://theme/icon-m-favorite?") +
                 (pressed ? Theme.highlightColor : Theme.primaryColor)

    onClicked: {
        var f = _bookmarked ? ornClient.removeBookmark : ornClient.addBookmark
        f(app.appId)
    }

    Component.onCompleted: _bookmarked = ornClient.hasBookmark(app.appId)

    Connections {
        target: ornClient
        onBookmarkChanged: {
            if (app.appId === appid) {
                _bookmarked = bookmarked
                notification.show(_bookmarked ?
                                      //% "The app was added to bookmarks"
                                      qsTrId("orn-bookmarks-added") :
                                      //% "The app was removed from bookmarks"
                                      qsTrId("orn-bookmarks-removed"))
            }
        }
    }
}
