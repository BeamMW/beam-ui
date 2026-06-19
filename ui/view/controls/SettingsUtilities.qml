import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

SettingsFoldable {
    id: control
    property var viewModel

    //% "Utilities"
    title: qsTrId("settings-utilities-title")

    ConfirmRefreshDialog {
        id: confirmRefreshDialog
        parent: main
        settingsViewModel: viewModel
    }

    ConfirmationDialog {
        id: removeWalletConfirmationDialog
        parent: main
        //% "Remove current wallet"
        title: qsTrId("settings-remove-wallet")
        //% "All data will be erased. Make sure you've saved your seed phrase if you want to restore this wallet later on!"
        text: qsTrId("settings-remove-wallet-confirm-message") + "\n\n" +
              //% "Are you sure you want to remove your wallet?"
              qsTrId("settings-remove-wallet-confirm-question")
        //% "proceed"
        okButtonText: qsTrId("settings-remove-wallet-proceed")
        okButtonIconSource: "qrc:/assets/icon-next-white.svg"
        okButtonColor: Style.swapStateIndicator
        cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
        cancelButtonVisible: true
        width: 460
        height: 252

        onAccepted: {
            removeWalletPasswordDialog.open()
        }
    }

    ConfirmPasswordDialog {
        id: removeWalletPasswordDialog
        parent: main
        settingsViewModel: viewModel
        //% "Confirm wallet removal"
        dialogTitle: qsTrId("settings-remove-wallet-password-title")
        dialogMessage: ""
        //% "Enter your password to remove the wallet"
        passwordPlaceholderText: qsTrId("settings-remove-wallet-password-placeholder")
        //% "remove"
        okButtonText: qsTrId("settings-remove-wallet-password-button")
        okButtonIcon: "qrc:/assets/icon-delete.svg"
        okButtonColor: Style.swapStateIndicator

        onDialogAccepted: function() {
            // Defer one turn so this (modal) dialog finishes closing and releases
            // its input grab on the window overlay before we navigate away;
            // otherwise the grab leaks and blocks input on the start screen.
            //
            // Inside the callback: drop the main wallet UI first (setSource
            // schedules deferred deletion of every main view-model, releasing
            // their references to the asset managers), then removeCurrentWallet()
            // - viewModel is only marked for deletion and still alive here - so
            // the reset is posted AFTER those deletions and AppModel::onResetWallet()
            // sees the asset managers uniquely owned.
            Qt.callLater(function() {
                main.parent.setSource("qrc:/start.qml")
                viewModel.removeCurrentWallet()
            })
        }
        onDialogRejected: function() {}
    }

    PublicOfflineAddressDialog {
        id: publicOfflineAddressDialog;
    }

    UtxoDialog {
        id: utxoDialog
    }

    content: ColumnLayout {
        spacing: 30

        LinkButton {
            //% "Show public offline address"
            text:       qsTrId("settings-show-public-offline-address")
            linkColor:  "#ffffff"
            bold: true
            enabled:    statusbarModel.isConnectionTrusted
            onClicked: {
                publicOfflineAddressDialog.address = Qt.binding(function() { return viewModel.publicAddress;})
                publicOfflineAddressDialog.open();
            }
        }

        LinkButton {
            //% "Get Beams from Beam Community Faucet"
            text:       qsTrId("settings-get-beam")
            linkColor:  "#ffffff"
            bold:       true
            onClicked: {
                Utils.openExternalWithConfirmation(viewModel.faucetUrl);
            }
        }

        LinkButton {
            //% "Export wallet data"
            text: qsTrId("settings-export")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                viewModel.exportData()
            }
        }

        LinkButton {
            //% "Import wallet data"
            text: qsTrId("settings-import")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                viewModel.importData()
            }
        }

        LinkButton {
            //% "Rescan"
            text: qsTrId("general-rescan")
            linkColor: "#ffffff"
            bold: true
            enabled: statusbarModel.isConnectionTrusted && statusbarModel.isOnline && confirmRefreshDialog.canRefresh 
            onClicked: {
                confirmRefreshDialog.open()
            }
        }

        LinkButton {
            //% "Show UTXO"
            text: qsTrId("settings-utilities-show-utxo")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                utxoDialog.open()
            }
        }

        LinkButton {
            //% "Remove current wallet"
            text: qsTrId("settings-remove-wallet")
            linkColor: Style.validator_error
            bold: true
            onClicked: {
                removeWalletConfirmationDialog.open()
            }
        }
    }
}
