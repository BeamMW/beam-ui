import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: thisDialog
    property string armored: ""
    property string txId:    ""
    property var    vm:      null

    //% "Slatepack to send"
    title: qsTrId("slatepack-send-title")
    okButtonText: qsTrId("general-copy")
    okButtonIconSource: "qrc:/assets/icon-copy-blue.svg"
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonText: qsTrId("general-close")
    cancelButtonVisible: true
    width: 580

    contentItem: ColumnLayout {
        spacing: 20

        // Inset, scrollable "code block" holding the full armored Slatepack.
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(320, slatepackValue.implicitHeight + 4)
            radius: 10
            color: Style.background_main
            border.width: 1
            border.color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1)

            ScrollView {
                id: slatepackScroll
                anchors.fill: parent
                anchors.margins: 2
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                SFLabel {
                    id: slatepackValue
                    // Fixed width (dialog is fixed-width) keeps implicitHeight stable and
                    // avoids a width<->height binding loop with the adaptive box height.
                    width: thisDialog.width - 90
                    leftPadding: 20
                    rightPadding: 20
                    topPadding: 15
                    bottomPadding: 15
                    font.pixelSize: 14
                    font.styleName: "DemiBold"; font.weight: Font.DemiBold
                    color: Style.content_main
                    wrapMode: Text.Wrap
                    text: thisDialog.armored
                    copyMenuEnabled: true
                    onCopyText: BeamGlobals.copyToClipboard(text)
                }
            }
        }

        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            //% "Save to file"
            text:      qsTrId("slatepack-save-file")
            onClicked: thisDialog.vm.saveSlatepackToFile(thisDialog.txId, thisDialog.armored)
        }

        SFText {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 30
            font.pixelSize: 14
            font.italic:    true
            color: Style.content_main
            wrapMode: Text.Wrap
            horizontalAlignment : Text.AlignHCenter
            //% "Send this Slatepack to the recipient over any channel. Paste their response back into the wallet to complete the transaction."
            text: qsTrId("slatepack-send-message")
        }
    }

    onAccepted: {
        BeamGlobals.copyToClipboard(slatepackValue.text);
    }
}
