import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    AssetsViewModel {
        id: viewModel
    }

    contentItem: GridLayout {
        columnSpacing: 10
        rowSpacing: 10
        columns: 3
        rows: 3

        Repeater {
            model: viewModel.assets

            delegate: AssetInfo {
                height: 67
                width:  220

                inTxCnt:   model.inTxCnt
                outTxCnt:  model.outTxCnt
                amount:    model.amount
                symbol:    model.name

                rate:      "0.25"
                symbol2:   "USD"
                icon:      "qrc:/assets/icon-beam.svg"
            }
        }
        Item {
            Layout.fillWidth: true
        }
    }
}
