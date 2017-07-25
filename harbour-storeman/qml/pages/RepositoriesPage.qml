import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaListView {
        anchors.fill: parent

        header: PageHeader {
            //% "Repositories"
            title: qsTrId("orn-repositories")
        }

        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnRepoModel.SortRole
            sourceModel: OrnRepoModel {
                id: repoModel
                onRowsInserted: proxyModel.sort(Qt.AscendingOrder)
                onErrorRemoveRepo: notification.showPopup(
                                       qsTrId("orn-error"),
                                       //% "Could not remove the repository"
                                       qsTrId("orn-repo-remove-error"))
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
            id: repoItem
            menu: ContextMenu {

                MenuItem {
                    text: qsTrId("orn-refresh")
                    onClicked: repoModel.refreshRepo(repoId)
                }

                MenuItem {
                    text: repoEnabled ?
                              //% "Disable"
                              qsTrId("orn-disable") :
                              //% "Enable"
                              qsTrId("orn-enable")
                    onClicked: repoModel.enableRepo(repoId, !repoEnabled)
                }

                MenuItem {
                    //% "Remove"
                    text: qsTrId("orb-remove")
                    //% "Removing"
                    onClicked: Remorse.itemAction(repoItem, qsTrId("orn-removing"), function() {
                        repoModel.removeRepo(repoAuthor)
                    })
                }
            }

            onClicked: showMenu()

            Label {
                anchors.verticalCenter: parent.verticalCenter
                x: Theme.horizontalPageMargin
                width: parent.width - Theme.horizontalPageMargin * 2
                color: repoEnabled ? Theme.primaryColor : Theme.secondaryColor
                text: repoAuthor
            }
        }

        PullDownMenu {

            MenuItem {
                //% "Reload"
                text: qsTrId("orn-reload")
                onClicked: repoModel.reset()
            }

            MenuItem {
                visible: repoModel.hasDisabledRepos
                //% "Enable all"
                text: qsTrId("orn-enable-all")
                //% "Enabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-enabling-all"),
                                               function() { repoModel.enableRepos(true) })

            }

            MenuItem {
                visible: repoModel.hasEnabledRepos
                //% "Disable all"
                text: qsTrId("orn-disable-all")
                //% "Disabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-disabling-all"),
                                               function() { repoModel.enableRepos(false) })
            }
        }
    }
}
