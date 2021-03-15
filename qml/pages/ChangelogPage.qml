import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    property string appName
    property string appIconSource
    property alias changelog: label.text

    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            FancyPageHeader {
                id: header
                title: qsTrId("orn-changelog")
                description: appName
                iconSource: appIconSource
            }

            Label {
                id: label
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.primaryColor
                linkColor: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }

        VerticalScrollDecorator { }
    }
}
