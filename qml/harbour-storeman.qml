import QtQuick 2.0
import Sailfish.Silica 1.0
import MeeGo.Connman 0.2
import Nemo.Notifications 1.0
import Nemo.DBus 2.0
import harbour.orn 1.0
import "pages"

ApplicationWindow
{
    property bool manualUnusedCheck
    // Qt.application.displayName (available since Qt 5.9) cannot be used, because SFOS 4.4.0 still deploys QT 5.6 and Storeman supports SFOS ≥ 3.1.0!
    // For a detailed discussion see https://github.com/storeman-developers/harbour-storeman/issues/356#issuecomment-1192249781
    readonly property string applicationDisplayName: "Storeman"
    readonly property string applicationIcon: Qt.application.name
    readonly property string dbusService: "harbour.storeman.service"
    readonly property string dbusPath: "/harbour/storeman/service"
    readonly property string dbusInterface: dbusService
    //% "Show details"
    readonly property string _showDetailsId: QT_TRID_NOOP("orn-show-details")
    property bool _showUpdatesNotification: true
    property string _processingLink
    readonly property var _locale: Qt.locale()
    property var _operations: OrnPm.initialised && OrnPm.operations
    property var _resolvedLinks: new Object
    readonly property real _appListDelegatePadding: {
        if (pageStack._currentOrientation === Orientation.Portrait) {
            return Screen.sizeCategory >= Screen.Large ?
                        Theme.horizontalPageMargin + Theme.iconSizeLauncher :
                        Theme.horizontalPageMargin
        } else {
            return Screen.sizeCategory >= Screen.Large ?
                        Theme.horizontalPageMargin + Theme.iconSizeLauncher * 2 :
                        Theme.horizontalPageMargin + Theme.iconSizeLauncher
        }
    }

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
                pageStack.push(Qt.resolvedUrl("pages/AppPage.qml"), {
                                   appId: appid,
                                   returnToUser: false
                               })
                return
            }
            _processingLink = link
            var req = new XMLHttpRequest()
            // Prepare an HTTP request to obtain the headers
            req.open("HEAD", link, true)
            req.onreadystatechange = function() {
                if (req.readyState === XMLHttpRequest.DONE) {
                    if (req.status === 200) {
                        // Check if the headers contain an ID link
                        match = /<\/node\/(\d*)>.*/.exec(req.getResponseHeader("link"))
                        if (match) {
                            appid = match[1]
                            // Load the application page
                            pageStack.push(Qt.resolvedUrl("pages/AppPage.qml"), {
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

    function _repoActionMessage(action) {
        switch (action) {
        case OrnPm.RemoveRepo:
            //% "Removed repository %0"
            return qsTrId("orn-repo-removed")
        case OrnPm.AddRepo:
            //% "Added repository %0"
            return qsTrId("orn-repo-added")
        case OrnPm.DisableRepo:
            //% "Disabled repository %0"
            return qsTrId("orn-repo-disabled")
        case OrnPm.EnableRepo:
            //% "Enabled repository %0"
            return qsTrId("orn-repo-enabled")
        }
    }

    function _updateAll() {
        console.log("Calling _updateAll()")
        var updates = OrnPm.updatablePackages()
        for (var i = 0; i < updates.length; ++i) {
            OrnPm.updatePackage(updates[i])
        }
    }

    function removeAuthorRepo(author) {
        OrnPm.modifyRepo("openrepos-" + author, OrnPm.RemoveRepo)
    }

    initialPage: Component { MainPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    Connections {
        target: __quickWindow
        onClosing: {
            pageStack.clear()
            authorisationWarning.close()
        }
    }

    NetworkManager {
        id: networkManager
    }

    DBusAdaptor {
        service: dbusService
        iface: dbusInterface
        path: dbusPath
        xml: '\
  <interface name="harbour.storeman.service">
    <method name="openPage">
      <arg name="page" type="s" direction="in">
        <doc:doc>
          <doc:summary>
            Name of the page to open
            (https://github.com/storeman-developers/harbour-storeman/tree/master/qml/pages)
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
    <method name="updateAll">
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

        function updateAll() {
            _updateAll()
        }

        function removeRepos(repos) {
            repos.forEach(removeAuthorRepo)
        }
    }

    Notification {

        function show(message, icn) {
            replacesId = 0
            previewSummary = ""
            previewBody = message
            icon = icn || ""
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
        appName: applicationDisplayName
        expireTimeout: 3000
    }

    Notification {
        id: authorisationWarning
        appName: applicationDisplayName
        appIcon: applicationIcon
        remoteActions: [ {
                name: "default",
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "openPage",
                arguments: [ "AuthorisationDialog", {} ]
            } ]
    }

    Notification {
        id: updatesNotification
        category: "x-storeman.updates"
        appName: applicationDisplayName
        appIcon: applicationIcon
        //% "Updates available"
        summary: qsTrId("orn-updates-available-summary")
        body: qsTrId(_showDetailsId)
        remoteActions: [ {
                name: "default",
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "openPage",
                arguments: [ "InstalledAppsPage", {} ]
            }, {
                name: "update-all",
                displayName: qsTrId("orn-update-all"),
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "updateAll"
            } ]
    }

    Notification {
        id: errorNotification
        appName: applicationDisplayName
        appIcon: applicationIcon
        //% "An error occurred"
        summary: qsTrId("orn-error")
        previewBody: qsTrId(_showDetailsId)
        remoteActions: [ {
                name: "default",
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "openPage",
                arguments: [ "ErrorPage", { message: body } ]
            } ]
    }

    Notification {
        property var repos

        id: unusedReposNotification
        appName: applicationDisplayName
        appIcon: applicationIcon
        //% "Unused repositories found"
        summary: qsTrId("orn-unused-repos-found")
        previewBody: qsTrId(_showDetailsId)
        remoteActions: [ {
                name: "default",
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "openPage",
                arguments: [ "UnusedReposDialog", { repos: repos } ]
            }, {
                name: "remove-all",
                //% "Remove all"
                displayName: qsTrId("orn-remove-all"),
                service: dbusService,
                path: dbusPath,
                iface: dbusInterface,
                method: "removeRepos",
                arguments: [ repos ]
            } ]
    }

    Connections {
        target: OrnClient

        onError: {
            var warnIcon = "image://theme/icon-lock-warning"
            switch (code) {
            case OrnClient.NetworkError:
                if (networkManager.connected) {
                    //% "Network error"
                    notification.show(qsTrId("orn-error-network"), warnIcon)
                }
                break
            case OrnClient.AuthorisationError:
                notification.showPopup(
                            //% "Login error"
                            qsTrId("orn-login-error-title"),
                            //% "Cannot log in to OpenRepos.net - check your credentials and network connection"
                            qsTrId("orn-login-error-message"),
                            warnIcon)
                break
            case OrnClient.CommentSendError:
                //% "Error sending comment"
                notification.show(qsTrId("orn-error-comment-sending"), warnIcon)
                break
            case OrnClient.CommentDeleteError:
                //% "Error deleting comment"
                notification.show(qsTrId("orn-error-comment-deletion"), warnIcon)
                break
            default:
                break
            }
        }

        onAuthorisedChanged: OrnClient.authorised
            //% "You are logged in to OpenRepos.net"
            ? notification.show(qsTrId("orn-loggedin-message"))
            //% "You are logged out from OpenRepos.net"
            : notification.show(qsTrId("orn-loggedout-message"))

        onDayToExpiry: {
            //% "Authorisation expires"
            authorisationWarning.summary = qsTrId("orn-authorisation-expires-summary")
            //% "Click to reauthorise"
            authorisationWarning.body = qsTrId("orn-reauthorise")
            authorisationWarning.publish()
        }

        onCookieIsValidChanged: if (!OrnClient.cookieIsValid) {
            //% "Authorisation expired"
            authorisationWarning.summary = qsTrId("orn-authorisation-expired-summary")
            authorisationWarning.body = qsTrId("orn-reauthorise")
            authorisationWarning.publish()
        }

        onBookmarkChanged: {
            notification.show(bookmarked
                //% "Added a bookmark for the app"
                ? qsTrId("orn-bookmarks-added")
                //% "Removed the bookmark for the app"
                : qsTrId("orn-bookmarks-removed"))
        }
    }

    Connections {
        target: Storeman

        onUpdatesNotification: {
            if (!show) {
                updatesNotification.replacesId = replaceId
                updatesNotification.close()
            } else if (_showUpdatesNotification) {
                // Do not show a notification if the app was opened from a notification
                updatesNotification.replacesId = replaceId
                updatesNotification.publish()
            }
            _showUpdatesNotification = true
        }
    }

    Connections {
        target: OrnPm

        onRepoModified: notification.show(_repoActionMessage(action).arg(alias))

        //% "Removed all repositories"
        onRemoveAllReposFinished: notification.show(qsTrId("orn-repo-allremoved"))

        //% "Installed Package %0"
        onPackageInstalled: notification.show(qsTrId("orn-package-installed").arg(packageName))

        //% "Updated Package %0"
        onPackageUpdated: notification.show(qsTrId("orn-package-updated").arg(packageName))

        onPackageRemoved: {
            //% "Removed package %0"
            notification.show(qsTrId("orn-package-removed").arg(packageName))
            if (Storeman.searchUnusedRepos) {
                OrnPm.getUnusedRepos()
            }
        }

        onUnusedRepos: {
            if (repos.length) {
                if (manualUnusedCheck) {
                    pageStack.push(Qt.resolvedUrl("pages/UnusedReposDialog.qml"), {
                        repos: repos
                    })
                } else {
                    unusedReposNotification.repos = repos
                    unusedReposNotification.publish()
                }
            } else if (manualUnusedCheck) {
                manualUnusedCheck = false
                //% "Found no unused repository"
                notification.show(qsTrId("orn-no-unused-repos"))
            }
        }

        onError: {
            switch (code) {
            case OrnPm.ErrorPackageNotFound:
                //% "Cannot find package"
                details = qsTrId("orn-error-packagenotfound")
                break
            case OrnPm.ErrorDepResolutionFailed:
                var match = details.match(/nothing provides (.*) needed by (.*)/)
                //: A template string for a dependency resolution error.  %1 is a dependency and %2 is a failed package.
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
