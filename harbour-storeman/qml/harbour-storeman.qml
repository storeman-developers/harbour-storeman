import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import org.nemomobile.notifications 1.0
import "pages"

ApplicationWindow
{
    initialPage: Component { RecentAppsPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    NetworkManager {
        id: networkManager
    }

    Notification {

        function show(message) {
            replacesId = 0
            previewBody = message
            publish()
        }

        function showPopup(title, message) {
            replacesId = 0
            previewSummary = title
            previewBody = message
            publish()
        }

        id: notification
        expireTimeout: 3000
    }
}

