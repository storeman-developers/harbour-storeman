import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Item {
    property alias statusLabel: statusLabel
    property alias likeButton: likeButton
    property alias starButton: starButton

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: Theme.horizontalPageMargin
        rightMargin: Theme.horizontalPageMargin
    }
    height: statusLabel.height + stats.height + categoryPanel.height + Theme.paddingLarge

    IconButton {
        id: likeButton
        anchors {
            right: starButton.left
            verticalCenter: starButton.verticalCenter
        }
        icon.source: (app.userVote > 0 ? "image://theme/icon-m-like?" : "image://theme/icon-m-outline-like?") +
                     (pressed ? Theme.highlightColor : Theme.primaryColor)
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/VotingPage.qml"), {
                                      appId: app.appId,
                                      userVote: app.userVote
                                  }, PageStackAction.Immediate)
    }

    BookmarkButton {
        id: starButton
        anchors.right: parent.right
        appId: app.appId
    }

    IconLabel {
        id: statusLabel
        anchors {
            left: parent.left
            right: likeButton.left
            rightMargin: Theme.paddingMedium
        }
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

    Row {
        id: stats
        anchors {
            top: statusLabel.bottom
            topMargin: Theme.paddingMedium
            left: parent.left
            right: likeButton.left
            rightMargin: Theme.paddingMedium
        }
        spacing: Theme.paddingMedium

        IconLabel {
            icon: "image://theme/icon-s-like"
            anchors.verticalCenter: parent.verticalCenter
            text: app.ratingCount.toLocaleString(_locale, "f", 0)
        }

        RatingBox {
            id: ratingBox
            anchors.verticalCenter: parent.verticalCenter
            ratingCount: app.ratingCount
            rating: app.rating
        }

        IconLabel {
            icon: "image://theme/icon-s-cloud-download"
            anchors.verticalCenter: parent.verticalCenter
            text: app.downloadsCount.toLocaleString(_locale, "f", 0)
        }
    }

    Row {
        id: categoryPanel
        anchors {
            top: stats.bottom
            topMargin: Theme.paddingMedium
            left: parent.left
            right: parent.right
        }
        spacing: Theme.paddingMedium
        clip: true

        Label {
            id: categoryLabel
            text: qsTrId("orn-categories") + ':'
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.highlightColor
        }

        Repeater {
            id: categoryRepeater
            model: app.categories

            Label {
                text: modelData.name
                font.pixelSize: Theme.fontSizeExtraSmall
                color: touchArea.pressed ? Theme.highlightColor : Theme.primaryColor
                truncationMode: TruncationMode.Fade

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

    OpacityRampEffect {
        sourceItem: categoryPanel
        enabled: categoryPanel.childrenRect.width > categoryPanel.width
        direction: OpacityRamp.LeftToRight
        offset: 0.75
    }
}
