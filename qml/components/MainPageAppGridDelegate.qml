import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    id: delegate
    width: cellWidth
    height: Theme.itemSizeSmall
    visible: model.index < gridColumns * gridRows

    onClicked: {
        if (!model.isValid) {
            return
        }
        pageStack.push(Qt.resolvedUrl("../pages/AppPage.qml"), {
                           appId: model.appId
                       })
    }

    Image {
        id: icon
        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium
        source: model.iconSource
    }

    Item {
        id: labelItem
        anchors {
            left: icon.right
            leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        height: authorLabel.y + authorLabel.height

        Label {
            id: titleLabel
            width: parent.width
            text: model.title
            color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
            font.pixelSize: Theme.fontSizeTiny
            truncationMode: TruncationMode.Fade
        }

        Label {
            id: authorLabel
            anchors.top: titleLabel.bottom
            width: parent.width
            text: model.userName
            color: Theme.secondaryHighlightColor
            font.pixelSize: Theme.fontSizeTiny
            truncationMode: TruncationMode.Fade
        }
    }
}
