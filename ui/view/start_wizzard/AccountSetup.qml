import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils


RowLayout {
    property bool createNewAccount: false
    spacing: 20
    ColumnLayout {
        Layout.alignment:   Qt.AlignHCenter
        SFText {
            //% "Network"
            text: qsTrId("start-network-label")
            color: Style.content_main
            font.pixelSize: 14
            font.styleName: "Bold"; font.weight: Font.Bold
        }
        CustomComboBox {
            id: networkSelector
            Layout.fillWidth: true
            Layout.maximumWidth: 190
            fontPixelSize: 14
            enableScroll: false
            textRole: "name"

            model: viewModel.networks
            currentIndex: viewModel.currentNetworkIndex
            onActivated: {
                viewModel.currentNetwork = currentText;
                Theme.update();
            }
        }
    }
    ColumnLayout {
        visible:     viewModel.accounts.length > 1 && !createNewAccount
        SFText {
            //% "Account"
            text: qsTrId("start-account-label")
            color: Style.content_main
            font.pixelSize: 14
            font.styleName: "Bold"; font.weight: Font.Bold
        }

        CustomComboBox {
            id:                 accountSelector
            Layout.fillWidth:   true
            Layout.maximumWidth:190
            fontPixelSize:      14
            enableScroll:       false
            textRole:           "name"
            model:              viewModel.accounts
            currentIndex:       viewModel.currentAccountIndex
            onActivated: {
                viewModel.currentAccountIndex = currentIndex
            }
        }
    }
}