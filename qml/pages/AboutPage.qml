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
                text: "Storeman " + Qt.application.version
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
                //% "Unofficial native OpenRepos.net client for Sailfish&nbsp;OS"
                text: qsTrId("orn-app-description") + "<br />" +
                //% "This is an open source software which is distributed under the terms of the<br /><a href='%0'>MIT License</a>"
                      qsTrId("orn-app-licensing").arg("https://github.com/mentaljam/harbour-storeman/blob/master/LICENSE")
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ButtonLayout {
                width: parent.width

                Button {
                    text: qsTrId("orn-development")
                    onClicked: pageStack.push(Qt.resolvedUrl("DevelopmentPage.qml"))
                }

                Button {
                    text: qsTrId("orn-translations")
                    onClicked: pageStack.push(Qt.resolvedUrl("TranslationsPage.qml"))
                }

                Button {
                    //% "Report an Issue"
                    text: qsTrId("orn-report")
                    onClicked: Qt.openUrlExternally("https://github.com/mentaljam/harbour-storeman/issues/new")
                }
            }
        }
    }
}
