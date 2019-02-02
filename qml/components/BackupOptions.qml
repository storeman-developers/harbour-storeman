import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0


Column {
    readonly property string path: {
        var p = StandardPaths.documents + "/Storeman/" + textField.text
        if (p.lastIndexOf('.ini') === -1) {
            p += '.ini'
        }
        return p.trim()
    }
    readonly property bool _fileExists: path && Storeman.fileExists(path)
    readonly property bool acceptable: !textField.errorHighlight

    width: parent.width

    TextField {
        id: textField
        width: parent.width
        //% "A file name for backup"
        placeholderText: qsTrId("orn-backup-filenameph")
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        validator: RegExpValidator {
            regExp: /((?!\/).)+/
        }
        errorHighlight: !acceptableInput || _fileExists
        text: backupOptions.filename
        //% "File already exists"
        label: _fileExists ? qsTrId("orn-file-exists") : placeholderText

        onTextChanged: backupOptions.filename = text
    }

    SectionHeader {
        //% "What to backup"
        text: qsTrId("orn-backup-items")
    }

    TextSwitch {
        checked: backupOptions.repos
        text: qsTrId("orn-repositories")

        onCheckedChanged: backupOptions.repos = checked
    }

    TextSwitch {
        checked: backupOptions.installed
        //% "Installed applications"
        text: qsTrId("orn-backup-apps")

        onCheckedChanged: backupOptions.installed = checked
    }

    TextSwitch {
        checked: backupOptions.bookmarks
        text: qsTrId("orn-bookmarks")

        onCheckedChanged: backupOptions.bookmarks = checked
    }
}
