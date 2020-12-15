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
    id: thisView

    property var  defaultFocusItem: sentAmountInput.amountInput
    property bool addressSaved: false
    property var locale: Qt.locale()

    // callbacks set by parent
    property var onClosed: undefined

    TopGradient {
        mainRoot: main
        topColor: Style.accent_incoming
    }

    ReceiveSwapViewModel {
        id: viewModel
        onNewAddressFailed: {
            thisView.enabled = true
            Qt.createComponent("receive_addrfail.qml")
                .createObject(sendView)
                .open();
        }
    }

    SwapTokenInfoDialog {
        id:               tokenInfoDialog
        token:            viewModel.transactionToken
        expirationTime:   expiresCombo.displayText
        rate:             rateStart.text + " " + rateInput.text + " " + rateEnd.text
        copyTokenEnabled: false
    }

    function isValid () {
        if (!viewModel.commentValid) return false;
        if (viewModel.receiveCurrency == viewModel.sentCurrency) return false;
        return receiveAmountInput.isValid && sentAmountInput.isValid && viewModel.isEnough &&
               receiveFeeInput.isValid && sendFeeInput.isValid && !currencyError() && rateRow.rateValid;
    }

    function canSend () {
        if (!isValid()) return false;
        if (parseFloat(viewModel.amountToReceive) <= 0 || parseFloat(viewModel.amountSent) <= 0) return false;
        return true;
    }

    function currencyError() {
        if (viewModel.receiveCurrency == viewModel.sentCurrency) return true;
        if (viewModel.receiveCurrency != Currency.CurrBeam && viewModel.sentCurrency != Currency.CurrBeam) return true;
        return false;
    }

    function saveAddress() {
        if (!thisView.addressSaved) {
            thisView.addressSaved = true
            viewModel.saveAddress()
        }
    }

    Component.onCompleted: {
        if (!BeamGlobals.canSwap()) swapna.open();
    }

    SwapNADialog {
        id: swapna
        onRejected: thisView.onClosed()
        onAccepted: main.openSwapSettings()
/*% "You do not have any 3rd-party currencies connected.
Update your settings and try again."
*/
        text:       qsTrId("swap-na-message")
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
            //% "Create a Swap Offer"
            text:               qsTrId("wallet-receive-swap-title")
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
                    Layout.rightMargin:     -5
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
                            id:                         sentAmountInput
                            color:                      Style.accent_outgoing
//                            hasFee:           true
                            currFeeTitle:       true
                            currency:                   viewModel.sentCurrency
                            amountIn:                   viewModel.amountSent
                            secondCurrencyRateValue:    viewModel.secondCurrencySendRateValue
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            multi:                      true
                            resetAmount:                false
                            currColor:                  currencyError() || !BeamGlobals.canReceive(currency) ? Style.validator_error : Style.content_main
                            error:                      getErrorText()

                            function getErrorText() {
                                if(!BeamGlobals.canReceive(currency)) {
/*% "%1 is not connected, 
please review your settings and try again"
*/
                                    return qsTrId("swap-currency-na-message").arg(BeamGlobals.getCurrencyName(currency)).replace("\n", "")
                                }
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

                            onCurrencyChanged: {
                                if(sentAmountInput.currency != Currency.CurrBeam &&
                                   receiveAmountInput.currency != Currency.CurrBeam) {
                                    receiveAmountInput.currency = Currency.CurrBeam
                                }
                            }
                        }

                        Binding {
                            target:   viewModel
                            property: "amountSent"
                            value:    sentAmountInput.amount
                        }

                        Binding {
                            target:   viewModel
                            property: "sentCurrency"
                            value:    sentAmountInput.currency
                        }
                    }

                    //
                    // Send Fee
                    //
                    FoldablePanel {
                        title:                   sentAmountInput.getFeeTitle()
                        Layout.fillWidth:        true
                        folded:                  false
                        content: FeeInput {
                            id:                         sendFeeInput
                            currency:                   viewModel.sentCurrency
                            minFee:                     currency == Currency.CurrBeam ? viewModel.minimalBeamFeeGrothes : BeamGlobals.getMinimalFee(currency, false)
                            recommendedFee:             BeamGlobals.getRecommendedFee(currency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(currency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         sentAmountInput.showSecondCurrency
                            isExchangeRateAvailable:    sentAmountInput.isExchangeRateAvailable
                            secondCurrencyAmount:       sentAmountInput.getFeeInSecondCurrency(viewModel.sentFee)
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            //minimumFeeNotificationText: viewModel.isShieldedTx ?
                            //    //% "For the best privacy Max privacy coins were selected. Min transaction fee is %1 %2"
                            //    qsTrId("max-pivacy-fee-fail").arg(Utils.uiStringToLocale(minFee)).arg(feeLabel) :
                            //    ""
                        }

                        Binding {
                            target:   viewModel
                            property: "sentFee"
                            value:    sendFeeInput.fee
                        }

                        Connections {
                            target: viewModel
                            onSentFeeChanged: sendFeeInput.fee = viewModel.sentFee
                        }
                    }

                    //
                    // Offer expiration time
                    //
                    FoldablePanel {
                        //% "Offer expiration time"
                        title:                   qsTrId("wallet-receive-offer-expires-label")
                        Layout.fillWidth:        true
                        folded:                  false
                        content: ColumnLayout {
                            CustomComboBox {
                                id:                  expiresCombo
                                Layout.fillWidth:    true
                                currentIndex:        viewModel.offerExpires
                                fontPixelSize:       14
                                model: [
                                    //% "30 minutes"
                                    qsTrId("wallet-receive-expires-30m"),
                                    //% "1 hour"
                                    qsTrId("wallet-receive-expires-1"),
                                    //% "2 hours"
                                    qsTrId("wallet-receive-expires-2"),
                                    //% "6 hours"
                                    qsTrId("wallet-receive-expires-6"),
                                    //% "12 hours"
                                    qsTrId("wallet-receive-expires-12")
                                ]
                            }
        
                            Binding {
                                target:   viewModel
                                property: "offerExpires"
                                value:    expiresCombo.currentIndex
                            }
                        }
                    }

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title:                   qsTrId("general-comment")
                        Layout.fillWidth:        true
                        content: ColumnLayout {
                        SFTextInput {
                                id:               addressComment
                                font.pixelSize:   14
                                Layout.fillWidth: true
                                font.italic :     !viewModel.commentValid
                                backgroundColor:  viewModel.commentValid ? Style.content_main : Style.validator_error
                                color:            viewModel.commentValid ? Style.content_main : Style.validator_error
                                focus:            true
                                text:             viewModel.addressComment
                                maximumLength:    BeamGlobals.maxCommentLength()
                                enabled:          !thisView.addressSaved
                                //% "Comments are local and won't be shared"
                                placeholderText:  qsTrId("general-comment-local")
                            }

                            Binding {
                                target:   viewModel
                                property: "addressComment"
                                value:    addressComment.text
                            }

                            Item {
                                Layout.fillWidth: true
                                SFText {
                                    //% "Address with the same comment already exists"
                                    text:           qsTrId("general-addr-comment-error")
                                    color:          Style.validator_error
                                    font.pixelSize: 12
                                    font.italic:    true
                                    visible:        !viewModel.commentValid
                                }
                            }
                        }
                    }
                } // ColumnLayout

                //
                // Middle column
                //
                Rectangle {
                    Layout.alignment:       Qt.AlignTop
                    Layout.topMargin:       60
                    Layout.leftMargin:      -12
                    Layout.rightMargin:     -12
                    Layout.preferredHeight: 24
                    Layout.preferredWidth:  24
                    color:                  Style.background_main
                    z:                      1
                    radius:                 12
                    SvgImage {                 
                        Layout.maximumHeight: 24
                        Layout.maximumWidth:  24
                        source: "qrc:/assets/icon-swap-currencies.svg"
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                var rateWasInFocus = false;
                                var rate = rateInput.rate;
                                if (rateInput.focus) {
                                    rateWasInFocus = true;
                                    rateInput.focus = false;
                                }
                                var sentCurency = sentAmountInput.currency;
                                sentAmountInput.currency = receiveAmountInput.currency;
                                receiveAmountInput.currency = sentCurency;
                                if (rateWasInFocus) {
                                    rateInput.focus = true;
                                    rateInput.text = rate;
                                    rateInput.onTextEdited();
                                }
                                rateRow.checkReceive();
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
                    Layout.leftMargin:      -5
                    spacing:                10
                    //
                    // Receive amount
                    //
                    Panel {
                        //% "Receive amount"
                        title:                   qsTrId("receive-amount-swap-label")
                        Layout.fillWidth:        true
                        AmountInput {
                            id:                         receiveAmountInput
                            //hasFee:                     true
                            currFeeTitle:               true
                            currency:                   viewModel.receiveCurrency
                            amountIn:                   viewModel.amountToReceive
                            secondCurrencyRateValue:    viewModel.secondCurrencyReceiveRateValue
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            multi:                      true
                            resetAmount:                false
                            currColor:                  currencyError() || !BeamGlobals.canReceive(currency) ? Style.validator_error : Style.content_main
                            error:                      getErrorText()
                            showTotalFee:               true
                            showSecondCurrency:         sentAmountInput.showSecondCurrency

                            function getErrorText() {
                                if(!BeamGlobals.canReceive(currency)) {
/*% "%1 is not connected, 
please review your settings and try again"
*/
                                    return qsTrId("swap-currency-na-message").arg(BeamGlobals.getCurrencyName(currency)).replace("\n", "")
                                }
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

                            onCurrencyChanged: {
                                if(receiveAmountInput.currency != Currency.CurrBeam &&
                                   sentAmountInput.currency != Currency.CurrBeam) {
                                    sentAmountInput.currency = Currency.CurrBeam
                                }
                            }
                        }

                        Binding {
                            target:   viewModel
                            property: "amountToReceive"
                            value:    receiveAmountInput.amount
                        }

                        Binding {
                            target:   viewModel
                            property: "receiveCurrency"
                            value:    receiveAmountInput.currency
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
                            minFee:                     BeamGlobals.getMinimalFee(currency, false)
                            maxFee:                     BeamGlobals.getMaximumFee(currency)
                            recommendedFee:             BeamGlobals.getRecommendedFee(currency)
                            feeLabel:                   BeamGlobals.getFeeRateLabel(currency)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         receiveAmountInput.showSecondCurrency
                            isExchangeRateAvailable:    receiveAmountInput.isExchangeRateAvailable
                            secondCurrencyAmount:       receiveAmountInput.getFeeInSecondCurrency(viewModel.receiveFee)
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            //minimumFeeNotificationText: viewModel.isShieldedTx ?
                            //    //% "For the best privacy Max privacy coins were selected. Min transaction fee is %1 %2"
                            //    qsTrId("max-pivacy-fee-fail").arg(Utils.uiStringToLocale(minFee)).arg(feeLabel) :
                            //    ""
                        }

                        Binding {
                            target:   viewModel
                            property: "receiveFee"
                            value:    receiveFeeInput.fee
                        }

                        Connections {
                            target: viewModel
                            onReceiveFeeChanged: receiveFeeInput.fee = viewModel.receiveFee
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
                                    text:             BeamGlobals.calcWithdrawTxFee(viewModel.receiveCurrency, viewModel.receiveFee)
                                    visible:          parent.showEstimatedFee
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Exchange rate"
                                    text:                   qsTrId("general-rate") + ":"
                                }

                                RowLayout {
                                    id:                     rateRow
                                    Layout.fillWidth:       true

                                    property double maxAmount: parseFloat(Utils.maxAmount)
                                    property double minAmount: parseFloat(Utils.minAmount)
                                    property bool rateValid:   true

                                    function changeRate() {
                                        if (!rateInput.activeFocus) {
                                            rateInput.rate = viewModel.rate;
                                            rateInput.text = rateInput.rate == "0" ? "" : Utils.uiStringToLocale(rateInput.rate);
                                            rateRow.checkIsRateValid();
                                        }
                                    }

                                    function checkReceive() {
                                        receiveAmountInput.amountInput.onTextChanged();
                                        if (parseFloat(receiveAmountInput.amount) >= rateRow.maxAmount) {
                                            if (receiveAmountInput.currency == Currency.CurrBeam) {
                                                //% "Amount overtop total Beam supply."
                                                receiveAmountInput.error = qsTrId("overtop-beam-supply");
                                            }
                                        }
                                    }

                                    function changeReceive() {
                                        var rateValue =
                                            parseFloat(Utils.localeDecimalToCString(rateInput.rate)) || 0;
                                        if (sentAmountInput.amount != "0" && rateValue) {
                                            receiveAmountInput.amount = viewModel.isSendBeam
                                                ? BeamGlobals.multiplyWithPrecision8(sentAmountInput.amount, rateValue)
                                                : BeamGlobals.divideWithPrecision8(sentAmountInput.amount, rateValue);
                                            checkReceive();
                                        } else if (!rateValue) {
                                            receiveAmountInput.amount = "0";
                                        }
                                        checkIsRateValid();
                                    }

                                    function checkIsRateValid() {

                                        var rate = parseFloat(Utils.localeDecimalToCString(rateInput.rate)) || 0;
                                        if (rate == 0 ||
                                            receiveAmountInput.amount == "0") {
                                            rateValid = true;
                                            return;
                                        }
                                        if (receiveAmountInput.currency == Currency.CurrBeam) {
                                            rateValid =
                                                parseFloat(receiveAmountInput.amount) <= rateRow.maxAmount &&
                                                parseFloat(receiveAmountInput.amount) >= rateRow.minAmount;
                                        } else {
                                            rateValid =
                                                parseFloat(receiveAmountInput.amount) >= rateRow.minAmount
                                        }
                                    }

                                    SFText {
                                        id:               rateStart
                                        font.pixelSize:   14
                                        color:            rateRow.rateValid ? Style.content_main : Style.validator_error
                                        text:             viewModel.isSendBeam
                                            ? ["1", sentAmountInput.currencyLabel, "="].join(" ")
                                            : ["1", receiveAmountInput.currencyLabel, "="].join(" ") 
                                    }

                                    SFTextInput {
                                        property string rate: "0"

                                        id:                  rateInput
                                        padding:             0
                                        Layout.minimumWidth: 35
                                        activeFocusOnTab:    true
                                        font.pixelSize:      14
                                        color:               rateRow.rateValid ? Style.content_main : Style.validator_error
                                        backgroundColor:     rateRow.rateValid ? Style.content_main : Style.validator_error
                                        text:                ""
                                        selectByMouse:       true
                                        maximumLength:       30
                                    
                                        validator: DoubleValidator {
                                            bottom: rateRow.minAmount
                                            top: rateRow.maxAmount
                                            decimals: 8
                                            locale: locale.name
                                            notation: DoubleValidator.StandardNotation
                                        }

                                        onActiveFocusChanged: {
                                            text = rate == "0" ? "" : (rateInput.activeFocus ? rate : Utils.uiStringToLocale(Utils.localeDecimalToCString(rate)));
                                        }

                                        onTextEdited: {
                                            if (rateInput.activeFocus) {
                                                if (text.match("^00*$")) {
                                                    text = "0";
                                                }

                                                var value = text ? text.split(locale.groupSeparator).join('') : "0";
                                                var parts = value.split(locale.decimalPoint);
                                                var left = (parseInt(parts[0], 10) || 0).toString();
                                                rate = parts[1] ? [left, parts[1]].join(locale.decimalPoint) : left;
                                                if (!parseFloat(Utils.localeDecimalToCString(rate))) {
                                                    rate = "0";
                                                }
                                                rateRow.changeReceive();
                                            }
                                        }

                                        Component.onCompleted: {
                                            viewModel.amountSentChanged.connect(rateRow.changeRate);
                                            viewModel.rateChanged.connect(rateRow.changeRate);
                                        }
                                    }

                                    SFText {
                                        id:               rateEnd
                                        font.pixelSize:   14
                                        color:            rateRow.rateValid ? Style.content_main : Style.validator_error
                                        text:             viewModel.isSendBeam ? receiveAmountInput.currencyLabel : sentAmountInput.currencyLabel
                                    }
                                    Item {
                                        Layout.leftMargin: rateInput.x
                                        SFText {
                                            color:               Style.validator_error
                                            font.pixelSize:      12
                                            font.styleName:      "Italic"
                                            width:               parent.width
                                            //% "Invalid rate"
                                            text:                qsTrId("swap-invalid-rate")
                                            visible:             !rateRow.rateValid
                                        }
                                    }
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Swap token"
                                    text:                   qsTrId("send-swap-token") + ":"
                                }

                                ColumnLayout {
                                    spacing: 11
                                    RowLayout {
                                        Layout.fillWidth:        true
                                        Layout.rightMargin:      showTokenLink.visible ? 0 : 40
                                        SFLabel {
                                            id:                  tokenLabel
                                            Layout.fillWidth:    true
                                            font.pixelSize:      14
                                            color:               Style.content_main
                                            elide:               Text.ElideMiddle
                                            text:                viewModel.transactionToken
                                        }

                                        LinkButton {
                                            id: showTokenLink
                                            //% "Show token"
                                            text: qsTrId("show-token")
                                            linkColor: Style.accent_incoming
                                            visible:  thisView.canSend()
                                            onClicked: {
                                                tokenInfoDialog.open();
                                            }
                                        }
                                    }

                                    CustomButton {
                                        //% "copy and close"
                                        text:                qsTrId("wallet-receive-swap-copy-and-close")
                                        palette.buttonText:  Style.content_main
                                        iconColor:           Style.content_main
                                        palette.button:      Style.background_button
                                        icon.source:         "qrc:/assets/icon-copy.svg"
                                        enabled:             thisView.canSend()
                                        onClicked: {
                                            BeamGlobals.copyToClipboard(viewModel.transactionToken);
                                            thisView.saveAddress();
                                            viewModel.startListen();
                                            onClosed();
                                        }
                                    }
                                }
                            }
                        }
                    }
                } // ColumnLayout
            } // RowLayout

            //
            // Footers
            //
            RowLayout {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                spacing:             30

                CustomButton {
                    //% "publish offer"
                    text:                qsTrId("wallet-receive-swap-publish")
                    palette.buttonText:  Style.content_opposite
                    icon.color:          Style.content_opposite
                    palette.button:      Style.active
                    icon.source:         "qrc:/assets/icon-share.svg"
                    enabled:             thisView.canSend()
                    onClicked: {
                        thisView.saveAddress();
                        viewModel.startListen();
                        viewModel.publishToken();
                        onClosed();
                    }
                }
            } // RowLayout
        }  // ColumnLayout
    }  // ScrollView
}
