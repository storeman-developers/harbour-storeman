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
    property var _resolvedLinks: new Object

    function itemInProgress(item) {
        if (!_operations) {
            return true
        }
        for (var i in _operations) {
            if (_operations[i].item === item) {
                return true
            }
        }
        return false
    }

    function openLink(link) {
        if (link === _processingLink) {
            return
        }
        // Check if link looks like an OpenRepos application link
        var match = /http[s]:\/\/openrepos\.net\/content\/([a-zA-Z\-_]*\/[a-zA-Z\-_]*)/.exec(link)
        if (match) {
            var path = match[1]
            var appid = _resolvedLinks[path]
            if (appid) {
                pageStack.push(Qt.resolvedUrl("pages/ApplicationPage.qml"), {
                                   appId: appid,
                                   returnToUser: false
                               })
                return
            }
            _processingLink = link
            var req = new XMLHttpRequest()
            // Prepare a http request to get headers
            req.open("HEAD", link, true)
            req.onreadystatechange = function() {
                if (req.readyState === XMLHttpRequest.DONE) {
                    if (req.status == 200) {
                        // Check if headers contain an id link
                        match = /<\/node\/(\d*)>.*/.exec(req.getResponseHeader("link"))
                        if (match) {
                            appid = match[1]
                            // Load the application page
                            pageStack.push(Qt.resolvedUrl("pages/ApplicationPage.qml"), {
                                               appId: appid,
                                               returnToUser: false
                                           })
                            _resolvedLinks[path] = appid
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
        xml: '\
  <interface name="harbour.storeman.service">
    <method name="openPage">
      <arg name="page" type="s" direction="in">
        <doc:doc>
          <doc:summary>
            Name of the page to open
            (https://github.com/mentaljam/harbour-storeman/tree/master/qml/pages)
          </doc:summary>
        </doc:doc>
      </arg>
      <arg name="arguments" type="a{sv}" direction="in">
        <doc:doc>
          <doc:summary>
            Arguments to pass to the page
          </doc:summary>
        </doc:doc>
      </arg>
    </method>
  </interface>'

        function openPage(page, arguments) {
            if (page === "InstalledAppsPage") {
                _showUpdatesNotification = false
            }
            __silica_applicationwindow_instance.activate()
            if (page === "ErrorPage" || page !== pageStack.currentPage.objectName) {
                pageStack.push(Qt.resolvedUrl("pages/%1.qml".arg(page)), arguments)
            }
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
                arguments: [ "AuthorisationDialog", {} ]
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
                arguments: [ "InstalledAppsPage", {} ]
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
                arguments: [ "ErrorPage", { message: body } ]
            } ]
    }

    Connections {
        target: OrnClient

        onError: {
            switch (code) {
            case OrnClient.NetworkError:
                //% "Network error"
                notification.show(qsTrId("orn-error-network"), "image://theme/icon-lock-warning")
                break
            case OrnClient.AuthorisationError:
                notification.showPopup(
                            //% "Login error"
                            qsTrId("orn-login-error-title"),
                            //% "Could not log in the OpenRepos.net - check your credentials and network connection"
                            qsTrId("orn-login-error-message"),
                            "image://theme/icon-lock-warning")
                break
            case OrnClient.CommentSendError:
                //% "Error sending comment"
                notification.show(qsTrId("orn-error-comment-sending"), "image://theme/icon-lock-warning")
                break
            case OrnClient.CommentDeleteError:
                //% "Error deleting comment"
                notification.show(qsTrId("orn-error-comment-deletion"), "image://theme/icon-lock-warning")
                break
            default:
                break
            }
        }

        onAuthorisedChanged: OrnClient.authorised ?
                                 //% "You have successfully logged in to the OpenRepos.net"
                                 notification.show(qsTrId("orn-loggedin-message")) :
                                 //% "You have logged out from the OpenRepos.net"
                                 notification.show(qsTrId("orn-loggedout-message"))

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

        onRepoSuggestion: pageStack.push(
                              Qt.resolvedUrl("pages/RepoSuggestionPage.qml"), {
                                  author: author,
                                  enableOnly: enableOnly
                              })
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

        onRemoveAllReposFinished: {
            //% "All repositories were removed"
            notification.show(qsTrId("orn-repo-allremoved"))
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
