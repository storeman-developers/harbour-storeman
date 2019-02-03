import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

IconButton {
    property int appId
    property var bookmarked: undefined
    property bool _updateState

    icon.source: (bookmarked ? "image://theme/icon-m-favorite-selected?" :
                               "image://theme/icon-m-favorite?") +
                 (pressed ? Theme.highlightColor : Theme.primaryColor)

    onClicked: {
        var f = bookmarked ? OrnClient.removeBookmark : OrnClient.addBookmark
        f(appId)
        if (_updateState) {
            bookmarked = !bookmarked
        }
    }

    Component.onCompleted: {
        _updateState = bookmarked === undefined
        if (_updateState) {
            bookmarked = OrnClient.hasBookmark(appId)
        }
    }
}
