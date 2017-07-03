import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Label {
        id: label
        anchors.centerIn: parent
        //% "Storeman"
        text: qsTrId("orn-storeman")
    }

//    CoverActionList {
//        id: coverAction

//        CoverAction {
//            iconSource: "image://theme/icon-cover-next"
//        }

//        CoverAction {
//            iconSource: "image://theme/icon-cover-pause"
//        }
//    }
}

