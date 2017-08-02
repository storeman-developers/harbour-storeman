import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property alias statusLabel: statusLabel

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: Theme.horizontalPageMargin
        rightMargin: Theme.horizontalPageMargin
    }
    height: Math.max(column.height, star.height)

    Column {
        id: column
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: star.right
            rightMargin: Theme.paddingMedium
        }
        spacing: Theme.paddingMedium

        IconLabel {
            id: statusLabel
            width: parent.width
        }

        Row {
            id: stats
            spacing: Theme.paddingSmall

            IconLabel {
                icon: "image://theme/icon-s-like"
                text: app.ratingCount.toLocaleString(locale, "f", 0)
            }

            RatingBox {
                width: implicitWidth + Theme.paddingLarge
            }

            IconLabel {
                icon: "image://theme/icon-s-cloud-download"
                text: app.downloadsCount.toLocaleString(locale, "f", 0)
            }
        }
    }

    BookmarkButton {
        id: star
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
    }
}
