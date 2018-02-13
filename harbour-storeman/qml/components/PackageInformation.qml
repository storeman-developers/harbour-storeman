import QtQuick 2.0
import QtQuick.Layouts 1.1
import Sailfish.Silica 1.0
import harbour.orn 1.0

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
        running: _packageStatus === OrnPm.PackageInstalling ||
                 _packageStatus === OrnPm.PackageRemoving   ||
                 _packageStatus === OrnPm.PackageUpdating
        text: {
            switch (_packageStatus) {
            case OrnPm.PackageNotInstalled:
            case OrnPm.PackageAvailable:
                //% "Not installed"
                return qsTrId("orn-not-installed")
            case OrnPm.PackageInstalled:
                //% "Installed"
                return qsTrId("orn-installed")
            case OrnPm.PackageUpdateAvailable:
                //% "Update available"
                return qsTrId("orn-update-available")
            case OrnPm.PackageInstalling:
                //% "Installing"
                return qsTrId("orn-installing")
            case OrnPm.PackageRemoving:
                return qsTrId("orn-removing")
            case OrnPm.PackageUpdating:
                //% "Updating"
                return qsTrId("orn-updating")
            default:
                //% "Unknown"
                return qsTrId("orn-unknown")
            }
        }
        icon: {
            switch (_packageStatus) {
            case OrnPm.PackageInstalled:
                return "image://theme/icon-s-installed"
            case OrnPm.PackageUpdateAvailable:
                return "image://theme/icon-s-update"
            default:
                return ""
            }
        }
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
            text: app.ratingCount.toLocaleString(_locale, "f", 0)
        }

        RatingBox {
            width: implicitWidth + Theme.paddingLarge
        }

        IconLabel {
            icon: "image://theme/icon-s-cloud-download"
            text: app.downloadsCount.toLocaleString(_locale, "f", 0)
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
