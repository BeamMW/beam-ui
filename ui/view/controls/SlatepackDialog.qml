import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: thisDialog
    property string armored: ""

    //% "Slatepack to send"
    title: qsTrId("slatepack-send-title")
    okButtonText: qsTrId("general-copy")
    okButtonIconSource: "qrc:/assets/icon-copy-blue.svg"
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonText: qsTrId("general-close")
    cancelButtonVisible: true
    width: 520

    contentItem: ColumnLayout {
        spacing: 20
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            clip: true

            SFLabel {
                id: slatepackValue
                width: thisDialog.width - 40
                leftPadding: 20
                rightPadding: 20
                topPadding: 15
                font.pixelSize: 13
                color: Style.content_secondary
                wrapMode: Text.WrapAnywhere
                text: thisDialog.armored
                copyMenuEnabled: true
                onCopyText: BeamGlobals.copyToClipboard(text)
            }
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
