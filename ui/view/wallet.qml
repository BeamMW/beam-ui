import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "controls"
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
            function onTokenPreviousAccepted(token) {
                tokenDuplicateChecker.isOwn = false;
                tokenDuplicateChecker.open();
            }
            function onTokenFirstTimeAccepted(token) {
                walletStackView.pop();
                walletStackView.push(Qt.createComponent("send_swap.qml"),
                                     {
                                         "onAccepted": tokenDuplicateChecker.onAccepted,
                                         "onClosed": onClosed,
                                         "swapToken": token
                                     });
                walletStackView.currentItem.validateCoin();
            }
            function onTokenOwnGenerated(token) {
                tokenDuplicateChecker.isOwn = true;
                tokenDuplicateChecker.open();
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
    }

    Component {
        id: walletLayout

        ColumnLayout {
            id: transactionsLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            function navigateSend() {
                walletStackView.push(Qt.createComponent("send_regular.qml"),
                                             {"onAccepted":      onAccepted,
                                              "onClosed":        onClosed,
                                              "onSwapToken":     onSwapToken,
                                              "receiverAddress": token});
                token = "";
            }

            function navigateReceive() {
                walletStackView.push(Qt.createComponent("receive_regular.qml"), 
                                                {"onClosed": onClosed,
                                                 "token":    token
                                                });
                token = "";
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
                    //font.capitalization: Font.AllUppercase
                    onClicked: {
                        navigateSend();
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
                    //font.capitalization: Font.AllUppercase
                    onClicked: {
                        navigateReceive();
                    }
                }
            }

            AvailablePanel {
                Layout.topMargin:      29
                Layout.maximumHeight:  80
                Layout.minimumHeight:  80
                Layout.preferredWidth: parseFloat(viewModel.beamSending) > 0 || parseFloat(viewModel.beamReceiving) > 0 ? parent.width : (parent.width / 2)

                available:                  viewModel.beamAvailable
                locked:                     viewModel.beamLocked
                lockedMaturing:             viewModel.beamLockedMaturing
                sending:                    viewModel.beamSending
                receiving:                  viewModel.beamReceiving
                receivingChange:            viewModel.beamReceivingChange
                receivingIncoming:          viewModel.beamReceivingIncoming
                secondCurrencyLabel:        viewModel.secondCurrencyLabel
                secondCurrencyRateValue:    viewModel.secondCurrencyRateValue
            }

            SFText {
                Layout.topMargin: 45
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth : true

                font {
                    pixelSize: 14
                    letterSpacing: 4
                    styleName: "Bold"; weight: Font.Bold
                    capitalization: Font.AllUppercase
                }

                opacity: 0.5
                color: Style.content_main
                //% "Transactions"
                text: qsTrId("wallet-transactions-title")
            }

            TxTable {
                Layout.topMargin:  12
                Layout.fillWidth:  true
                Layout.fillHeight: true
            }
        }
    }

    StackView {
        id: walletStackView
        anchors.fill: parent
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
        } else if (root.openReceive) {
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

