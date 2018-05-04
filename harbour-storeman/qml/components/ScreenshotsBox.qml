import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    id: screenshotBox
    width: parent.width

    Repeater {
        model: Math.min(app.screenshots.length, 3)

        Image {
            width: screenshotBox.width / 3
            height: width
            fillMode: Image.PreserveAspectCrop
            source: app.screenshots[index].thumb

            BusyIndicator {
                anchors.centerIn: parent
                running: parent.status === Image.Loading
            }

            MouseArea {
                anchors.fill: parent
                onClicked: pageStack.push(
                               Qt.resolvedUrl("../pages/ScreenshotPage.qml"), {
                                   model: app.screenshots,
                                   currentIndex: model.index
                               }, PageStackAction.Immediate)
            }
        }
    }
}
