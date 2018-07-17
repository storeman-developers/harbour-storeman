import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property alias message: messageLabel.text

    objectName: "ErrorPage"
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTrId("orn-error")
            }

            Label {
                id: messageLabel
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
            }
        }

        VerticalScrollDecorator { }
    }
}
