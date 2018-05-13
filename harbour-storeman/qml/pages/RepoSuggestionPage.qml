import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    property string author
    property bool enableOnly

    id: dialog
    allowedOrientations: defaultAllowedOrientations

    onAccepted: enableOnly ?
                    OrnPm.modifyRepo("openrepos-" + author, OrnPm.EnableRepo) :
                    OrnPm.addRepo(author)

    DialogHeader {
        id: header
        acceptText: qsTrId("orn-enable")
    }

    SilicaFlickable {
        id: flickable
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: label.height

        Label {
            id: label
            anchors {
                left: parent.left
                right: parent.right
                margins: Theme.horizontalPageMargin
            }
            color: Theme.highlightColor
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            //% "<h3>Enable the Storeman repository</h3>"
            //% "<p>To receive the latest versions of Storeman, "
            //% "you need to enable its OpenRepos repository like for any other app.</p>"
            //% "<p>Do you want to do it now?</p>"
            text: qsTrId("orn-repo-suggestion")
        }
    }
}
