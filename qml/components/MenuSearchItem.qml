import QtQuick 2.0
import Sailfish.Silica 1.0

MenuItem {
    visible: networkManager.connected
    text: qsTrId("orn-search")
    onClicked: pageStack.push(Qt.resolvedUrl("../pages/SearchPage.qml"))
}
