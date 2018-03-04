import QtQuick 2.0
import Sailfish.Silica 1.0


ListItem {
    id: delegate
    contentHeight: label.height + Theme.paddingMedium * 2

    onClicked: pageStack.push(Qt.resolvedUrl("../pages/TagAppsPage.qml"),
                              {
                                  tagId: model.tagId,
                                  tagName: model.name,
                                  previousAppId: page.previousAppId
                              })

    Label {
        id: label
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: Theme.horizontalPageMargin
            rightMargin: Theme.horizontalPageMargin
        }
        wrapMode: Text.WordWrap
        text: model.name
        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
    }
}
