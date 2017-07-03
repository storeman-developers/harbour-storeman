import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

PullDownMenu {
    id: pullMenu
    enabled: networkManager.state === "online"

    onBusyChanged: {
        var length = children.length
        for (var i = 0; i < length; ++i) {
            children[i].enabled = !busy
        }
    }

    MenuItem {
        visible: text
        text: {
            switch (app.repoStatus) {
            case OrnApplication.RepoNotInstalled:
                //% "Add repository"
                return qsTrId("orn-repo-add")
            case OrnApplication.RepoDisabled:
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
                    app.enableRepo()
                })
                break
            case OrnApplication.RepoDisabled:
                app.enableRepo()
                break
            default:
                break
            }
        }
    }

    MenuItem {
        visible: text
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
                    pullMenu.busy = true
                    app.remove()
                })
            } else if (app.availableVersion) {
                pullMenu.busy = true
                app.install()
            }
        }
        onTextChanged: {
            pullMenu.busy = false
        }
    }

    MenuItem {
        visible: app.updateAvailable
        //% "Update"
        text: qsTrId("orn-update")
        onClicked: {
            pullMenu.busy = true
            app.install()
        }
    }

    MenuItem {
        visible: app.installedVersion
        //% "Launch"
        text: qsTrId("orn-launch")
        onClicked: app.launch()
    }
}
