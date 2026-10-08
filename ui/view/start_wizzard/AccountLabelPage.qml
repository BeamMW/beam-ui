import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

WizzardPage {
    ColumnLayout {
        Layout.alignment:       Qt.AlignHCenter
        Layout.fillWidth:       false
        Layout.preferredWidth:  730
        SFText {
            Layout.alignment:   Qt.AlignHCenter
            //% "Create new wallet"
            text: qsTrId("general-create-wallet")
            color: Style.content_main
            font.pixelSize: 36
        }
        ColumnLayout {
            Layout.topMargin:       50
            Layout.alignment:       Qt.AlignHCenter
            Layout.fillWidth:       false
            Layout.preferredWidth:  400
            spacing:                10
            SFText {
                //% "Account name"
                text:                   qsTrId("start-new-account-label")
                color:                  Style.content_main
                font.pixelSize:         14
                font.styleName:        "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id:                    accountLabel
                Layout.fillWidth:      true
                font.pixelSize:        14
                color:                 Style.content_main
                text:                  viewModel.newAccountLabel
                onTextChanged: {
                    viewModel.accountLabelExists = false;
                }
                Binding {
                    target: viewModel
                    property: "newAccountLabel"
                    value: accountLabel.text
                }
                Component.onCompleted: {
                    viewModel.newAccountLabel = viewModel.defaultNewAccountLabel
                }
            }

            SFText {
                id: accountLabelError
                visible: viewModel.accountLabelExists
                //% "An account with the same label already exists."
                text: qsTrId("account-label-exists-error")
                color: Style.validator_error
                font.pixelSize: 14
            }
        }
        AccountPictureGrid {
            id: picturesView
            Layout.topMargin:       40
            Layout.alignment:       Qt.AlignHCenter
            Layout.fillWidth:       false
            Layout.fillHeight:      false
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
            enabled: accountLabel.text.length > 0 && !viewModel.accountLabelExists
            icon.source: "qrc:/assets/icon-next-blue.svg"
            onClicked: {
                viewModel.setNewAccountPictureIndex(picturesView.currentIndex)
                startWizzardView.push(createPasswordPage)
            }
        }
    ]
}