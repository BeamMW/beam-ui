import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: sendRegularView

    property var defaultFocusItem: receiverTAInput
    spacing: 0
    // callbacks set by parent
    property var onAccepted:        undefined
    property var onClosed:          undefined
    property var onSwapToken:       undefined
    property alias receiverAddress: viewModel.receiverTA

    readonly property bool showInsufficientBalanceWarning:
        !viewModel.isEnough &&
        !(viewModel.isZeroBalance && (viewModel.sendAmount == "" || viewModel.sendAmount == "0"))  // not shown if available is 0 and no value entered to send

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    SendViewModel {
        id: viewModel

        onSendMoneyVerified: {
            onAccepted();
        }

        onCantSendToExpired: {
            Qt.createComponent("send_expired.qml")
                .createObject(sendRegularView)
                .open();
        }
    }

    TokenInfoDialog {
        id:         tokenInfoDialog
        token:      viewModel.receiverTA
        incoming:   false
    }

    SaveAddressDialog {
        id:     saveAddressDialog
        //% "Do you want to name the contact?"
        dialogTitle:  qsTrId("save-address-title")
        //% "No name"
        text:         qsTrId("save-address-no-name")

        onAccepted: {
            viewModel.saveReceiverAddress(text);
            viewModel.sendMoney();
        }
        onRejected: {
            viewModel.sendMoney();
        }
    }

    function isTAInputValid() {
        return viewModel.receiverTA.length == 0 || viewModel.receiverTAValid
    }

    function getFeeInSecondCurrency(feeValue) {
        return Utils.formatAmountToSecondCurrency(
            feeValue,
            viewModel.secondCurrencyRateValue ,
            viewModel.secondCurrencyLabel)
    }

    //
    // Title row
    //
    Item {
        Layout.fillWidth:    true
        Layout.topMargin:    100 // 101
        Layout.bottomMargin: 30  // 31
        CustomButton {
            anchors.left:   parent.left
            anchors.verticalCenter: parent.verticalCenter
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true
            //% "Back"
            text:           qsTrId("general-back")
            icon.source:    "qrc:/assets/icon-back.svg"
            onClicked:      onClosed();
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
            //% "Send"
            text:               qsTrId("send-title")
        }
    }

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
                    // Transaction info
                    //
                    Panel {
                        //% "Transaction info"
                        title:                   qsTrId("general-transaction-info")
                        Layout.fillWidth:        true
                        content: 
                        ColumnLayout {
                            spacing: 0

                            SFTextInput {
                                Layout.fillWidth: true
                                id:               receiverTAInput
                                font.pixelSize:   14
                                color:            isTAInputValid() ? Style.content_main : Style.validator_error
                                backgroundColor:  isTAInputValid() ? Style.content_main : Style.validator_error
                                font.italic :     !isTAInputValid()
                                text:             viewModel.receiverTA
                                validator:        RegExpValidator { regExp: /[0-9a-zA-Z]{1,}/ }
                                selectByMouse:    true
                                visible:          !receiverTAText.visible
                                property bool isSwap: BeamGlobals.isSwapToken(text)
                                placeholderText:  isSwap ?
                                    //% "Paste recipient token here"
                                    qsTrId("send-contact-token-placeholder") :
                                    //% "Paste recipient address here"
                                    qsTrId("send-contact-address-placeholder")
                                onTextChanged: {
                                    if (isSwap && typeof onSwapToken == "function") {
                                        onSwapToken(text);
                                    }
                                }
                            }
                            RowLayout {
                                id:                 receiverTAText
                                Layout.fillWidth:     true
                                Layout.leftMargin:    0
                                Layout.rightMargin:   6
                                Layout.topMargin:     6
                                Layout.bottomMargin:  3
                                spacing:              0
                                visible:              !receiverTAInput.activeFocus && viewModel.receiverTAValid
                                SFText {
                                    id:                 receiverTAPlaceholder
                                    Layout.fillWidth:   true
                                    font.pixelSize:     14
                                    color:              Style.content_main
                                    text:               viewModel.receiverTA
                                    elide:              Text.ElideMiddle
                                    wrapMode:           Text.NoWrap
                                    rightPadding:       160
                                    activeFocusOnTab:   true
                                    onActiveFocusChanged: {
                                        if (activeFocus)
                                            receiverTAInput.forceActiveFocus();
                                    }
                                    MouseArea {
                                        property bool   hovered: false
                                        id:             receiverTAPlaceholderMA
                                        anchors.fill:   parent
                                        hoverEnabled:   true
                                        acceptedButtons: Qt.LeftButton
                                        onPressed: {
                                            receiverTAInput.forceActiveFocus();
                                        }
                                        onEntered: {
                                            hovered = true
                                        }
                                        onExited: {
                                            hovered = false
                                        }
                                    }
                                }
                                LinkButton {
                                    //% "More details"
                                    text:       qsTrId("more-details")
                                    linkColor:  Style.accent_outgoing
                                    visible:    viewModel.receiverTAValid
                                    onClicked: {
                                        tokenInfoDialog.open();
                                    }
                                }
                            }
                            Rectangle {
                                id:                 receiverTAUnderline
                                Layout.fillWidth:   true
                                Layout.bottomMargin:2
                                height:             1
                                color:              receiverTAInput.backgroundColor
                                visible:            receiverTAText.visible
                                opacity:            (receiverTAPlaceholder.activeFocus || receiverTAPlaceholderMA.hovered)? 0.3 : 0.1
                            }

                            SFText {
                                property bool isTokenOrAddressValid: !isTAInputValid()
                                Layout.alignment: Qt.AlignTop
                                Layout.fillWidth: true
                                id:               receiverTAError
                                color:            isTokenOrAddressValid ? Style.validator_error : Style.content_secondary
                                font.italic:      !isTokenOrAddressValid
                                font.pixelSize:   12
                                wrapMode:         Text.Wrap
                                text:             isTokenOrAddressValid
                                       //% "Invalid wallet address"
                                      ? qsTrId("wallet-send-invalid-address-or-token")
                                      : viewModel.tokenGeneratebByNewAppVersionMessage
                                visible:          isTokenOrAddressValid || viewModel.isTokenGeneratebByNewAppVersion
                            }
                    
                            Binding {
                                target:   viewModel
                                property: "receiverTA"
                                value:    receiverTAInput.text
                            }

                            SFText {
                                Layout.alignment:   Qt.AlignTop
                                Layout.topMargin:   10
                                Layout.fillWidth:   true
                                id:                 addressNote
                                color:              Style.content_secondary
                                font.italic:        true
                                font.pixelSize:     14
                                wrapMode:           Text.Wrap
                                text:               viewModel.isPermanentAddress ? 
                                                    //% "Permanent address"
                                                    qsTrId("wallet-send-permanent-note") 
                                                    :
                                                    //% "One-time use address (expire in 12 hours after succesfull transaction)."
                                                    qsTrId("wallet-send-one-time-note")
                                visible:            viewModel.isToken && !viewModel.isShieldedTx
                            }

                            SFText {
                                Layout.alignment:   Qt.AlignTop
                                Layout.topMargin:   10
                                Layout.fillWidth:   true
                                id:                 maxPrivacyNoteToken
                                color:              Style.content_secondary
                                font.italic:        true
                                font.pixelSize:     14
                                wrapMode:           Text.Wrap
                                text:               viewModel.isOffline ? 
                                                    //% "Offline address. Payments left: %1"
                                                    qsTrId("wallet-send-max-privacy-note-address-offline").arg(viewModel.offlinePayments)
                                                    : 
                                                        viewModel.isMaxPrivacy ?
                                                        //% "Max privacy address"
                                                        qsTrId("wallet-send-max-privacy-note-address")
                                                        : viewModel.isPublicOffline ? 
                                                            //% "Public offline address"
                                                            qsTrId("wallet-send-public-offline-address")
                                                            :""
                                visible:            viewModel.isShieldedTx && viewModel.isToken// && !viewModel.isOwnAddress
                            }

                            SFText {
                                height: 16
                                Layout.alignment:   Qt.AlignTop
                                Layout.topMargin:   10
                                Layout.fillWidth:   true
                                id:                 needExtractShieldedNote
                                color:              Style.content_secondary
                                font.italic:        true
                                font.pixelSize:     14
                                wrapMode:           Text.Wrap
                                //% "Transaction is slower, fees are higher."
                                text:               qsTrId("wallet-send-need-extract-shielded-note")
                                visible:            viewModel.isNeedExtractShieldedCoins && !viewModel.isShieldedTx
                            }
                        }
                    }

                    //
                    // Amount
                    //
                    Panel {
                        //% "Amount"
                        title:                   qsTrId("general-amount")
                        Layout.fillWidth:        true

                        content: AmountInput {
                            id:                         sendAmountInput
                            amountIn:                   viewModel.sendAmount
                            secondCurrencyRateValue:    viewModel.secondCurrencyRateValue
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            setMaxAvailableAmount:      function() {
                                viewModel.setMaxAvailableAmount();
                            }
                            showAddAll:                 true
                            color:                      Style.accent_outgoing
                            error:                      showInsufficientBalanceWarning
                                                        //% "Insufficient funds: you would need %1 to complete the transaction"
                                                        ? qsTrId("send-founds-fail").arg(Utils.uiStringToLocale(viewModel.missing))
                                                        //% "Max privacy coins are selected therefore the maximum amount is %1."
                                                        : (viewModel.canSendByOneTransaction ? "" : qsTrId("send-founds-fail-by-one-tx").arg(Utils.uiStringToLocale(viewModel.maxSendAmount)))
                        }
     
                        Binding {
                            target:   viewModel
                            property: "sendAmount"
                            value:    sendAmountInput.amount
                        }
                    }

                    //
                    // Fee
                    //
                    FoldablePanel {
                        id: foldableFee
                        //% "Fee"
                        title:                   qsTrId("send-regular-fee")
                        Layout.fillWidth:        true

                        content: FeeInput {
                            id:                         feeInput
                            fee:                        viewModel.feeGrothes
                            minFee:                     viewModel.minimalFeeGrothes
                            feeLabel:                   BeamGlobals.getFeeRateLabel(Currency.CurrBeam)
                            color:                      Style.accent_outgoing
                            readOnly:                   false
                            fillWidth:                  true
                            showSecondCurrency:         sendAmountInput.showSecondCurrency
                            isExchangeRateAvailable:    sendAmountInput.isExchangeRateAvailable
                            secondCurrencyAmount:       getFeeInSecondCurrency(viewModel.fee)
                            secondCurrencyLabel:        viewModel.secondCurrencyLabel
                            minimumFeeNotificationText: viewModel.isShieldedTx || viewModel.isNeedExtractShieldedCoins ?
                                //% "For the best privacy Max privacy coins were selected. Min transaction fee is %1 %2"
                                qsTrId("max-pivacy-fee-fail").arg(Utils.uiStringToLocale(minFee)).arg(feeLabel) :
                                ""
                        }

                        Binding {
                            target:   viewModel
                            property: "feeGrothes"
                            value:    feeInput.fee
                        }

                        Connections {
                            target: viewModel
                            onFeeGrothesChanged: {
                                feeInput.fee = viewModel.feeGrothes;
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
                                id:               addressComment
                                font.pixelSize:   14
                                Layout.fillWidth: true
                                //focus:            true
                                color:            Style.content_main
                                text:             viewModel.comment
                                maximumLength:    BeamGlobals.maxCommentLength()
                                //% "Comments are local and won't be shared"
                                placeholderText:  qsTrId("general-comment-local")
                            }
                 
                            Binding {
                                target:   viewModel
                                property: "comment"
                                value:    addressComment.text
                            }
                        }
                    }
                }

                //
                // Right column
                //
                ColumnLayout {
                    Layout.alignment:   Qt.AlignTop
                    Layout.fillWidth:   true
                    Layout.preferredWidth: 400
                    spacing:            10

                    Pane {
                        Layout.fillWidth:        true
                        padding:                 20

                        background: Rectangle {
                            radius: 10
                            color:  Style.background_button
                        }

                        GridLayout {
                            anchors.fill:        parent
                            columnSpacing:       20
                            rowSpacing:          14
                            columns:             2

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                Layout.fillWidth:       true
                                font.pixelSize:         14
                                color:                  Style.content_secondary
                                //% "Amount to send"
                                text:                   qsTrId("send-amount-label") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:        Qt.AlignTop
                                Layout.fillWidth:        true
                                error:                   showInsufficientBalanceWarning
                                amount:                  viewModel.sendAmount
                                lightFont:               false
                                currencySymbol:          BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                                secondCurrencyLabel:     viewModel.secondCurrencyLabel
                                secondCurrencyRateValue: viewModel.secondCurrencyRateValue
                            }
                    
                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                Layout.fillWidth:       true
                                font.pixelSize:         14
                                color:                  Style.content_secondary
                                text:                   qsTrId("general-change") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:        Qt.AlignTop
                                Layout.fillWidth:        true
                                error:                   showInsufficientBalanceWarning
                                amount:                  viewModel.change
                                lightFont:               false
                                currencySymbol:          BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                                secondCurrencyLabel:     viewModel.secondCurrencyLabel
                                secondCurrencyRateValue: viewModel.secondCurrencyRateValue
                            }

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                Layout.fillWidth:       true
                                font.pixelSize:         14
                                color:                  Style.content_secondary
                                text:                   qsTrId("send-regular-fee") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:        Qt.AlignTop
                                Layout.fillWidth:        true
                                error:                   showInsufficientBalanceWarning
                                amount:                  viewModel.fee
                                lightFont:               false
                                currencySymbol:          BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                                secondCurrencyLabel:     viewModel.secondCurrencyLabel
                                secondCurrencyRateValue: viewModel.secondCurrencyRateValue
                            }

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                Layout.fillWidth:       true
                                font.pixelSize:         14
                                color:                  Style.content_secondary
                                //% "Remaining"
                                text:                   qsTrId("send-remaining-label") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:        Qt.AlignTop
                                Layout.fillWidth:        true
                                error:                   showInsufficientBalanceWarning
                                amount:                  viewModel.available
                                lightFont:               false
                                currencySymbol:          BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                                secondCurrencyLabel:     viewModel.secondCurrencyLabel
                                secondCurrencyRateValue: viewModel.secondCurrencyRateValue
                            }
                        }
                    }
                }
            }

            //
            // Footers
            //
            CustomButton {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                //% "Send"
                text:                qsTrId("general-send")
                palette.buttonText:  Style.content_opposite
                palette.button:      Style.accent_outgoing
                icon.source:         "qrc:/assets/icon-send-blue.svg"
                enabled:             viewModel.canSend
                onClicked: {                
                    const dialogComponent = Qt.createComponent("send_confirm.qml");
                    const dialogObject = dialogComponent.createObject(sendRegularView,
                        {
                            addressText: viewModel.receiverTA,
                            typeText: viewModel.isShieldedTx ? 
                                    viewModel.isMaxPrivacy ? qsTrId("tx-max-privacy") 
                                        : viewModel.isOffline ? qsTrId("tx-address-offline") 
                                            //% "Public offline"
                                            : viewModel.isPublicOffline ? qsTrId("tx-address-public-offline") 
                                                //% "Unknown"
                                                : qsTrId("tx-address-unknown")
                                    : qsTrId("tx-regular"),
                            isOnline:   !viewModel.isShieldedTx,
                            currency: Currency.CurrBeam,
                            amount: viewModel.sendAmount,
                            fee: viewModel.feeGrothes,
                            onAcceptedCallback: acceptedCallback,
                            secondCurrencyRate: viewModel.secondCurrencyRateValue,
                            secondCurrencyLabel: viewModel.secondCurrencyLabel
                        }).open();

                    function acceptedCallback() {
                        if (viewModel.isPermanentAddress && !viewModel.hasAddress) {
                            // TODO: uncomment when UX will be ready
                            //saveAddressDialog.open();
                            //viewModel.saveReceiverAddress(viewModel.comment);
                            viewModel.sendMoney();
                        } else {
                            viewModel.sendMoney();
                        }
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }  // ColumnLayout
    }  // ScrollView
} // ColumnLayout
