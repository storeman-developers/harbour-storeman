import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingMedium

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                id: header
                //% "About Storeman"
                title: qsTrId("orn-about")
            }

            Item {
                height: icon.height + Theme.paddingMedium
                width: parent.width

                Image {
                    id: icon
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "/usr/share/icons/hicolor/172x172/apps/harbour-storeman.png"
                }
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.highlightColor
                text: applicationDisplayName + " " + Qt.application.version
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: Theme.horizontalPageMargin
                    rightMargin: Theme.horizontalPageMargin
                }
                height: implicitHeight + Theme.paddingMedium
                color: Theme.highlightColor
                linkColor: Theme.primaryColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                //% "<p>Native OpenRepos.net client for Sailfish&nbsp;OS</p>"
                //% "<p>This is an open source software which is distributed under the terms of the <a href='%1'>MIT&nbsp;License</a></p>"
                //% "<p>You can report an issue on OpenRepos.net or GitHub (use the buttons below)</p>"
                text: qsTrId("orn-app-description").arg("https://github.com/mentaljam/harbour-storeman/blob/master/LICENSE")
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ButtonLayout {
                width: parent.width

                Button {
                    text: "OpenRepos.net"
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("AppPage.qml"), {
                            appId: 10056
                        })
                    }
                }

                Button {
                    text: qsTrId("orn-development")
                    onClicked: pageStack.push(Qt.resolvedUrl("DevelopmentPage.qml"))
                }

                Button {
                    text: qsTrId("orn-translations")
                    onClicked: pageStack.push(Qt.resolvedUrl("TranslationsPage.qml"))
                }

                Button {
                    //% "Donate with %1"
                    text: qsTrId("orn-donate-with").arg("PayPal")
                    onClicked: Qt.openUrlExternally("https://www.paypal.me/osetr")
                }

                Button {
                    text: qsTrId("orn-donate-with").arg("Tinkoff Bank")
                    onClicked: Qt.openUrlExternally("https://www.tinkoff.ru/sl/AHE8JmExARH")
                }
            }
        }
    }
}
