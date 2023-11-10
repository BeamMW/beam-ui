import QtQuick 2
import QtQuick.Controls 1
import QtQuick.Controls 2
import QtQuick.Controls.Styles 1
import QtGraphicalEffects 1
import QtQuick.Layouts 1
import Beam.Wallet 1
import "controls"
import "wallet"
import "utils.js" as Utils

ColumnLayout {
    id:           root
    anchors.fill: parent
    spacing:      0

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

    TokenDuplicateChecker {
        id: tokenDuplicateChecker
        onAccepted: {
            walletStackView.pop();
            main.openSwapActiveTransactionsList()
        }
        Connections {
            target: tokenDuplicateChecker.model

            function onTokenPreviousAccepted (token) {
                tokenDuplicateChecker.isOwn = false
                tokenDuplicateChecker.open()
            }

            function onTokenFirstTimeAccepted (token) {
                walletStackView.pop()
                walletStackView.push(Qt.createComponent("send_swap.qml"),
                                     {
                                         "onAccepted": tokenDuplicateChecker.onAccepted,
                                         "onClosed":   onClosed,
                                         "swapToken":  token
                                     })
                walletStackView.currentItem.validateCoin()
            }

            function onTokenOwnGenerated (token) {
                tokenDuplicateChecker.isOwn = true
                tokenDuplicateChecker.open()
            }
        }
    }

    Component {
        id: walletLayout

        ColumnLayout {
            id: transactionsLayout
            Layout.fillWidth:   true
            Layout.fillHeight:  true
            spacing: 0

            function navigateSend(assetId) {
                var params = {
                    "onAccepted":    onAccepted,
                    "onClosed":      onClosed,
                    "onSwapToken":   onSwapToken,
                    "receiverToken": root.token,
                    "assetId":       assetId
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

            Title {
                //% "Wallet"
                text: qsTrId("wallet-title")

                Item {
                    Layout.fillWidth:   true
                    Layout.fillHeight:  true
                }

                Row {
                    spacing: 20

                    CustomButton {
                        id: sendButton
                        height: 32
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
                        height: 32
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
                }
            }

            AssetsPanel {
                id: assets
                Layout.topMargin: 25
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
                    letterSpacing: 4
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
        }
    }

    StackView {
        id: walletStackView
        Layout.fillWidth:   true
        Layout.fillHeight:  true
        initialItem:        walletLayout
        pushEnter: Transition {
            enabled: false
        }
        pushExit: Transition {
            enabled: false
        }
        popEnter: Transition {
            enabled: false
        }
        popExit: Transition {
            enabled: false
        }
        onCurrentItemChanged: {
            if (currentItem && currentItem.defaultFocusItem) {
                walletStackView.currentItem.defaultFocusItem.forceActiveFocus();
            }
        }
    }

    Component.onCompleted: {
        if (root.openSend) {
            var item = walletStackView.currentItem;
            if (item && item.navigateSend && typeof item.navigateSend == "function" ) {
                item.navigateSend();
                root.openSend = false;
            }
        }
        else if (root.openReceive) {
            var item = walletStackView.currentItem;
            if (item && item.navigateReceive && typeof item.navigateReceive == "function" ) {
                item.navigateReceive();
                root.openReceive = false;
            }
        }
    }

    Component.onDestruction: {
        var item = walletStackView.currentItem;
        if (item && item.saveAddress && typeof item.saveAddress == "function") {
            item.saveAddress();
        }
    }
}
