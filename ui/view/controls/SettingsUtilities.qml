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
    }
}
