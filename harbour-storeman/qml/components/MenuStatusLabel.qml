import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.orn 1.0

MenuLabel {
    property PullDownMenu __menu: parent.parent

    visible: text
    text: {
        if (!OrnPm.initialised) {
            __menu.busy = true
            //% "Initialising"
            return qsTrId("orn-pmstate-initialising")
        }

        switch (_operations.length) {
        case 0:
            __menu.busy = false
            return ""
        case 1:
            __menu.busy = true
            var op = _operations[0]
            switch (op.operation) {
            case OrnPm.AddingRepo:
                //% "Adding repo %0"
                return qsTrId("orn-pmstate-addingrepo").arg(op.item)
            case OrnPm.RemovingRepo:
                //% "Removing repo %0"
                return qsTrId("orn-pmstate-removingrepo").arg(op.item)
            case OrnPm.EnablingRepo:
                //% "Enabling repo %0"
                return qsTrId("orn-pmstate-enablingrepo").arg(op.item)
            case OrnPm.DisablingRepo:
                //% "Disabling repo %0"
                return qsTrId("orn-pmstate-disablingrepo").arg(op.item)
            case OrnPm.RefreshingRepo:
                //% "Refreshing %0"
                return qsTrId("orn-pmstate-refreshingrepo").arg(op.item)
            case OrnPm.RefreshingAllRepos:
                //% "Refreshing cache<br />(checking for updates)"
                return qsTrId("orn-pmstate-refreshingcache")
            case OrnPm.InstallingPackage:
                //% "Installing package %0"
                return qsTrId("orn-pmstate-installingpackage").arg(op.item)
            case OrnPm.RemovingPackage:
                //% "Removing package %0"
                return qsTrId("orn-pmstate-removingpackage").arg(op.item)
            case OrnPm.UpdatingPackage:
                //% "Updating package %0"
                return qsTrId("orn-pmstate-updatingpackage").arg(op.item)
            default:
                return ""
            }
        default:
            __menu.busy = true
            //: There are always more than 1 operations
            //% "%n operations are in progress"
            return qsTrId("orn-pmstate-multiple", _operations.length).arg(_operations.length)
        }
    }
}
