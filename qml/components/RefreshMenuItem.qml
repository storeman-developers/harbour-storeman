import QtQuick 2.0
import Sailfish.Silica 1.0

MenuItem {
    property var model

    visible: model
    enabled: networkManager.connected
    //% "Refresh"
    text: qsTrId("orn-refresh")
    onClicked: model.reset()
}
