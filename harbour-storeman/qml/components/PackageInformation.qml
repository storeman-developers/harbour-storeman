import QtQuick 2.0
import QtQuick.Layouts 1.1
import Sailfish.Silica 1.0

GridLayout {
    property alias statusLabel: statusLabel

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: Theme.horizontalPageMargin
        rightMargin: Theme.horizontalPageMargin
    }
    columns: 2
    rowSpacing: Theme.paddingMedium
    columnSpacing: Theme.paddingMedium

    IconLabel {
        id: statusLabel
        Layout.fillWidth: true
    }

    BookmarkButton {
        id: star
        Layout.rowSpan: 2
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

    Row {
        id: categoryPanel
        spacing: Theme.paddingMedium
        Layout.columnSpan: 2

        Label {
            text: qsTrId("orn-categories") + ':'
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.highlightColor
        }

        Repeater {
            model: app.categories

            Label {
                text: modelData.name
                font.pixelSize: Theme.fontSizeExtraSmall
                color: touchArea.pressed ? Theme.highlightColor : Theme.primaryColor

                MouseArea {
                    id: touchArea
                    anchors.fill: parent
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/CategoryPage.qml"), {
                                                  categoryId: modelData.id,
                                                  categoryName: modelData.name
                                              })
                }
            }
        }
    }
}
