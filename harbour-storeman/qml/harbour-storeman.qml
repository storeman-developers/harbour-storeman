import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import Nemo.Notifications 1.0
import Nemo.DBus 2.0
import harbour.orn 1.0
import "pages"

ApplicationWindow
{
    property bool _showUpdatesNotification: true
    property string _processingLink
    readonly property var _locale: Qt.locale()
    property var _operations: OrnPm.initialised ? OrnPm.operations : null

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

    Connections {
        target: __quickWindow
        onClosing: authorisationWarning.close()
    }

    NetworkManager {
        readonly property bool online: state === "online"

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
        category: "x-storeman.updates"
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
    }

    Notification {
        id: errorNotification
        appIcon: "image://theme/icon-lock-warning"
        //% "An error occured"
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
        target: Storeman

        onUpdatesNotification: {
            if (show) {
                // Don't show notification if the app was openned from notification
                if (_showUpdatesNotification) {
                    updatesNotification.replacesId = replaceId
                    updatesNotification.publish()
                }
            } else {
                updatesNotification.replacesId = replaceId
                updatesNotification.close()
            }
            _showUpdatesNotification = true
        }
    }

    Connections {
        target: OrnPm

        onRepoModified: {
            switch (action) {
            case OrnPm.RemoveRepo:
                //% "The repository %0 was removed"
                notification.show(qsTrId("orn-repo-removed").arg(repoAlias))
                break
            case OrnPm.AddRepo:
                //% "The repository %0 was added"
                notification.show(qsTrId("orn-repo-added").arg(repoAlias))
                break
            case OrnPm.DisableRepo:
                //% "The repository %0 was disabled"
                notification.show(qsTrId("orn-repo-disabled").arg(repoAlias))
                break
            case OrnPm.EnableRepo:
                //% "The repository %0 was enabled"
                notification.show(qsTrId("orn-repo-enabled").arg(repoAlias))
                break
            default:
                break
            }
        }

        //% "Package %0 was successfully installed"
        onPackageInstalled: notification.show(qsTrId("orn-package-installed").arg(packageName))

        //% "Package %0 was successfully removed"
        onPackageRemoved: notification.show(qsTrId("orn-package-removed").arg(packageName))

        onError: {
            switch (code) {
            case OrnPm.ErrorPackageNotFound:
                //% "Couldn't find package"
                details = qsTrId("orn-error-packagenotfound")
                break
            case OrnPm.ErrorDepResolutionFailed:
                var match = details.match(/nothing provides (.*) needed by (.*)/)
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
