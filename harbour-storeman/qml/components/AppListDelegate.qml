import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

ListItem {
    property bool returnToUser: false
    property alias showUser: userNameLabel.visible
    property int previousAppId: -1
    // To reduce calls of model data() method store item data in a property
    readonly property var app: appData

    contentHeight: Theme.itemSizeExtraLarge

    onClicked: previousAppId === app.appId ?
                   // Trying to open a page for the previous application, so just go back
                   pageStack.navigateBack() :
                   pageStack.push(Qt.resolvedUrl("../pages/ApplicationPage.qml"), {
                                  appId: app.appId,
                                  returnToUser: returnToUser
                              })

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
            source: app.iconSource ? app.iconSource : "qrc:/images/appicon.png"
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
                text: app.title
            }

            Label {
                id: categoryLabel
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: app.category
            }

            Row {

                RatingBox {
                    id: ratingBox
                }

                Label {
                    id: userNameLabel
                    anchors.verticalCenter: parent.verticalCenter
                    width: column.width - ratingBox.width
                    horizontalAlignment: Qt.AlignRight
                    truncationMode: TruncationMode.Fade
                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.highlightColor
                    text: app.userName + "\u2009\u2022\u2009" +
                          app.createDate.toLocaleDateString(_locale, Locale.NarrowFormat)
                }
            }
        }
    }

    BookmarkButton {
        id: star
        anchors {
            right: parent.right
            rightMargin: Theme.paddingSmall
            bottom: parent.bottom
            bottomMargin: userNameLabel.height + Theme.paddingSmall
        }
    }
}
