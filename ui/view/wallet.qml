import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "controls"
import "wallet"
import "utils.js" as Utils

Item {
    id: root
    anchors.fill: parent

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
    
    Title {
        x: 0
        //% "Wallet"
        text: qsTrId("wallet-title")
    }

    StatusBar {
        id: status_bar
        model: statusbarModel
        z: 33
    }

    Component {
        id: walletLayout

        ColumnLayout {
            id: transactionsLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: -27
            spacing: 0

            property bool showSelected:  false

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

            Row {
                Layout.alignment: Qt.AlignTop | Qt.AlignRight
                Layout.topMargin: 30
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

            RowLayout {
                Layout.topMargin: 25
                Layout.preferredHeight: 38
                spacing: 0

                SFText {
                    Layout.fillWidth: true

                    font {
                        pixelSize: 14
                        letterSpacing: 4
                        styleName: "DemiBold"; weight: Font.DemiBold
                        capitalization: Font.AllUppercase
                    }

                    opacity: 0.5
                    color: Style.content_main
                    //% "Assets"
                    text: qsTrId("wallet-assets-title")
                }

                CustomButton {
                    id: removeFilterButton
                    Layout.rightMargin: 20
                    // Layout.alignment: Qt.AlignTop
                    height: 32
                    palette.button: Style.background_button
                    palette.buttonText: Style.content_main
                    icon.source: "qrc:/assets/icon-cancel-white.svg"
                    visible: assets.selectedIds.length
                    //% "Remove filter"
                    text: qsTrId("wallet-remove-filter-button")
                    font.pixelSize: 12
                    onClicked: {
                        assets.clearSelectedAssets()
                        showSelected = false
                    }
                }

                SFText {
                    //% "Selected"
                    text: qsTrId("wallet-selected-assets-checkbox")
                    color: transactionsLayout.showSelected ? Style.active : Style.content_secondary
                    font.pixelSize: 14
                    opacity: assets.selectedIds.length == 0 ? 0.2 : 1
                }

                CustomSwitch {
                    id: assetsFilterSwitch
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    checkable: assets.selectedIds.length != 0
                    checked: assets.selectedIds.length == 0 ? true : !transactionsLayout.showSelected
                    alwaysGreen: true
                    leftPadding: 0
                    rightPadding: 0
                    spacing: 0

                    Binding {
                        target: transactionsLayout
                        property: "showSelected"
                        value: !assetsFilterSwitch.checked
                    }
                }

                SFText {
                    //% "All"
                    text: qsTrId("wallet-all-assets-checkbox")
                    color: transactionsLayout.showSelected ? Style.content_secondary : Style.active
                    font.pixelSize: 14
                }
            }

            AssetsPanel {
                id: assets
                Layout.topMargin: 25
                Layout.fillWidth: true
                showSelected: transactionsLayout.showSelected

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
        anchors.fill: parent
        anchors.topMargin: -27
        initialItem: walletLayout
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
