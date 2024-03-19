import QtQuick 2.0
import Sailfish.Silica 1.0
import QtDocGallery 5.0
import harbour.orn 1.0

Page {
    // Duplicate count property because items are not really deleted from the model
    property int _count: 0

    readonly property bool _ready: page.status === PageStatus.Active && (queryModel.status === DocumentGalleryModel.Idle || queryModel.status === DocumentGalleryModel.Finished)

    on_ReadyChanged: _ready && (_count = queryModel.count)

    id: page
    allowedOrientations: defaultAllowedOrientations

    DocumentGalleryModel {
        id: queryModel
        properties: ["fileName", "filePath"]
        sortProperties: ["+fileName"]
        rootType: DocumentGallery.File
        filter: GalleryEqualsFilter { property: "mimeType"; value: "application/x-rpm" }
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        model: _ready ? queryModel : null

        header: PageHeader {
            //% "Local RPM files"
            title: qsTrId("orn-local-rpms")
        }

        delegate: ListItem {
            id: delegateItem
            width: parent.width
            contentHeight: delegateColumn.height + Theme.paddingMedium * 2
            onClicked: openMenu()

            menu: ContextMenu {

                MenuItem {
                    text: qsTrId("orn-install")
                    onClicked: {
                        // Temp variable is required for cases when user navigates back
                        // while remorse action is still active (filePath belongs to another context)
                        var path = filePath
                        delegateItem.remorseAction(qsTrId("orn-installing"), function() {
                            OrnPm.installFile(path)
                        })
                    }
                }

                MenuItem {
                    //% "Delete"
                    text: qsTrId("orn-delete")
                    onClicked: {
                        var path = filePath
                        //% "Deleting"
                        delegateItem.remorseAction(qsTrId("orn-deleting"), function() {
                            if (Storeman.removeFile(path)) {
                                delegateItem.animateRemoval()
                                _count -= 1
                            } else {
                                //% "Failed to delete"
                                notification.showPopup(qsTrId("orn-deletion-error"),
                                                       path, "image://theme/icon-lock-warning")
                            }
                        })
                    }
                }
            }

            Column {
                readonly property var info: OrnPm.rpmQuery(filePath, "%{NAME} %{VERSION}\n%{DESCRIPTION}").split('\n')

                id: delegateColumn
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                    margins: Theme.horizontalPageMargin
                }

                Label {
                    width: parent.width
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: parent.info[0]
                }

                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: parent.info[1]
                }

                Label {
                    width: parent.width
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Fade
                    color: delegateItem.highlighted ? Theme.highlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    text: filePath
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTrId("orn-refresh")
                onClicked: queryModel.reload()
            }
        }

        VerticalScrollDecorator { }

        BusyIndicator {
            id: busyIndicator
            size: BusyIndicatorSize.Large
            anchors.centerIn: parent
            running: !_ready
        }

        ViewPlaceholder {
            enabled: _ready && _count === 0
            //% "No local RPM files were found"
            text: qsTrId("orn-no-local-rpms")
            hintText: qsTrId("orn-pull-refresh")
        }
    }
}
