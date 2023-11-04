import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

WizzardPage {
    Layout.fillWidth:   true
    ColumnLayout {
        Layout.alignment:   Qt.AlignHCenter
        SFText {
            Layout.alignment:   Qt.AlignHCenter
            //% "Create new wallet"
            text: qsTrId("general-create-wallet")
            color: Style.content_main
            font.pixelSize: 36
        }
        SFText {
            //% "Account label"
            text: qsTrId("start-new-account-label")
            color: Style.content_main
            font.pixelSize: 14
            font.styleName: "Bold"; font.weight: Font.Bold
        }

        SFTextInput {
            id:                 accountLabel
            Layout.fillWidth:   true
            Layout.maximumWidth:190
            font.pixelSize:     14
            color:              Style.content_main
            text:               viewModel.newAccountLabel
        }
    }

    buttons: [

        CustomButton {
            //% "Back"
            text: qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            onClicked: startWizzardView.pop();
        },

        PrimaryButton {
            id: checkRecoveryNextButton
            //% "Next"
            text: qsTrId("general-next")
            enabled: accountLabel.text.length >0
            icon.source: "qrc:/assets/icon-next-blue.svg"
            onClicked: {
                viewModel.newAccountLabel = accountLabel.text
                startWizzardView.push(createPasswordPage)
            }
        }
    ]
}