import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    width: parent.width
    height: Theme.itemSizeLarge
    onClicked: pageStack.push(Qt.resolvedUrl("../pages/AuthorisationDialog.qml"))

    Label {
        anchors {
            left: parent.left
            right: parent.right
            margins: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        color: parent.pressed ? Theme.highlightColor : Theme.primaryColor
        wrapMode: Text.WordWrap
        //% "Login to comment"
        text: qsTrId("orn-login2comment")
    }
}
