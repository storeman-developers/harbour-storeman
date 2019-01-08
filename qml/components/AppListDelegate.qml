import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property bool returnToUser: false
    property alias showUser: userNameLabel.visible
    property int previousAppId: -1
    property int previousStep: 1

    contentHeight: Math.max(appIcon.height, centerRect.height) + Theme.paddingLarge * 1.5

    onClicked: {
        var appId = model.appId
        if (previousAppId === appId) {
            // Trying to open a page for the previous application, so just go back
            var p = pageStack.previousPage()
            for (var i = 1; i < previousStep; ++i) {
                p = pageStack.previousPage(p)
            }
            pageStack.pop(p)
        } else {
            pageStack.push(Qt.resolvedUrl("../pages/ApplicationPage.qml"), {
                               appId: appId,
                               returnToUser: returnToUser
                           })
        }
    }

    // Application icon on the left
    Image {
        id: appIcon
        anchors {
            left: parent.left
            leftMargin: _appListDelegatePadding
            verticalCenter: parent.verticalCenter
        }
        width: Theme.iconSizeLauncher
        height: Theme.iconSizeLauncher
        fillMode: Image.PreserveAspectFit
        source: model.iconSource ? model.iconSource : "image://theme/icon-launcher-default"
    }

    // Title, category and stars
    Rectangle {
        id: centerRect
        anchors {
            left: appIcon.right
            leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: _appListDelegatePadding
            verticalCenter: parent.verticalCenter
        }
        height: ratingStars.y + ratingStars.height
        color: 'transparent'

        // Application title
        Label {
            id: titleLabel
            anchors.top: parent.top
            width: parent.width - Theme.iconSizeLauncher
            text: model.title
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.WordWrap
            maximumLineCount: 2
        }

        // Category
        Label {
            id: categoryLabel
            anchors.top: titleLabel.bottom
            width: parent.width - Theme.iconSizeLauncher
            text: model.category
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }

        // Star rating
        RatingBox {
            id: ratingStars
            anchors.top: categoryLabel.bottom
            width: parent.width
            ratingCount: model.ratingCount
            rating: model.rating
        }

        // Installed / installing spinner
        Loader {
            id: statusLoader
            anchors {
                verticalCenter: ratingStars.verticalCenter
                right: parent.right
            }
            width: BusyIndicatorSize.ExtraSmall
            height: BusyIndicatorSize.ExtraSmall
            readonly property var _packageStatus: packageStatus
            sourceComponent: _packageStatus < OrnPm.PackageInstalling ?
                                 iconComponent : busyComponent

            Component {
                id: busyComponent
                BusyIndicator {
                    size: BusyIndicatorSize.ExtraSmall
                    running: true
                }
            }

            Component {
                id: iconComponent
                Image {
                    source: _packageStatus === OrnPm.PackageInstalled ?
                                "image://theme/icon-s-installed" :
                                _packageStatus === OrnPm.PackageUpdateAvailable ?
                                    "image://theme/icon-s-update" : ""
                }
            }
        }

        // Author and date next to star rating
        Label {
            id: userNameLabel
            anchors {
                right: statusLoader.left
                verticalCenter: ratingStars.verticalCenter
            }
            font.pixelSize: Theme.fontSizeTiny
            color: Theme.highlightColor
            text: model.userName + "\u2009\u2022\u2009" +
                  model.createDate.toLocaleDateString(_locale, Locale.ShortFormat)
        }

        // Bookmark star on the right
        BookmarkButton {
            id: bookmarkStar
            anchors {
                top: parent.top
                right: parent.right
                bottom: userNameLabel.top
            }
            width: Theme.iconSizeLauncher
        }
    }
}
