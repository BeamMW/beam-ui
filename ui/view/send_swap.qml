import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: sendSwapView
    spacing: 0
    property var defaultFocusItem: commentInput
    property var predefinedTxParams: undefined
    property alias swapToken: viewModel.token

    // callbacks set by parent
    property var onAccepted: undefined
    property var onClosed: undefined

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    function validateCoin() {
        var currency = viewModel.sendCurrency
        if (currency == OldWalletCurrency.CurrBeam) {
            currency = viewModel.receiveCurrency;

            if (currency == OldWalletCurrency.CurrBeam) return;
        }

        if (!BeamGlobals.canReceive(currency)) {
/*% "%1 is not connected, 
please review your settings and try again"
*/
            swapna.text = qsTrId("swap-currency-na-message").arg(BeamGlobals.getCurrencyName(currency))
            swapna.open()
            return false;
        }

        return true;
    }

    function setToken(token) {
        viewModel.token = token
        validateCoin();
    }

    function isValid () {
        return receiveAmountInput.isValid && sendAmountInput.isValid &&
               receiveFeeInput.isValid && sendFeeInput.isValid;
    }

    SwapNADialog {
        id:         swapna
        onRejected: sendSwapView.onClosed();
        onAccepted: main.openSettings("ALL_COINS");
    }

    Component.onCompleted: {
        commentInput.forceActiveFocus();
        if (predefinedTxParams != undefined) {
            viewModel.setParameters(predefinedTxParams);
            validateCoin();
        }
    }

    SendSwapViewModel {
        id: viewModel

        // TODO:SWAP Implement the necessary callbacks and error handling for the send operation
        /*
        onSendMoneyVerified: {
           parent.enabled = true
           walletView.pop()
        }

        onCantSendToExpired: {
            parent.enabled = true;
            Qt.createComponent("send_expired.qml")
                .createObject(sendView)
                .open();
        }
        */
    }

    SwapTokenInfoDialog {
        id:             tokenInfoDialog
        token:          viewModel.token
        //expirationTime: expiresCombo.displayText
        rate:           rate.text
        offered:        offered.text
        expires:        expires.text
    }

    Timer {
        interval: 1000
        repeat:   true
        running:  true

        onTriggered: {
            const expired = viewModel.expiresTime < (new Date())
            expires.color = expired ? Style.validator_error : Style.content_main
            expiredLabel.visible = expired
        }
    }

    //% "Accept Offer"
    property string title:       qsTrId("wallet-send-swap-title")

    ScrollView {
        id:                  scrollView
        Layout.fillWidth:    true
        Layout.fillHeight:   true
        Layout.bottomMargin: 10
        clip:                true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical:          CustomScrollBar {}

        ColumnLayout {
            width: scrollView.availableWidth

            //
            // Content row
            //
            RowLayout {
                Layout.fillWidth:   true
                spacing:  10

                //
                // Left column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    //
                    // Send amount
                    //
                    Panel {
                        //% "I send"
                        title:                   qsTrId("atomic-swap-i-send")
                        Layout.fillWidth:        true

                        content:
                        AmountInput {
                            id:           sendAmountInput
                            amount:       viewModel.sendAmount
                            currencies:   viewModel.currList
                            currencyIdx:  viewModel.sendCurrency
                            rate:         viewModel.secondCurrencySendRateValue
                            rateUnit:     viewModel.secondCurrencyUnitName
                            readOnlyA:    true
                            multi:        false
                            color:        Style.accent_outgoing
                            currColor:    viewModel.receiveCurrency == viewModel.sendCurrency || getErrorText().length ? Style.validator_error : Style.content_main
                            error:        getErrorText()

                            function getErrorText() {
                                if(!viewModel.isSendFeeOK) {
                                    //% "The swap amount must be greater than the transaction fee"
                                    return qsTrId("send-less-than-fee")
                                }
                                if(!viewModel.isEnough) {
                                    //% "There is not enough funds to complete the transaction"
                                    return qsTrId("send-not-enough")
                                }
                                return ""
                            }
                        }

                        Connections {
                            target: viewModel
                            function onSendAmountChanged () {
                                sendAmountInput.amount = viewModel.sendAmount
                            }
                        }

                        Connections {
                            target: viewModel
                            function onIsSendFeeOKChanged () {
                                sendAmountInput.error = sendAmountInput.getErrorText()
                            }
                        }
                    }

                    //
                    // Send Fee
                    //
                    FoldablePanel {
                        title:                   viewModel.sentFeeTitle
                        Layout.fillWidth:        true
                        folded:                  false
                        content: FeeInput {
                            id:                         sendFeeInput
                            currency:                   viewModel.sendCurrency
                            minFee:                     currency == OldWalletCurrency.CurrBeam ? viewModel.minimalBeamFeeGrothes : BeamGlobals.getMinimalFee(currency, false)
                            maxFee:                     BeamGlobals.getMaximumFee(currency)
                            recommendedFee:             BeamGlobals.getRecommendedFee(currency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(currency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         sendAmountInput.showRate
                            isExchangeRateAvailable:    sendAmountInput.isExchangeRateAvailable
                            rateAmount:                 Utils.formatFeeToSecondCurrency(viewModel.sendFee, viewModel.secondCurrencySendRateValue, viewModel.secondCurrencyUnitName)
                            rateUnit:                   viewModel.secondCurrencyUnitName
                        }

                        Binding {
                            target:   viewModel
                            property: "sendFee"
                            value:    sendFeeInput.fee
                        }

                        Connections {
                            target: viewModel
                            function onSendFeeChanged () {
                                sendFeeInput.fee = viewModel.sendFee
                            }
                        }
                    }

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title:             qsTrId("general-comment")
                        Layout.fillWidth:        true

                        content:
                        ColumnLayout {
                            SFTextInput {
                                id:               commentInput
                                Layout.fillWidth: true
                                font.pixelSize:   14
                                color:            Style.content_main
                                maximumLength:    BeamGlobals.maxCommentLength()
                                //% "Comments are local and won't be shared"
                                placeholderText:  qsTrId("general-comment-local")
                            }

                            Binding {
                                target:   viewModel
                                property: "comment"
                                value:    commentInput.text
                            }
                        }
                    }

                    //
                    // ERC-20 token details
                    //
                    FoldablePanel {
                        visible:                 viewModel.isErc20Swap
                        //% "Token"
                        title:                   qsTrId("swap-accept-token-title")
                        Layout.fillWidth:        true

                        content: ColumnLayout {
                            SFText {
                                font.pixelSize:   14
                                color:            Style.content_secondary
                                //% "Contract address"
                                text:             qsTrId("swap-accept-token-contract-label")
                            }
                            SFLabel {
                                Layout.fillWidth:    true
                                font.pixelSize:      14
                                font.family:         "Monospace"
                                color:               Style.content_main
                                wrapMode:            Text.WrapAnywhere
                                copyMenuEnabled:     true
                                text:                viewModel.tokenContract
                            }
                            SFText {
                                Layout.topMargin: 10
                                font.pixelSize:   14
                                color:            Style.content_secondary
                                //% "Symbol / decimals"
                                text:             qsTrId("swap-accept-token-symbol-label")
                            }
                            SFText {
                                font.pixelSize: 14
                                color:          Style.content_main
                                text:           viewModel.tokenSymbol + " / " + viewModel.tokenDecimals
                            }
                            SFText {
                                Layout.topMargin:  10
                                Layout.fillWidth:  true
                                font.pixelSize:    12
                                color:             Style.validator_error
                                wrapMode:          Text.Wrap
                                //% "Verify this token contract address carefully. Anyone can create a token with any name."
                                text:              qsTrId("swap-accept-token-warning")
                            }
                        }
                    }

                    //
                    // Confidential Asset details
                    //
                    FoldablePanel {
                        visible:                 viewModel.isBeamAssetSwap
                        //% "Confidential Asset"
                        title:                   qsTrId("swap-accept-asset-title")
                        Layout.fillWidth:        true

                        content: ColumnLayout {
                            SFText {
                                font.pixelSize: 14
                                color:          Style.content_main
                                //% "Asset id %1, unit %2"
                                text:           qsTrId("swap-accept-asset-id-unit").arg(viewModel.beamAssetId).arg(viewModel.beamAssetUnitName)
                            }
                            SFText {
                                visible:           viewModel.needsBeamForRedeemFee
                                Layout.topMargin:  10
                                Layout.fillWidth:  true
                                font.pixelSize:    12
                                color:             Style.content_secondary
                                wrapMode:          Text.Wrap
                                //% "You are receiving a Confidential Asset. A small BEAM balance is required to pay the redeem transaction fee."
                                text:              qsTrId("swap-accept-asset-beam-fee")
                            }
                        }
                    }
                }

                //
                // Right column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10
                    //
                    // Receive amount
                    //
                    Panel {
                        //% "I receive"
                        title:                   qsTrId("atomic-swap-i-receive")
                        Layout.fillWidth:        true
                        content:

                        AmountInput {
                            id:            receiveAmountInput
                            amount:        viewModel.receiveAmount
                            currencies:    viewModel.currList
                            currencyIdx:   viewModel.receiveCurrency
                            rate:          viewModel.secondCurrencyReceiveRateValue
                            rateUnit:      viewModel.secondCurrencyUnitName
                            readOnlyA:     true
                            multi:         false
                            color:         Style.accent_incoming
                            currColor:     viewModel.receiveCurrency == viewModel.sendCurrency || getErrorText().length ? Style.validator_error : Style.content_main
                            error:         getErrorText()

                            function getErrorText() {
                                if(!viewModel.isReceiveFeeOK) {
                                    //% "The swap amount must be greater than the transaction fee"
                                    return qsTrId("send-less-than-fee")
                                }
                                if(!viewModel.isEnoughToReceive) {
                                    //% "There is not enough funds to complete the transaction"
                                    return qsTrId("send-not-enough")
                                }
                                return ""
                            }
                        }

                        Connections {
                            target: viewModel
                            function onReceiveAmountChanged () {
                                receiveAmountInput.amount = viewModel.receiveAmount
                            }
                        }

                        Connections {
                            target: viewModel
                            function onIsReceiveFeeOKChanged () {
                                receiveAmountInput.error = receiveAmountInput.getErrorText()
                            }
                        }
                    }
                    //
                    // Fee
                    //
                    FoldablePanel {
                        title:                   viewModel.receiveFeeTitle
                        Layout.fillWidth:        true
                        folded:                  false
                        content: FeeInput {
                            id:                         receiveFeeInput
                            currency:                   viewModel.receiveCurrency
                            minFee:                     BeamGlobals.getMinimalFee(currency, false)
                            maxFee:                     BeamGlobals.getMaximumFee(currency)
                            recommendedFee:             BeamGlobals.getRecommendedFee(currency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(currency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         receiveAmountInput.showRate
                            isExchangeRateAvailable:    receiveAmountInput.isExchangeRateAvailable
                            rateAmount:                 Utils.formatFeeToSecondCurrency(viewModel.receiveFee, viewModel.secondCurrencyReceiveRateValue, viewModel.secondCurrencyUnitName)
                            rateUnit:                   viewModel.secondCurrencyUnitName
                        }

                        Binding {
                            target:   viewModel
                            property: "receiveFee"
                            value:    receiveFeeInput.fee
                        }
                    }

                    //
                    // Summary pane
                    //
                    Pane {
                        Layout.fillWidth:        true
                        padding:                 20

                        background: Rectangle {
                            radius: 10
                            color:  Style.background_button
                        }

                        ColumnLayout {
                            anchors.fill:        parent
                            spacing:             20
                            GridLayout {
                                Layout.fillWidth:    true
                                columnSpacing:       20
                                rowSpacing:          20
                                columns:             2

                                property bool showEstimatedFee: viewModel.receiveCurrency != OldWalletCurrency.CurrBeam

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    text:                   Utils.getSwapTotalFeeTitle(receiveAmountInput.currencyUnit)
                                    visible:                parent.showEstimatedFee
                                }
    
                                SFText {
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             BeamGlobals.calcWithdrawTxFee(viewModel.receiveCurrency, viewModel.receiveFee)
                                    visible:          parent.showEstimatedFee
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Offered on"
                                    text:                   qsTrId("wallet-send-swap-offered-label") + ":"
                                }
    
                                SFText {
                                    id:               offered
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             Utils.formatDateTime(viewModel.offeredTime, BeamGlobals.getLocaleName())
                                }

                                SFText {
                                    id:                     expiresTitle
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Expires on"
                                    text:                   qsTrId("wallet-send-swap-expires-label") + ":"
                                }
                                ColumnLayout {
                                    SFText {
                                        id:               expires
                                        font.pixelSize:   14
                                        color:            Style.content_main
                                        text:             Utils.formatDateTime(viewModel.expiresTime, BeamGlobals.getLocaleName())
                                    }
                                    SFText {
                                        id:               expiredLabel
                                        font.pixelSize:   14
                                        color:            Style.validator_error
                                        //% "Expired."
                                        text:             qsTrId("swap-expired")
                                        visible:          false
                                    }
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Exchange rate"
                                    text:                   qsTrId("general-rate") + ":"
                                }
            
                                SFText {
                                    id:               rate
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.isSendBeam
                                        ? ["1", sendAmountInput.currencyUnit, "=", Utils.uiStringToLocale(viewModel.rate), receiveAmountInput.currencyUnit].join(" ")
                                        : ["1", receiveAmountInput.currencyUnit, "=", Utils.uiStringToLocale(viewModel.rate), sendAmountInput.currencyUnit].join(" ")
                                }


                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Swap token"
                                    text:                   qsTrId("send-swap-token") + ":"
                                }
                                RowLayout {
                                    Layout.fillWidth:        true
                                    SFLabel {
                                        id:                  tokenLabel
                                        Layout.fillWidth:    true
                                        font.pixelSize:      14
                                        color:               Style.content_main
                                        elide:               Text.ElideMiddle
                                        text:                viewModel.token
                                    }
                                
                                    LinkButton {
                                        //% "Token details"
                                        text: qsTrId("swap-token-details")
                                        linkColor: Style.accent_outgoing
                                        onClicked: {
                                            tokenInfoDialog.open();
                                        }
                                    }
                                }

                            } // GridLayoyut
                        } // ColumnLayout
                    }
                } // ColumnLayout
            } // RowLayout

            //
            // Footers
            //
            CustomButton {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                //% "Swap"
                text:                qsTrId("general-swap")
                palette.buttonText:  Style.content_opposite
                palette.button:      Style.accent_outgoing
                icon.source:         "qrc:/assets/icon-create-offer.svg"
                enabled:             viewModel.canSend && sendSwapView.isValid()
                onClicked: {
                    if (!validateCoin()) return;

                    var unitName = viewModel.currList[viewModel.sendCurrency].unitName
                    var onlineMessage =
                        //% "Keep your wallet online. The swap normally takes about 1 hour to complete."
                        qsTrId("send-swap-sconfirmation-online-time") + (viewModel.sendCurrency !== OldWalletCurrency.CurrBeam ?
                        //% " Once the offer is accepted by the other side, the %1 transaction fee will be charged even if the offer is cancelled."
                        qsTrId("send-swap-fee-warning").arg(unitName)
                        : "")

                    const dialogComponent = Qt.createComponent("swap_confirm.qml");
                    var dialogObject = dialogComponent.createObject(sendSwapView,
                        {
                            swapMode:       true,
                            addressText:    viewModel.receiverAddress,
                            typeText:       qsTrId("general-swap"),
                            amount:         viewModel.sendAmount,
                            unitName:       unitName,
                            fee:            viewModel.sendFee,
                            flatFee:        viewModel.sendCurrency == OldWalletCurrency.CurrBeam,
                            rate:           viewModel.secondCurrencySendRateValue,
                            rateUnit:       viewModel.secondCurrencyUnitName,
                            isErc20Swap:           viewModel.isErc20Swap,
                            tokenContract:         viewModel.tokenContract,
                            tokenSymbol:           viewModel.tokenSymbol,
                            tokenDecimals:         viewModel.tokenDecimals,
                            isBeamAssetSwap:       viewModel.isBeamAssetSwap,
                            beamAssetId:           viewModel.beamAssetId,
                            beamAssetUnitName:     viewModel.beamAssetUnitName,
                            needsBeamForRedeemFee: viewModel.needsBeamForRedeemFee,
                        })

                    dialogObject.onAccepted.connect(function () {
                        viewModel.sendMoney()
                        sendSwapView.onAccepted()
                    })

                    dialogObject.open()
                }
            }
        }  // ColumnLayout
    }  // ScrollView
} // ColumnLayout