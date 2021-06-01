import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    property alias repos: listView.model
    property var _reposToRemove: repos.reduce(function(res, author) {
        res[author] = true
        return res
    }, {})

    function _setRepo(author, remove) {
        if (remove) {
            _reposToRemove[author] = remove
        } else {
            delete _reposToRemove[author]
        }
    }

    id: dialog
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: console.log(_reposToRemove)

    onAccepted: Object.keys(_reposToRemove).forEach(removeAuthorRepo)

    SilicaListView {
        id: listView
        anchors.fill: parent

        header: Column {
            width: parent.width

            DialogHeader {
                id: header
                acceptText: qsTrId("orn-remove")
                //% "Unused repositories"
                title: qsTrId("orn-unused-repos")
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                //% "<p>There are no installed packages for the next repositories.</p>"
                //% "<p>Do you want to remove them now?</p>"
                text: qsTrId("orn-unused-repos-text")
            }
        }

        delegate: TextSwitch {
            checked: true
            text: modelData
            onCheckedChanged: _setRepo(modelData, checked)
        }
    }
}
