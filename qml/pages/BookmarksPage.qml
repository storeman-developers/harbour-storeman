import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0
import "../components"

Page {
    id: page
    allowedOrientations: defaultAllowedOrientations

    SilicaListView {
        id: bookmarksList
        anchors.fill: parent
        model: OrnProxyModel {
            id: proxyModel
            sortRole: OrnBookmarksModel.SortRole
            sortCaseSensitivity: Qt.CaseInsensitive
            sourceModel: OrnBookmarksModel {
                id: bookmarksModel
                onRowsInserted: proxyModel.sort(Qt.AscendingOrder)
            }
        }

        header: PageHeader {
            //% "Bookmarks"
            title: qsTrId("orn-bookmarks")
        }

        section {
            property: "title"
            criteria: ViewSection.FirstCharacter
            delegate: SectionHeader {
                text: section.toUpperCase()
            }
        }

        delegate: AppListDelegate { }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: bookmarksModel
            }

            MenuStatusLabel { }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: bookmarksModel.fetching && !menu.active
        }

        ViewPlaceholder {
            enabled: !bookmarksModel.fetching && bookmarksList.count === 0
            //% "Your bookmarked applications will be shown here"
            text: qsTrId("orn-no-bookmarks")
        }
    }
}
