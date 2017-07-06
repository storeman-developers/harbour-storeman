import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {

    ListModel {
        id: participantsModel

        ListElement {
            //% "Developers"
            role: qsTrId("orn-developers")
            participants: [
                ListElement {
                    name: "Petr Tsymbarovich"
                    link: "https://github.com/mentaljam"
                }
            ]
        }

        ListElement {
            //% "Application Icon"
            role: qsTrId("orn-appicon")
            participants: [
                ListElement {
                    name: "Laurent_C"
                    link: "mailto:l.chambon@gmail.com"
                }
            ]
        }
    }

    SilicaListView {
        anchors.fill: parent
        model: participantsModel

        header: PageHeader {
            //% "Development
            title: qsTrId("orn-development")
        }

        delegate: Item {
            width: parent.width
            height: column.height + Theme.paddingLarge

            Column {
                id: column
                x: Theme.horizontalPageMargin
                width: parent.width - Theme.horizontalPageMargin * 2
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.paddingSmall

                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Qt.AlignHCenter
                    color: Theme.highlightColor
                    text: role
                }

                Column {
                    width: parent.width

                    Repeater {
                        model: participants
                        delegate: Label {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Qt.AlignHCenter
                            color: Theme.secondaryColor
                            linkColor: Theme.primaryColor
                            font.pixelSize: Theme.fontSizeSmall
                            text: link ? "<a href='%0'>%1</a>".arg(link).arg(name) : name
                            onLinkActivated: Qt.openUrlExternally(link)
                        }
                    }
                }
            }
        }

        PullDownMenu {
            MenuItem {
                //% "Source Code"
                text: qsTrId("orn-sources")
                onClicked: Qt.openUrlExternally("https://github.com/mentaljam/harbour-storeman")
            }
        }

        VerticalScrollDecorator { }
    }
}
