import QtQuick 2.0

ListModel {

    ListElement {
        //% "Developers"
        role: qsTrId("orn-developers")
        participants: [
            ListElement {
                name: "Petr Tsymbarovich (mentaljam / osetr)"
                link: "https://github.com/mentaljam"
            },
            ListElement {
                name: "olf (Olf0)"
                link: "https://github.com/Olf0"
            },
            ListElement {
                name: "Matti Viljanen (direc85)"
                link: "https://github.com/direc85"
            },
            ListElement {
                name: "Björn Bidar (Thaodan)"
                link: "https://github.com/Thaodan"
            },
            ListElement {
                name: "citronalco"
                link: "https://github.com/citronalco"
            },
            ListElement {
                name: "Dmitry Gerasimov (dseight)"
                link: "https://github.com/dseight"
            },
            ListElement {
                name: "elros34"
                link: "https://github.com/elros34"
            },
            ListElement {
                name: "Christoph (inta)"
                link: "https://github.com/inta"
            },
            ListElement {
                name: "Miklós Márton (martonmiklos)"
                link: "https://github.com/martonmiklos"
            }
        ]
    }

    ListElement {
        role: "OpenRepos"
        participants: [
            ListElement {
                name: "Basil Semuonov (custodian / thecust)"
                link: "https://github.com/custodian"
            }
        ]
    }

    ListElement {
        //% "Application icon"
        role: qsTrId("orn-appicon")
        participants: [
            ListElement {
                name: "Laurent Chambon (Laurent_C)"
                link: "mailto:l.chambon@gmail.com"
            }
        ]
    }
}
