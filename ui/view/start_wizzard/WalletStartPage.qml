import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

StartLayout {
    property Item defaultFocusItem: createNewWallet

    ConfirmationDialog {
        id: restoreWalletConfirmation

        //% "I agree"
        okButtonText: qsTrId("start-restore-confirm-button")
        okButtonIconSource: "qrc:/assets/icon-done.svg"
        okButtonAllLowercase: false
        cancelButtonVisible: false
        width: 460
        height: contentItem.implicitHeight + footer.implicitHeight
        padding: 0

        contentItem: Column {
            width: parent.width
            height: restoreWalletConfirmationTitle.implicitHeight + restoreWalletConfirmationMessage.implicitHeight
            SFText {
                id: restoreWalletConfirmationTitle
                topPadding: 20
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
                //% "Restore wallet"
                text: qsTrId("general-restore-wallet")
                color: Style.content_main
                font.pixelSize: 18
                font.styleName: "Bold"
                font.weight: Font.Bold
            }

            SFText {
                id: restoreWalletConfirmationMessage
                padding: 30
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment : Text.AlignHCenter
                width: parent.width
                //% "You are trying to restore an existing Beam Wallet. Please notice that if you use your wallet on another device, your balance will be up to date, but  transaction history and addresses will be kept separately on each device."
                text: qsTrId("start-restore-message-line")
                color: Style.content_main
                font.pixelSize: 14
                wrapMode: Text.Wrap
            }
        }
        onAccepted: {
            onClicked: {
                viewModel.isRecoveryMode = true;

                if (viewModel.useHWWallet)
                    startWizzardView.push(accountLabelPage);
                else
                    startWizzardView.push(restoreWalletPage);
            }
        }
    }

    ColumnLayout {
        id: startColumn
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 0

        AccountSetup {
            id:                     accountSetup
            Layout.alignment:       Qt.AlignHCenter
            Layout.minimumWidth:    400
            Layout.maximumWidth:    400
            createNewAccount:       true
        }

        RowLayout {
            Layout.alignment:   Qt.AlignHCenter
            Layout.fillWidth:   true
            Layout.topMargin:   30

            CustomButton {
                text: qsTrId("general-back")
                icon.source: "qrc:/assets/icon-back.svg"
                Layout.preferredHeight: 38
                visible: startWizzardView.depth > 1
                onClicked: {
                    startWizzardView.pop();
                }
            }

            spacing: 30

            Row {
                spacing: 20

                PrimaryButton {
                    id: createNewWallet
                    //% "Create new wallet"
                    text: qsTrId("general-create-wallet")
                    Layout.preferredHeight: 38
                    Layout.alignment: Qt.AlignHCenter
                    icon.source: "qrc:/assets/icon-add-blue.svg"
                    onClicked: {
                        viewModel.isRecoveryMode = false;
                        startWizzardView.push(createNewWalletPage);
                    }
                }

                PrimaryButton {
                    visible: viewModel.isTrezorEnabled
                    id: createNewTrezorWallet
                    //% "Create new Trezor wallet"
                    text: qsTrId("general-create-trezor-wallet")
                    Layout.preferredHeight: 38
                    Layout.alignment: Qt.AlignHCenter
                    icon.source: "qrc:/assets/icon-add-blue.svg"
                    onClicked: {
                        viewModel.isRecoveryMode = false;
                        startWizzardView.push(createTrezorWalletPage);
                    }
                }
            }
        }
        
        Item {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
        }
        Row {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 40
            Layout.bottomMargin: 37
            spacing: 20
            LinkButton {
                //% "Restore wallet"
                text: qsTrId("general-restore-wallet")
                fontSize: 14
                onClicked: {
                    viewModel.useHWWallet = false;
                    restoreWalletConfirmation.open();
                }
            }

            Rectangle {
                id:      separator
                anchors.verticalCenter: parent.verticalCenter
                height:  10
                width:   1
                color:   Style.active
                opacity: 0.3
            }

            LinkButton {
                //% "Use Hardware Wallet"
                text: qsTrId("general-use-hw-wallet")
                fontSize: 14
                onClicked: {
                    viewModel.useHWWallet = true;
                    restoreWalletConfirmation.open();
                }
            }
        }
    }
}