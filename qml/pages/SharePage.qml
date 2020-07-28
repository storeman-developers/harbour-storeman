import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.TransferEngine 1.0

SharePage {
    id: page

    property string link
    property string linkTitle

    //% "Share link"
    header: qsTrId("orn-share-link")
    mimeType: "text/x-url"
    showAddAccount: false
    content: {
        "type": "text/x-url",
        "status": page.link,
        "linkTitle": page.linkTitle
    }
}
