import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: sendSwapView
    
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
        if (currency == Currency.CurrBeam) {
            currency = viewModel.receiveCurrency;

            if (currency == Currency.CurrBeam) return;
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
        onAccepted: main.openSwapSettings();
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

    //
    // Title row
    //
    Item {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30
        CustomButton {
            anchors.left:   parent.left
            anchors.verticalCenter: parent.verticalCenter
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true
            //% "Back"
            text:           qsTrId("general-back")
            icon.source:    "qrc:/assets/icon-back.svg"
            onClicked:      {
                onClosed();
            }
        }

        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            color:              Style.content_main
            font {
                styleName:      "Bold"
                weight:         Font.Bold
                pixelSize:      14
                letterSpacing:  4
                capitalization: Font.AllUppercase
            }
            //% "Accept Swap Offer"
            text:               qsTrId("wallet-send-swap-title")
        }
    } // Item

    ScrollView {
        id:                  scrollView
        Layout.fillWidth:    true
        Layout.fillHeight:   true
        Layout.bottomMargin: 10
        clip:                true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy:   ScrollBar.AsNeeded

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
                        //% "Send amount"
                        title:                   qsTrId("sent-amount-label")
                        Layout.fillWidth:        true

                        content:
                        AmountInput {
                            id:           sendAmountInput
                            currFeeTitle: true
                            amountIn:     viewModel.sendAmount
                            currency:     viewModel.sendCurrency
                            rate:         viewModel.secondCurrencySendRate
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
                            onIsSendFeeOKChanged: sendAmountInput.error = sendAmountInput.getErrorText()
                        }
                    }

                    //
                    // Send Fee
                    //
                    FoldablePanel {
                        title:                   sendAmountInput.getFeeTitle()
                        Layout.fillWidth:        true
                        folded:                  false
                        content: FeeInput {
                            id:                         sendFeeInput
                            //fee:                        viewModel.sendFee
                            currency:                   viewModel.sendCurrency
                            minFee:                     currency == Currency.CurrBeam ? viewModel.minimalBeamFeeGrothes : BeamGlobals.getMinimalFee(viewModel.sentCurrency, false)
                            recommendedFee:             BeamGlobals.getRecommendedFee(viewModel.sendCurrency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(viewModel.sendCurrency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         sendAmountInput.showSecondCurrency
                            isExchangeRateAvailable:    sendAmountInput.isExchangeRateAvailable
                            rateAmount:                 sendAmountInput.getFeeInSecondCurrency(viewModel.sendFee)
                            rateUnit:                   viewModel.secondCurrencyUnitName
                        }

                        Binding {
                            target:   viewModel
                            property: "sendFee"
                            value:    sendFeeInput.fee
                        }

                        Connections {
                            target: viewModel
                            onSendFeeChanged: sendFeeInput.fee = viewModel.sendFee
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
                                selectByMouse:    true
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
                        //% "Receive amount"
                        title:                   qsTrId("receive-amount-swap-label")
                        Layout.fillWidth:        true
                        content:

                        AmountInput {
                            id:            receiveAmountInput
                            currFeeTitle:  true
                            amountIn:      viewModel.receiveAmount
                            currency:      viewModel.receiveCurrency
                            rate:          viewModel.secondCurrencyReceiveRate
                            rateUnit:      viewModel.secondCurrencyUnitName
                            readOnlyA:     true
                            multi:         false
                            color:         Style.accent_incoming
                            currColor:     viewModel.receiveCurrency == viewModel.sendCurrency || getErrorText().length ? Style.validator_error : Style.content_main
                            error:         getErrorText()
                            showTotalFee:  true

                            function getErrorText() {
                                if(!viewModel.isReceiveFeeOK) {
                                    //% "The swap amount must be greater than the transaction fee"
                                    return qsTrId("send-less-than-fee")
                                }
                                return ""
                            }
                        }

                        Connections {
                            target: viewModel
                            onIsReceiveFeeOKChanged: receiveAmountInput.error = receiveAmountInput.getErrorText()
                        }
                    }
                    //
                    // Fee
                    //
                    FoldablePanel {
                        title:                   receiveAmountInput.getFeeTitle()
                        Layout.fillWidth:        true
                        folded:                  false
                        content: FeeInput {
                            id:                         receiveFeeInput
                            currency:                   viewModel.receiveCurrency
                            minFee:                     BeamGlobals.getMinimalFee(viewModel.receiveCurrency, false)
                            recommendedFee:             BeamGlobals.getRecommendedFee(viewModel.receiveCurrency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(viewModel.receiveCurrency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         receiveAmountInput.showSecondCurrency
                            isExchangeRateAvailable:    receiveAmountInput.isExchangeRateAvailable
                            rateAmount:                 receiveAmountInput.getFeeInSecondCurrency(viewModel.receiveFee)
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

                                property bool showEstimatedFee: viewModel.receiveCurrency != Currency.CurrBeam

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    text:                   receiveAmountInput.getTotalFeeTitle() + ":"
                                    visible:                parent.showEstimatedFee
                                }
    
                                SFText {
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             BeamGlobals.calcTotalFee(viewModel.receiveCurrency, viewModel.receiveFee)
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
                                        ? ["1", sendAmountInput.currencyUnitName, "=", Utils.uiStringToLocale(viewModel.rate), receiveAmountInput.currencyUnitName].join(" ")
                                        : ["1", receiveAmountInput.currencyUnitName, "=", Utils.uiStringToLocale(viewModel.rate), sendAmountInput.currencyUnitName].join(" ")
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
                                        //% "Show token"
                                        text: qsTrId("show-token")
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

                    var unitName = BeamGlobals.getCurrencyUnitName(viewModel.sendCurrency)
                    var onlineMessage =
                        //% "Keep your wallet online. The swap normally takes about 1 hour to complete."
                        qsTrId("send-swap-sconfirmation-online-time") + (viewModel.sendCurrency !== Currency.CurrBeam ?
                        //% " Once the offer is accepted by the other side, the %1 transaction fee will be charged even if the offer is cancelled."
                        qsTrId("send-swap-fee-warning").arg(unitName)
                        : "")

                    const dialogComponent = Qt.createComponent("send_confirm.qml");
                    var dialogObject = dialogComponent.createObject(sendSwapView,
                        {
                            swapMode:    true,
                            addressText: viewModel.receiverAddress,
                            typeText:    qsTrId("general-swap"),
                            amount:      viewModel.sendAmount,
                            unitName:    unitName,
                            fee:         viewModel.sendFee,
                            flatFee:     viewModel.sendCurrency == Currency.CurrBeam,
                            onAccepted:  acceptedCallback,
                            rate:        viewModel.secondCurrencySendRateValue,
                            rateUnit:    viewModel.secondCurrencyUnitName,
                        }).open();

                    function acceptedCallback() {
                        viewModel.sendMoney();
                        sendSwapView.onAccepted();
                    }
                }
            }
        }  // ColumnLayout
    }  // ScrollView
} // ColumnLayout