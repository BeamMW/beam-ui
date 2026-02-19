import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Beam.Wallet 1.0
import "controls"
import "wallet"
import "utils.js" as Utils

ColumnLayout {
    id:           root
    spacing:      0

    property var walletStackView: StackView.view
    property string openedTxID: ""
    
    function onAccepted() { walletStackView.pop(); }
    function onClosed() { walletStackView.pop(); }
    function onSwapToken(token) {
        tokenDuplicateChecker.checkTokenForDuplicate(token);
    }

    WalletViewModel {
        id: viewModel
    }

    property bool openSend:     false
    property bool openReceive:  false
    property string token:      ""

    function navigateSend(assetId) {
        var params = {
            "onAccepted":    onAccepted,
            "onClosed":      onClosed,
            "receiverToken": root.token
        }

        if (assetId != undefined)
        {
            params["assetId"] = assetId >= 0 ? assetId : 0
        }

        walletStackView.push(Qt.createComponent("send_regular.qml"), params)
        root.token = ""
    }

    function navigateReceive(assetId) {
        walletStackView.push(Qt.createComponent("receive_regular.qml"),
                                {"onClosed": onClosed,
                                    "token":    root.token,
                                    "assetId":  assetId})
        token = ""
    }

    //% "Wallet"
    property string title:      qsTrId("wallet-title")
    property var titleContent:  RowLayout {
        spacing: 20
        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }

        CustomButton {
            id: sendButton
            Layout.preferredHeight: 32
            palette.button: Style.accent_outgoing
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-send-blue.svg"
            //% "Send"
            text: qsTrId("general-send")
            font.pixelSize: 12
            onClicked: {
                navigateSend(assets.selectedId);
            }
        }

        CustomButton {
            Layout.preferredHeight: 32
            palette.button: Style.accent_incoming
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-receive-blue.svg"
            //% "Receive"
            text: qsTrId("wallet-receive-button")
            font.pixelSize: 12
            onClicked: {
                navigateReceive(assets.selectedId);
            }
        }

        ContextMenu {
            id: swapContextMenu
            Action {
                text:           qsTrId("atomic-swap-title")
                icon.source:    "qrc:/assets/icon-atomic_swap.svg"
                onTriggered: {
                    main.openAtomicSwaps();
                }
            }
            Action {
                text:           qsTrId("assets-swap-title")
                icon.source:    "qrc:/assets/icon-assets_swap.svg"
                onTriggered: {
                    main.openAssetSwaps();
                }
            }
        }

        CustomButton {
            id:                     swapButton
            Layout.preferredHeight: 32
            palette.button:         Style.active
            palette.buttonText:     Style.content_opposite
            icon.source:            "qrc:/assets/icon-swap-blue.svg"
            //% "Swap"
            text: qsTrId("wallet-swap-button")
            font.pixelSize:         12
            onClicked: {
                swapContextMenu.popup(swapButton, Qt.point(0, swapButton.height + 6))
            }
        }
    }

    AssetsPanel {
        id: assets
        Layout.fillWidth: true

        Binding {
            target:    txTable
            property:  "selectedAssets"
            value:     assets.selectedIds
        }
    }

    SFText {
        Layout.topMargin: assets.folded ? 25 : 35
        Layout.fillWidth: true

        font {
            pixelSize: 14
            letterSpacing: 3.11
            styleName: "DemiBold"; weight: Font.DemiBold
            capitalization: Font.AllUppercase
        }

        opacity: 0.5
        color: Style.content_main
        //% "Transactions"
        text: qsTrId("wallet-transactions-title")
    }

    TxTable {
        id:    txTable
        owner: root

        Layout.topMargin:  12
        Layout.fillWidth:  true
        Layout.fillHeight: true
    }

    Component.onDestruction: {
        //var item = walletStackView.currentItem;
        //if (item && item.saveAddress && typeof item.saveAddress == "function") {
        //    item.saveAddress();
        //}
    }
}
