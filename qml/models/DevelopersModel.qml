import QtQuick 2.0

ListModel {

    ListElement {
        //% "Developers"
        role: qsTrId("orn-developers")
        participants: [
            ListElement {
                name: "Petr Tsymbarovich"
                link: "https://github.com/mentaljam"
            },
            ListElement {
                name: "Christoph"
                link: "https://github.com/inta"
            },
            ListElement {
                name: "Matti Viljanen"
                link: "https://github.com/direc85"
            }
        ]
    }

    ListElement {
        role: "OpenRepos"
        participants: [
            ListElement {
                name: "Basil Semuonov"
                link: "https://github.com/custodian"
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
