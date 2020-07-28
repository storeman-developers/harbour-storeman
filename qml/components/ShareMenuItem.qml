import QtQuick 2.0
import Sailfish.Silica 1.0

MenuItem {
    property string link
    property string linkTitle

    text: qsTrId("orn-share-link")

    onClicked: pageStack.push(Qt.resolvedUrl("../pages/SharePage.qml"), {
                                  link: link,
                                  linkTitle: linkTitle,
                              })
}
