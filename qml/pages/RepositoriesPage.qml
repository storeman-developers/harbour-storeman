import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool _working: false
    property var _knownAuthors: ({})

    function getAuthorId(name) {
        if (_working) {
            return
        }
        name = name.replace("_", "")
        var user = _knownAuthors[name]
        if (user) {
            pageStack.push(Qt.resolvedUrl("UserAppsPage.qml"), {
                               userId: user.id,
                               userName: name,
                               userIcon: user.icon,
                           })
            return
        }
        _working = true
        var req = new XMLHttpRequest()
        req.onreadystatechange = function() {
            if (req.readyState === XMLHttpRequest.DONE) {
                if (req.status == 200) {
                    // req.responceXml is null - using ugly regex instead
                    var match = /<body class=".*page-user-(\d*)" >[\s\S]*<img typeof="foaf:Image" src="(.*)" alt/.exec(req.responseText)
                    if (match) {
                        var userId = match[1]
                        var userIcon = match[2]
                        // Don't show the placeholder icon
                        if (userIcon.indexOf("https://openrepos.net/misc/") !== -1) {
                            userIcon = ""
                        }
                        _knownAuthors[name] = {
                            id: userId,
                            idcon: userIcon,
                        }
                        pageStack.push(Qt.resolvedUrl("UserAppsPage.qml"), {
                                           userId: userId,
                                           userName: name,
                                           userIcon: userIcon,
                                       })
                        _working = false
                        return
                    }
                }
                pageStack.push(Qt.resolvedUrl("SearchPage.qml"),
                               { initialSearch: name })
                _working = false
            }
        }
        req.open("GET", "https://openrepos.net/users/" + name)
        req.send()
    }

    id: page
    allowedOrientations: Orientation.All

    SilicaListView {
        id: reposList
        anchors.fill: parent

        header: PageHeader {
            //% "Repositories"
            title: qsTrId("orn-repositories")
        }

        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnRepoModel.SortRole
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: OrnRepoModel {
                id: repoModel
                onRowsInserted: proxyModel.sort()
                onModelAboutToBeReset: _working = true
                onModelReset: {
                    _working = false
                    proxyModel.sort()
                }
            }
        }

        section {
            property: "repoEnabled"
            delegate: SectionHeader {
                text: section === "true" ?
                          //% "Enabled"
                          qsTrId("orn-enabled") :
                          //% "Disabled"
                          qsTrId("orn-disabled")
            }
        }

        delegate: ListItem {
            readonly property bool _enableMenu:
                networkManager.online && !itemInProgress(repoAlias)

            id: repoItem
            onClicked: getAuthorId(repoAuthor)
            menu: ContextMenu {

                MenuItem {
                    visible: repoEnabled
                    enabled: _enableMenu
                    //% "Refresh cache"
                    text: qsTrId("orn-refresh-cache")
                    onClicked: OrnPm.refreshRepo(repoAlias, true)
                }

                MenuItem {
                    text: repoEnabled ?
                              //% "Disable"
                              qsTrId("orn-disable") :
                              //% "Enable"
                              qsTrId("orn-enable")
                    enabled: _enableMenu
                    onClicked: OrnPm.modifyRepo(
                                   repoAlias, repoEnabled ? OrnPm.DisableRepo : OrnPm.EnableRepo)
                }

                MenuItem {
                    enabled: _enableMenu
                    //% "Remove"
                    text: qsTrId("orb-remove")
                    //% "Removing"
                    onClicked: Remorse.itemAction(repoItem, qsTrId("orn-removing"), function() {
                        OrnPm.modifyRepo(repoAlias, OrnPm.RemoveRepo)
                    })
                }
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                x: Theme.horizontalPageMargin
                width: parent.width - Theme.horizontalPageMargin * 2
                color: repoEnabled ? Theme.primaryColor : Theme.secondaryColor
                text: repoAuthor
            }
        }

        PullDownMenu {
            readonly property bool _enableMenu:
                networkManager.online && _operations && _operations.length === 0

            id: menu

            RefreshMenuItem {
                model: repoModel
            }

            MenuItem {
                visible: reposList.count
                enabled: menu._enableMenu
                text: qsTrId("orn-refresh-cache")
                onClicked: Storeman.resetUpdatesTimer()
            }

            MenuItem {
                visible: repoModel.hasDisabledRepos
                enabled: menu._enableMenu
                //% "Enable all"
                text: qsTrId("orn-enable-all")
                //% "Enabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-enabling-all"),
                                               function() {
                                                   _working = true
                                                   OrnPm.enableRepos(true)
                                               })

            }

            MenuItem {
                visible: repoModel.hasEnabledRepos
                enabled: menu._enableMenu
                //% "Disable all"
                text: qsTrId("orn-disable-all")
                //% "Disabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-disabling-all"),
                                               function() {
                                                   _working = true
                                                   OrnPm.enableRepos(false)
                                               })
            }

            MenuItem {
                visible: reposList.count
                enabled: menu._enableMenu
                //% "Remove all"
                text: qsTrId("orn-remove-all")
                //% "Removing all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-removing-all"),
                                               function() {
                                                   _working = true
                                                   OrnPm.removeAllRepos()
                                               })
            }

            MenuStatusLabel { }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: _working && !menu.active
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: reposList.count === 0 && !_working
            //% "No OpenRepos repositories have been added yet"
            text: qsTrId("orn-no-repos")
            //% "You can add a repository from an application page"
            hintText: qsTrId("orn-add-repo-hint")
        }
    }
}
