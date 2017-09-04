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
            property: "section"
            delegate: SectionHeader {
                text: section
            }
        }

        delegate: AppListDelegate { }

        PullDownMenu {
            id: menu

            RefreshMenuItem {
                model: bookmarksModel
            }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !viewPlaceholder.text &&
                     bookmarksList.count === 0 &&
                     !menu.active
        }

        ViewPlaceholder {
            id: viewPlaceholder
            enabled: text
            //% "Your bookmarked applications will be shown here"
            text: bookmarksList.count ? "" : qsTrId("orn-no-bookmarks")
        }
    }
}
