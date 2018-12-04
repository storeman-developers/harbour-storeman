import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property bool returnToUser: false
    property alias showUser: userNameLabel.visible
    property int previousAppId: -1
    property int previousStep: 1

    property int paddingPortrait: Screen.width > 1280
                                  ? Theme.paddingLarge + Theme.iconSizeLauncher
                                  : Theme.paddingLarge

    property int paddingLandscape: Screen.width > 1280
                                   ? Theme.paddingLarge + Theme.iconSizeLauncher * 2
                                   : Theme.paddingLarge + Theme.iconSizeLauncher

    contentHeight: 1.5*Theme.paddingLarge + (appIcon.height > centerRect.height ? appIcon.height : centerRect.height)

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
        anchors.left: parent.left
        anchors.leftMargin: deviceOrientation === Orientation.Portrait
                            ? paddingPortrait : paddingLandscape
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.iconSizeLauncher
        height: Theme.iconSizeLauncher
        fillMode: Image.PreserveAspectFit
        source: model.iconSource ? model.iconSource : "image://theme/icon-launcher-default"
    }

    // Title, category and stars
    Rectangle {
        id: centerRect
        color: 'transparent'
        anchors.left: appIcon.right
        anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: deviceOrientation === Orientation.Portrait
                             ? paddingPortrait : paddingLandscape
        anchors.verticalCenter: parent.verticalCenter
        height: ratingStars.y + ratingStars.height

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
            width: parent.width - Theme.iconSizeLauncher
            anchors.top: titleLabel.bottom
            text: model.category
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }

        // Star rating
        RatingBox {
            id: ratingStars
            width: parent.width
            anchors.top: categoryLabel.bottom
            ratingCount: model.ratingCount
            rating: model.rating
        }

        // Installed / installing spinner
        Loader {
            id: statusLoader
            anchors.verticalCenter: ratingStars.verticalCenter
            anchors.right: parent.right
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
            anchors.verticalCenter: ratingStars.verticalCenter
            anchors.right: statusLoader.left
            font.pixelSize: Theme.fontSizeTiny
            color: Theme.highlightColor
            text: model.userName + "\u2009\u2022\u2009" +
                  model.createDate.toLocaleDateString(_locale, Locale.ShortFormat)
        }

        // Bookmark star on the right
        BookmarkButton {
            id: bookmarkStar
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: userNameLabel.top
            width: Theme.iconSizeLauncher
        }
    }
}
