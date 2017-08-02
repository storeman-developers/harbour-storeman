import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

PullDownMenu {
    id: pullMenu
    visible: networkManager.state === "online" &&
             (repoMenuItem.text   || installMenuItem.text ||
              app.updateAvailable || app.installedVersion)

    MenuItem {
        id: repoMenuItem
        visible: text
        enabled: !pullMenu.busy
        text: {
            if (!app.repoAlias) {
                return ""
            }
            switch (app.repoStatus) {
            case OrnZypp.RepoNotInstalled:
                //% "Add repository"
                return qsTrId("orn-repo-add")
            case OrnZypp.RepoDisabled:
                //% "Enable repository"
                return qsTrId("orn-repo-enable")
            default:
                return ""
            }
        }
        onClicked: {
            switch (app.repoStatus) {
            case OrnApplication.RepoNotInstalled:
                //% "Adding"
                Remorse.popupAction(page, qsTrId("orn-adding-repo"), function() {
                    ornZypp.addRepo(app.userName)
                })
                break
            case OrnApplication.RepoDisabled:
                ornZypp.modifyRepo(app.repoAlias, OrnZypp.EnableRepo)
                break
            default:
                break
            }
        }
    }

    MenuItem {
        id: installMenuItem
        visible: text
        enabled: !pullMenu.busy
        text: {
            if (app.installedVersion) {
                //% "Remove"
                return qsTrId("orn-remove")
            } else if (app.availableVersion) {
                //% "Install"
                return qsTrId("orn-install")
            } else {
                return ""
            }
        }
        onClicked: {
            if (app.installedVersion) {
                Remorse.popupAction(page, qsTrId("orn-removing"), function()
                {
                    page.state = "Removing"
                    pullMenu.busy = true
                    app.remove()
                })
            } else if (app.availableVersion) {
                page.state = "Installing"
                pullMenu.busy = true
                app.install()
            }
        }
    }

    MenuItem {
        id: updateMenuItem
        visible: app.updateAvailable
        enabled: !pullMenu.busy
        //% "Update"
        text: qsTrId("orn-update")
        onClicked: {
            page.state = "Updating"
            pullMenu.busy = true
            app.install()
        }
    }

    MenuItem {
        id: launchMenuItem
        visible: app.canBeLaunched
        enabled: !pullMenu.busy
        //% "Launch"
        text: qsTrId("orn-launch")
        onClicked: app.launch()
    }
}
