import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

Dialog {
    id: dialog
    allowedOrientations: defaultAllowedOrientations
    canAccept: networkManager.online &&
               usernameField.text &&
               usernameField.acceptableInput &&
               passwordField.acceptableInput

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
                //: A translated string should not be longer than the original
                //% "Username"
                placeholderText: qsTrId("orn-username")
                label: placeholderText
                validator: RegExpValidator {
                    regExp: new RegExp([
                        /([a-zA-Z0-9_]{1,}|(?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*/,
                        /|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*/,
                        /")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:/,
                        /(2(5[0-5]|[0-4][0-9])|1[0-9][0-9]|[1-9]?[0-9]))\.){3}(?:(2(5[0-5]|[0-4][0-9])|/,
                        /1[0-9][0-9]|[1-9]?[0-9])|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|/,
                        /\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\]))/
                    ].map(function(r) {return r.source}).join(''))
                }
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase

                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.enabled: text && acceptableInput
                EnterKey.onClicked: passwordField.forceActiveFocus()

                Component.onCompleted: text = OrnClient.userName
            }

            PasswordField {
                id: passwordField
                width: parent.width
                validator: RegExpValidator { regExp: /^.{1,}$/ }

                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.enabled: dialog.canAccept
                EnterKey.onClicked: dialog.accept()
            }
        }

        VerticalScrollDecorator { }
    }
}
