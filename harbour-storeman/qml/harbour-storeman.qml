import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import org.nemomobile.notifications 1.0
import org.nemomobile.dbus 2.0
import harbour.orn 1.0
import "pages"

ApplicationWindow
{
    property bool repoFetching: true
    property bool _showUpdatesNotification: true
    property string _processingLink

    function openLink(link) {
        if (link === _processingLink) {
            return
        }
        // Check if link looks like an OpenRepos application link
        if (/http[s]:\/\/openrepos\.net\/content\/[a-zA-Z\-_]*\/[a-zA-Z\-_]*/.exec(link)) {
            _processingLink = link
            var req = new XMLHttpRequest()
            // Prepare a http request to get headers
            req.open("GET", link, true)
            req.onreadystatechange = function() {
                if (req.readyState == 4) {
                    if (req.status == 200) {
                        // Check if headers contain an id link
                        var match = /<\/node\/(\d*)>.*/.exec(req.getResponseHeader("link"))
                        if (match) {
                            // Load the application page
                            pageStack.push(Qt.resolvedUrl("pages/ApplicationPage.qml"), {
                                               appId: match[1],
                                               returnToUser: false
                                           })
                            _processingLink = ""
                            return
                        }
                    }
                    _processingLink = ""
                    Qt.openUrlExternally(link)
                }
            }
            req.send(null)
        // Open other links externally
        } else {
            Qt.openUrlExternally(link)
        }
    }

    initialPage: Component { RecentAppsPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: OrnZypp.fetchRepos()

    Connections {
        target: __quickWindow
        onClosing: authorisationWarning.close()
    }

    NetworkManager {
        id: networkManager
    }

    DBusAdaptor {
        service: "harbour.storeman.service"
        iface: "harbour.storeman.service"
        path: "/harbour/storeman/service"
        xml: "  <interface name=\"harbour.storeman.service\">\n" +
             "    <method name=\"openPage\"/>\n" +
             "  </interface>\n"

        function openPage(page, arguments) {
            if (page === "pages/InstalledAppsPage.qml") {
                _showUpdatesNotification = false
            }
            __silica_applicationwindow_instance.activate()
            pageStack.push(Qt.resolvedUrl(page), arguments)
        }
    }

    Notification {

        function show(message, icn) {
            replacesId = 0
            previewSummary = ""
            previewBody = message
            icon = icn ? icn : ""
            publish()
        }

        function showPopup(title, message, icn) {
            replacesId = 0
            previewSummary = title
            previewBody = message
            icon = icn
            publish()
        }

        id: notification
        expireTimeout: 3000
    }

    Notification {
        id: authorisationWarning
        appIcon: "image://theme/icon-lock-warning"
        remoteActions: [ {
                name: "default",
                service: "harbour.storeman.service",
                path: "/harbour/storeman/service",
                iface: "harbour.storeman.service",
                method: "openPage",
                arguments: [ "pages/AuthorisationDialog.qml", {} ]
            } ]
    }

    Notification {
        id: updatesNotification
        appIcon: "image://theme/icon-lock-application-update"
        //% "Updates available"
        previewSummary: qsTrId("orn-updates-available-summary")
        //% "Click to view updates"
        previewBody: qsTrId("orn-updates-available-preview")
        //% "Applications updates are available. Click to view details."
        body: qsTrId("orn-updates-available-body")
        remoteActions: [ {
                name: "default",
                service: "harbour.storeman.service",
                path: "/harbour/storeman/service",
                iface: "harbour.storeman.service",
                method: "openPage",
                arguments: [ "pages/InstalledAppsPage.qml", {} ]
            } ]

        Component.onCompleted: {
            var uid = OrnClient.value("gui/update_notification_id")
            if (uid !== undefined) {
                replacesId = uid
            }
        }

        onClosed: OrnClient.setValue("gui/update_notification_id", undefined)
    }

    Notification {
        id: errorNotification
        appIcon: "image://theme/icon-lock-warning"
        previewSummary: qsTrId("orn-error")
        //% "Click to view details"
        previewBody: qsTrId("orn-view-details")
        remoteActions: [ {
                name: "default",
                service: "harbour.storeman.service",
                path: "/harbour/storeman/service",
                iface: "harbour.storeman.service",
                method: "openPage",
                arguments: [ "pages/ErrorPage.qml", { message: body } ]
            } ]
    }

    Connections {
        target: OrnClient

        onAuthorisedChanged: OrnClient.authorised ?
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

        onCookieIsValidChanged: if (!OrnClient.cookieIsValid) {
            //% "Authorisation expired"
            authorisationWarning.previewSummary = qsTrId("orn-authorisation-expired-summary")
            authorisationWarning.previewBody = qsTrId("orn-reauthorise")
            //% "The OpenRepos authorisation has expired. Click to reauthorise."
            authorisationWarning.body = qsTrId("orn-authorisation-expired-body")
            authorisationWarning.publish()
        }

        onBookmarkChanged: {
            notification.show(bookmarked ?
                                  //% "The app was added to bookmarks"
                                  qsTrId("orn-bookmarks-added") :
                                  //% "The app was removed from bookmarks"
                                  qsTrId("orn-bookmarks-removed"))
        }
    }

    Connections {
        target: OrnZypp

        onUpdatesAvailableChanged: {
            // Don't show notification if
            // the app was openned from notification
            if (OrnZypp.updatesAvailable) {
                if (_showUpdatesNotification) {
                    updatesNotification.publish()
                    OrnClient.setValue("gui/update_notification_id", updatesNotification.replacesId)
                }
            } else {
                updatesNotification.close()
            }
            _showUpdatesNotification = true
        }

        onBeginRepoFetching: {
            repoFetching = true
            //% "Reading the repositories data"
            notification.show(qsTrId("orn-reading-repos-begin"),
                              "image://theme/icon-s-high-importance")
        }

        onEndRepoFetching: {
            repoFetching = false
            //% "Finished reading the repositories data"
            notification.show(qsTrId("orn-reading-repos-end"),
                              "image://theme/icon-s-installed")
        }

        onPkError: {
            switch (error) {
            case OrnZypp.ErrorDepResolutionFailed:
                var match = details.match(/nothing provides (.*) needed by (.*)/)
                console.log(match)
                //: A template string for a dependecy resolution error. %1 is a dependency and %2 is a failed package.
                //% "Nothing provides %1 needed by %2"
                details = qsTrId("orn-error-depresolution").arg(match[1]).arg(match[2])
                break
            default:
                break
            }

            errorNotification.replacesId = 0
            errorNotification.body = details
            errorNotification.publish()
        }
    }
}
