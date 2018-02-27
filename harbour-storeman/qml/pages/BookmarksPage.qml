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
            sortRole: OrnBookmarksModel.TitleRole
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
            property: "appData.title"
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
