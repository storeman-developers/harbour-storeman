import QtQuick 2.0
import Sailfish.Silica 1.0

MenuItem {
    property variant model

    visible: model && networkManager.state === "online"
    //% "Refresh"
    text: qsTrId("orn-refresh")
    onClicked: model.reset()
}
