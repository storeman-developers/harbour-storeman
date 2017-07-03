import QtQuick 2.0
import Sailfish.Silica 1.0

Column {
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: Theme.horizontalPageMargin
        rightMargin: Theme.horizontalPageMargin
    }
    spacing: Theme.paddingMedium

    IconLabel {
        width: parent.width
        icon: app.updateAvailable ? "image://theme/icon-s-update" :
                app.installedVersion ? "image://theme/icon-s-installed" : ""
        //% "Update available"
        text: app.updateAvailable ? qsTrId("orn-update-available") :
                //% "Installed"
                app.installedVersion ? qsTrId("orn-installed") : ""
    }

    Row {
        id: stats
        spacing: Theme.paddingSmall

        IconLabel {
            icon: "image://theme/icon-s-like"
            text: app.ratingCount.toLocaleString(locale, "f", 0)
        }

        RatingBox {
            width: implicitWidth + Theme.paddingLarge
        }

        IconLabel {
            icon: "image://theme/icon-s-cloud-download"
            text: app.downloadsCount.toLocaleString(locale, "f", 0)
        }
    }
}
