import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    id: dialog
    allowedOrientations: defaultAllowedOrientations
    canAccept: networkManager.online &&
               usernameField.text &&
               usernameField.acceptableInput &&
               passwordField.text

    onAccepted: OrnClient.login(usernameField.text, passwordField.text)

    DialogHeader {
        id: header
        //% "Log in"
        acceptText: qsTrId("orn-login-action")
    }

    SilicaFlickable {
        anchors {
            left: parent.left
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                height: implicitHeight + Theme.paddingLarge
                verticalAlignment: Qt.AlignVCenter
                //% "Log in to OpenRepos.net to comment applications and "
                //% "reply to others comments.<br /><br />"
                //% "Storeman does not store your password or send it to third-parties."
                text: qsTrId("orn-login-help")
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
            }

            // Spacer
            Item {
                width: 1
                height: Theme.paddingMedium
            }

            TextField {
                id: usernameField
                width: parent.width
                //% "Username"
                placeholderText: qsTrId("orn-username")
                //: A translated string should not be longer than the original
                //% "Only letters, numbers, and underscores"
                label: qsTrId("orn-acceptable-username")
                validator: RegExpValidator { regExp: /[a-zA-Z0-9_]*/ }
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase

                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.enabled: text && acceptableInput
                EnterKey.onClicked: passwordField.forceActiveFocus()

                Component.onCompleted: text = OrnClient.userName
            }

            PasswordField {
                id: passwordField
                width: parent.width

                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.enabled: dialog.canAccept
                EnterKey.onClicked: dialog.accept()
            }
        }

        VerticalScrollDecorator { }
    }
}
