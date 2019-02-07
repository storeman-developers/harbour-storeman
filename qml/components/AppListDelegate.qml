import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property bool returnToUser: false
    property alias showUser: userNameLabel.visible
    property int previousAppId: -1
    property int previousStep: 1

    id: appListDelegate
    contentHeight: Math.max(appIcon.height, appInfo.height) + Theme.paddingLarge * 1.5

    onClicked: {
        if (!model.isValid) {
            return
        }
        var appId = model.appId
        if (previousAppId === appId) {
            // Trying to open a page for the previous application, so just go back
            var p = pageStack.previousPage()
            for (var i = 1; i < previousStep; ++i) {
                p = pageStack.previousPage(p)
            }
            pageStack.pop(p)
        } else {
            pageStack.push(Qt.resolvedUrl("../pages/AppPage.qml"), {
                               appId: appId,
                               returnToUser: returnToUser
                           })
        }
    }

    ListView.onRemove: RemoveAnimation {
        target: appListDelegate
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
        source: model.iconSource
    }

    // Title, category, rating and bookmark button
    Item {
        id: appInfo
        anchors {
            left: appIcon.right
            leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: _appListDelegatePadding
            verticalCenter: parent.verticalCenter
        }
        height: ratingBox.y + ratingBox.height

        // Application title
        Label {
            id: titleLabel
            width: parent.width - bookmarkStar.width - Theme.paddingSmall
            //% "Invalid package ID %1"
            text: model.title || qsTrId("orn-bad-appid").arg(model.appId)
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            truncationMode: TruncationMode.Elide
        }

        // Category
        Label {
            id: categoryLabel
            anchors.top: titleLabel.bottom
            width: titleLabel.width
            text: model.category
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }

        // Current rating
        RatingBox {
            id: ratingBox
            y: Math.max(categoryLabel.y + categoryLabel.height,
                        bookmarkStar.y + bookmarkStar.height) +
               Theme.paddingSmall
            ratingCount: model.ratingCount
            rating: model.rating
        }

        // Author and date next to star rating
        Label {
            id: userNameLabel
            anchors {
                left: ratingBox.right
                leftMargin: Theme.paddingSmall
                right: statusLoader.left
                rightMargin: Theme.paddingSmall
                verticalCenter: ratingBox.verticalCenter
            }
            font.pixelSize: Theme.fontSizeTiny
            horizontalAlignment: Qt.AlignRight
            truncationMode: TruncationMode.Fade
            color: Theme.highlightColor
            text: {
                var res = model.userName
                var createDate = model.createDate
                if (!isNaN(createDate)) {
                    res += "\u2009\u2022\u2009"
                    res += createDate.toLocaleDateString(_locale, Locale.ShortFormat)
                }
                return res
            }
        }

        // Installed / installing spinner
        Loader {
            id: statusLoader
            anchors {
                verticalCenter: ratingBox.verticalCenter
                right: parent.right
            }
            sourceComponent: packageStatus < OrnPm.PackageInstalling ?
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
                    source: packageStatus === OrnPm.PackageInstalled ?
                                "image://theme/icon-s-installed" :
                                packageStatus === OrnPm.PackageUpdateAvailable ?
                                    "image://theme/icon-s-update" : ""
                }
            }
        }

        // Bookmark star on the right
        BookmarkButton {
            id: bookmarkStar
            anchors.right: parent.right
            appId: model.appId
            bookmarked: model.isBookmarked
        }
    }
}
