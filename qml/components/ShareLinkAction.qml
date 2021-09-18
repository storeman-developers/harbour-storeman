import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Share 1.0

ShareAction {
    function shareLink(url, title) {
        var res = {
            type:   mimeType,
            status: url
        }
        if (title) {
            res.linkTitle = title
        }

        resources = [res]
        trigger()
    }

    mimeType: "text/x-url"
}
