import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils


RowLayout {
    property bool createNewAccount: false
    spacing: 20

    ColumnLayout {
        visible:     !createNewAccount
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
            Layout.maximumWidth:400
            fontPixelSize:      14
            enableScroll:       false
            textRole:           "label"
            model:              viewModel.accounts
            currentIndex:       viewModel.currentAccountIndex
            onActivated: {
                viewModel.currentAccountIndex = currentIndex
            }
        }
    }
}