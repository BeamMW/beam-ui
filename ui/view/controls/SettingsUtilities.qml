import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
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

    content: ColumnLayout {
        spacing: 30

        LinkButton {
            //% "Get Beams from Beam Community Faucet"
            text: qsTrId("settings-get-beam")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                Utils.openExternalWithConfirmation(Style.faucetUrl);
            }
        }

        /*LinkButton {
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
        }*/

        LinkButton {
            //% "Rescan"
            text: qsTrId("general-rescan")
            linkColor: "#ffffff"
            bold: true
            enabled: viewModel.localNodeRun && confirmRefreshDialog.canRefresh && viewModel.isLocalNodeRunning
            onClicked: {
                confirmRefreshDialog.open()
            }
        }
    }
}
