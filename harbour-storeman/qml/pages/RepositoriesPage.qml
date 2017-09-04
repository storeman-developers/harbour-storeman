import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    property bool _working: false

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
            id: repoItem
            onClicked: pageStack.push(Qt.resolvedUrl("SearchPage.qml"),
                                      { initialSearch: repoAuthor })
            menu: ContextMenu {

                MenuItem {
                    visible: repoEnabled
                    //% "Refresh cache"
                    text: qsTrId("orn-refresh-cache")
                    onClicked: OrnZypp.refreshRepo(repoAlias, true)
                }

                MenuItem {
                    text: repoEnabled ?
                              //% "Disable"
                              qsTrId("orn-disable") :
                              //% "Enable"
                              qsTrId("orn-enable")
                    onClicked: OrnZypp.modifyRepo(
                                   repoAlias, repoEnabled ? OrnZypp.DisableRepo : OrnZypp.EnableRepo)
                }

                MenuItem {
                    //% "Remove"
                    text: qsTrId("orb-remove")
                    //% "Removing"
                    onClicked: Remorse.itemAction(repoItem, qsTrId("orn-removing"), function() {
                        OrnZypp.modifyRepo(repoAlias, OrnZypp.RemoveRepo)
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
            id: menu

            RefreshMenuItem {
                model: repoModel
            }

            MenuItem {
                text: qsTrId("orn-refresh-cache")
                onClicked: OrnZypp.refreshRepos(true)
            }

            MenuItem {
                visible: repoModel.hasDisabledRepos
                //% "Enable all"
                text: qsTrId("orn-enable-all")
                //% "Enabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-enabling-all"),
                                               function() { OrnZypp.enableRepos(true) })

            }

            MenuItem {
                visible: repoModel.hasEnabledRepos
                //% "Disable all"
                text: qsTrId("orn-disable-all")
                //% "Disabling all"
                onClicked: Remorse.popupAction(page, qsTrId("orn-disabling-all"),
                                               function() { OrnZypp.enableRepos(false) })
            }
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
