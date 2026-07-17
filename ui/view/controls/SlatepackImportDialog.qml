import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: thisDialog
    property var onImport: function (text) {}

    //% "Paste Slatepack"
    title: qsTrId("slatepack-import-title")
    //% "Import"
    okButtonText: qsTrId("slatepack-import-ok")
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonText: qsTrId("general-close")
    cancelButtonVisible: true
    width: 520

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
        ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.bottomMargin: 20
            clip: true
            TextArea {
                id: input
                wrapMode: TextArea.WrapAnywhere
                selectByMouse: true
                font.pixelSize: 13
                color: Style.content_secondary
                //% "Paste here"
                placeholderText: qsTrId("slatepack-import-placeholder")
            }
        }
    }

    onAccepted: {
        thisDialog.onImport(input.text.trim());
    }
}
