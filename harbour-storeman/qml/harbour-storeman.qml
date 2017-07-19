import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import org.nemomobile.notifications 1.0
import harbour.orn 1.0
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

        function show(message, icon) {
            replacesId = 0
            previewSummary = ""
            previewBody = message
            appIcon = icon ? icon : "harbour-storeman"
            publish()
        }

        function showPopup(title, message, icon) {
            replacesId = 0
            previewSummary = title
            previewBody = message
            appIcon = icon ? icon : "harbour-storeman"
            publish()
        }

        id: notification
        expireTimeout: 3000
    }

    Notification {
        id: authorisationWarning
        replacesId: 0
        appIcon: "image://theme/icon-lock-warning"
        // TODO
        onClicked: console.log("Implement me!!!")
    }

    OrnClient {
        id: ornClient
        networkManager: dataAccessManager
        onAuthorisedChanged: authorised ?
                                 //% "You have successfully logged in to the OpenRepos.net"
                                 notification.show(qsTrId("orn-loggedin-message")) :
                                 //% "You have logged out from the OpenRepos.net"
                                 notification.show(qsTrId("orn-loggedout-message"))
        onAuthorisationError: notification.showPopup(
                                  //% "Login error"
                                  qsTrId("orn-login-error-title"),
                                  //% "Could not log in the OpenRepos.net - check your credentials and network connection"
                                  qsTrId("orn-login-error-message"),
                                  "image://theme/icon-lock-warning")
        onDayToExpiry: {
            //% "Authorisation expires"
            authorisationWarning.previewSummary = qsTrId("orn-authorisation-expires-summary")
            //% "Click to reauthorise"
            authorisationWarning.previewBody = qsTrId("orn-reauthorise")
            //% "The OpenRepos authorisation expires. Click to reauthorise."
            authorisationWarning.body = qsTrId("orn-authorisation-expires-body")
            authorisationWarning.publish()
        }
        onCookieIsValidChanged: if (!cookieIsValid) {
            //% "Authorisation expired"
            authorisationWarning.previewSummary = qsTrId("orn-authorisation-expired-summary")
            authorisationWarning.previewBody = qsTrId("orn-reauthorise")
            //% "The OpenRepos authorisation has expired. Click to reauthorise."
            authorisationWarning.body = qsTrId("orn-authorisation-expired-body")
            authorisationWarning.publish()
        }
    }
}

