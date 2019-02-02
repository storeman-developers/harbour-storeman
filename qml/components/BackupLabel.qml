import QtQuick 2.0
import Sailfish.Silica 1.0


Label {
    color: Theme.highlightColor
    wrapMode: Text.WordWrap
    font.pixelSize: Theme.fontSizeSmall
    //% "<h2>Backup to a file</h2><br />"
    //% "<p>Backup allows you to save your current OpenRepos repositories, installed applications and bookmarks and "
    //% "restore them later (for example after factory reset). A backup is a local file that is saved to the<br />"
    //% "<i>~/Documents/Storeman</i> directory.</p><br />"
    //% "<p><b>Attention!</b> You should copy your backups manually to some safe place before performing a factory reset. "
    //% "It could be your SD card, external device, cloud storage or something else.</p>"
    text: qsTrId("orn-backup-hint")
}
