import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

IconButton {
    property bool _bookmarked

    icon.source: (_bookmarked ? "image://theme/icon-m-favorite-selected?" :
                                "image://theme/icon-m-favorite?") +
                 (pressed ? Theme.highlightColor : Theme.primaryColor)

    onClicked: {
        var f = _bookmarked ? OrnClient.removeBookmark : OrnClient.addBookmark
        f(app.appId)
        _bookmarked = !_bookmarked
    }

    Component.onCompleted: _bookmarked = OrnClient.hasBookmark(app.appId)
}
