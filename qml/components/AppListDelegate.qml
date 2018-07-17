import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property bool returnToUser: false
    property alias showUser: userNameLabel.visible
    property int previousAppId: -1
    property int previousStep: 1

    contentHeight: Theme.itemSizeExtraLarge

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

    Row {
        id: row
        anchors.verticalCenter: parent.verticalCenter
        x: Theme.horizontalPageMargin
        width: parent.width - Theme.horizontalPageMargin * 2
        spacing: Theme.paddingMedium

        Image {
            id: appIcon
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.iconSizeLauncher
            height: Theme.iconSizeLauncher
            fillMode: Image.PreserveAspectFit
            source: model.iconSource ? model.iconSource : "qrc:/images/appicon.png"
        }

        Column {
            id: column
            anchors.verticalCenter: parent.verticalCenter
            width: row.width - appIcon.width - Theme.paddingMedium
            spacing: Theme.paddingSmall

            Label {
                id: titleLabel
                width: parent.width - star.width
                maximumLineCount: 2
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.WordWrap
                text: model.title
            }

            Label {
                id: categoryLabel
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: model.category
            }

            Row {
                spacing: Theme.paddingSmall

                RatingBox {
                    id: ratingBox
                    ratingCount: model.ratingCount
                    rating: model.rating
                }

                Label {
                    id: userNameLabel
                    anchors.verticalCenter: parent.verticalCenter
                    width: column.width - ratingBox.width - statusLoader.width - parent.spacing * 2
                    horizontalAlignment: Qt.AlignRight
                    truncationMode: TruncationMode.Fade
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.highlightColor
                    text: model.userName + "\u2009\u2022\u2009" +
                          model.createDate.toLocaleDateString(_locale, Locale.ShortFormat)
                }

                Loader {
                    readonly property var _packageStatus: packageStatus

                    id: statusLoader
                    anchors.verticalCenter: parent.verticalCenter
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
            }
        }
    }

    BookmarkButton {
        id: star
        anchors {
            right: parent.right
            rightMargin: Theme.paddingSmall
            topMargin: Theme.paddingSmall
        }
        appId: model.appId
    }
}
