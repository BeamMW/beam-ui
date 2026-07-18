import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: thisDialog
    property var onImport: function (text) {}
    property var vm:       null

    //% "Paste Slatepack"
    title: qsTrId("slatepack-import-title")
    //% "Import"
    okButtonText: qsTrId("slatepack-import-ok")
    okButtonEnable: input.text.trim().length > 0
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonText: qsTrId("general-close")
    cancelButtonVisible: true
    width: 580

    contentItem: ColumnLayout {
        spacing: 20
        SFText {
            Layout.fillWidth: true
            leftPadding: 20
            rightPadding: 20
            topPadding: 15
            font.pixelSize: 14
            color: Style.content_main
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            //% "Paste a Slatepack you received to continue the transaction."
            text: qsTrId("slatepack-import-message")
        }

        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            //% "Load from file"
            text: qsTrId("slatepack-load-file")
            onClicked: {
                var t = thisDialog.vm.openSlatepackFromFile();
                if (t && t.length) input.text = t;
            }
        }

        // Inset, scrollable box for the pasted Slatepack — mirrors SlatepackDialog.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            radius: 10
            color: Style.background_main
            border.width: 1
            border.color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1)

            ScrollView {
                anchors.fill: parent
                anchors.margins: 2
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                TextArea {
                    id: input
                    leftPadding: 20
                    rightPadding: 20
                    topPadding: 15
                    bottomPadding: 15
                    wrapMode: TextArea.Wrap
                    selectByMouse: true
                    font.pixelSize: 14
                    color: Style.content_main
                    placeholderTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                    //% "Paste here"
                    placeholderText: qsTrId("slatepack-import-placeholder")
                    background: Item {}
                }
            }

            DropArea {
                anchors.fill: parent
                onDropped: (drop) => {
                    if (drop.hasUrls && drop.urls.length && thisDialog.vm) {
                        var t = thisDialog.vm.readSlatepackFile(drop.urls[0]);
                        if (t.length) input.text = t;
                    }
                }
            }
        }
    }

    onAccepted: {
        thisDialog.onImport(input.text.trim());
    }
}
