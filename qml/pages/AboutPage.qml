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
                    source: "/usr/share/icons/hicolor/480x480/apps/harbour-storeman.png"
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
                //% "<p>OpenRepos client application for SailfishOS<br />&nbsp;</p>"
                //% "<p>Storeman is Free Software (FLOSS), distributed under the terms of the <a href='%1'>MIT&nbsp;license</a>.<br />&nbsp;</p>"
                //% "<p>Any issues (bug reports, feature suggestions, help requests etc.) shall be filed at GitHub (you may use the button below).</p>"
                text: qsTrId("orn-app-description-full").arg("https://github.com/storeman-developers/harbour-storeman/raw/master/LICENSE")
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ButtonLayout {
                width: parent.width

                Button {
                    text: "@GitHub"
                    onClicked: Qt.openUrlExternally("https://github.com/storeman-developers")
                }

                Button {
                    text: "@OpenRepos"
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("AppPage.qml"), {
                            appId: 11621
                        })
                    }
                }

                Button {
                    //% "Donation"
                    text: qsTrId("orn-donation")
                    onClicked: Qt.openUrlExternally("https://openrepos.net/donate")
                }

                Button {
                    text: qsTrId("orn-translations")
                    onClicked: pageStack.push(Qt.resolvedUrl("TranslationsPage.qml"))
                }

                Button {
                    text: qsTrId("orn-development")
                    onClicked: pageStack.push(Qt.resolvedUrl("DevelopmentPage.qml"))
                }                
            }
        }
    }
}
