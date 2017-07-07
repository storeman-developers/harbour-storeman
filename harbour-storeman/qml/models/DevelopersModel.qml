import QtQuick 2.0

ListModel {

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
