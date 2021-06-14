import QtQuick          2.11
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.4
import Beam.Wallet      1.0
import "controls"
import "dex"

ColumnLayout {
    id: control
    Layout.fillWidth: true

    DexViewModel {
        id: viewModel
    }

    //
    // Page Header (Title + Status Bar)
    //
    Title {
        //% "Order Book"
        text: qsTrId("dex-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    ColumnLayout {
        Layout.fillWidth:  true
        Layout.fillHeight: true
        Layout.topMargin:  50

        MarketPanel {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
        }

        Row {
            Layout.topMargin: 15
            Layout.bottomMargin: 15
            spacing: 15

            CustomButton {
                text: "Buy 10 DEMOX"
                onClicked: {
                    viewModel.buyBEAMX()
                }
            }

            CustomButton {
                text: "Sell 10 DEMOX"
                onClicked: {
                    viewModel.sellBEAMX()
                }
            }
        }

        MarketTable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            viewModel: viewModel
        }
    }
}
