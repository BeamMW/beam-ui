import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "controls"

ColumnLayout {
    id: receiveView
    property var defaultFocusItem: null//addressComment

    // callbacks set by parent
    property var onClosed: function() {}

    TopGradient {
        mainRoot: main
        topColor: Style.accent_incoming
    }

    ReceiveViewModel {
        id: viewModel
        onNewAddressFailed: {
            var popup = Qt.createComponent("popup_message.qml")
                .createObject(receiveView)

            //% "You cannot generate new address. Your wallet doesn't have a master key."
            popup.message = qsTrId("can-not-generate-new-address-message")
            popup.open()
        }
    }

    function isValid() {
        return viewModel.commentValid
    }

    function saveAddress() {
        if (receiveView.isValid()) viewModel.saveAddress();
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
            //% "Receive"
            text:               qsTrId("wallet-receive-title")
        }
    }

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
            Layout.alignment:   Qt.AlignTop
            Layout.fillWidth:   true
            Layout.preferredWidth: 400
            spacing:            10
            Panel {
                //% "Transaction info"
                title:                   qsTrId("general-transaction-info")
                Layout.fillWidth:        true
                content: 
                ColumnLayout {
                    spacing: 20
                    
                    Pane {
                        padding:    2
                        background: Rectangle {
                            color: Style.table_header
                            radius: 10
                            border.width: 1
                            border.color: Style.active
                        }
                        ButtonGroup { id: txTypeGroup }
                        RowLayout {
                            spacing: 0
                            CustomButton {
                                Layout.preferredHeight: 18
                                id:                 regularCheck
                                //% "Regular"
                                text:               qsTrId("tx-regular")
                                palette.buttonText: Style.content_main
                                ButtonGroup.group:  txTypeGroup
                                checkable:          true
                                checked: !viewModel.isShieldedTx
                                onToggled: {
                                    viewModel.isShieldedTx = false;
                                    viewModel.isNonInteractive= false;
                                }
                            }
                            CustomButton {
                                Layout.preferredHeight: 18
                                id:                 maxPrivacyCheck
                                //% "Max privacy"
                                text:               qsTrId("tx-max-privacy")
                                palette.buttonText: Style.content_main
                                ButtonGroup.group:  txTypeGroup
                                checkable:          true
                                checked: viewModel.isShieldedTx && !viewModel.isNonInteractive
                                onToggled: {
                                    viewModel.isShieldedTx = true;
                                    viewModel.isNonInteractive= false;
                                }
                            }
                            CustomButton {
                                Layout.preferredHeight: 18
                                //% "Non-interactive"
                                text:               qsTrId("tx-non-interactive")
                                palette.buttonText: Style.content_main
                                ButtonGroup.group:  txTypeGroup
                                checkable:          true
                                checked:            viewModel.isShieldedTx && viewModel.isNonInteractive
                                onToggled: {
                                    viewModel.isShieldedTx = true;
                                    viewModel.isNonInteractive= true;
                                }
                            }
                        }
                    }

                    RowLayout {
                        spacing:    10
                        visible:    !viewModel.isNonInteractive
                        SFText {
                            //% "One-time use"
                            text:  qsTrId("token-one-time")
                            color: permanentTokenSwitch.checked ? Style.content_secondary : Style.active
                            font.pixelSize: 14
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    permanentTokenSwitch.checked = !permanentTokenSwitch.checked;
                                }
                            }
                        }

                        CustomSwitch {
                            id:          permanentTokenSwitch
                            alwaysGreen: true
                            spacing:     0
                            checked:     viewModel.isPermanentAddress
                            Binding {
                                target:   viewModel
                                property: "isPermanentAddress"
                                value:    permanentTokenSwitch.checked
                            }
                        }

                        SFText {
                            //% "Permanent token"
                            text: qsTrId("token-permanent")
                            color: permanentTokenSwitch.checked ? Style.active : Style.content_secondary
                            font.pixelSize: 14
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    permanentTokenSwitch.checked = !permanentTokenSwitch.checked;
                                }
                            }
                        }
                    }

                    SFText {
                        height: 16
                        Layout.alignment:   Qt.AlignTop
                        id:                 maxPrivacyNote
                        color:              Style.content_secondary
                        font.italic:        true
                        font.pixelSize:     14
                        text:               viewModel.isNonInteractive ?
                                            //% "Token good for 20 transactions."
                                            qsTrId("wallet-send-non-int-note") : 
                                            //% "Transaction is slower, fees are higher."
                                            qsTrId("wallet-send-max-privacy-note")
                        visible:            viewModel.isShieldedTx
                    }
                }
            }
            //
            // Request
            //
            Panel {
                //% "Request"
                title:                   qsTrId("receive-request")
                Layout.fillWidth:        true
                //
                // Amount
                //
                content: AmountInput {
                    //Layout.topMargin:           20
                    ////% "Receive amount (optional)"
                    //title:                      qsTrId("receive-amount-label")
                    id:                         receiveAmountInput
                    amountIn:                   viewModel.amountToReceive
                    secondCurrencyRateValue:    viewModel.secondCurrencyRateValue
                    secondCurrencyLabel:        viewModel.secondCurrencyLabel
                }
                
                Binding {
                    target:   viewModel
                    property: "amountToReceive"
                    value:    receiveAmountInput.amount
                }
            }
            ////
            //// Fee
            ////
            //Panel {
            //    //% "Fee"
            //    title:                   qsTrId("general-fee")
            //    Layout.preferredHeight:  100
            //    Layout.fillWidth:        true
            //}
            //
            // Comment
            //
            Panel {
                //% "Comment"
                title:             qsTrId("general-comment")
                Layout.preferredHeight:  100
                Layout.fillWidth:        true

                content:
                ColumnLayout {
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
                            visible:        !viewModel.commentValid
                            font.italic:    true
                        }
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
            TokenInfoPanel {
                Layout.fillWidth:   true
                //% "For wallet"
                title:              qsTrId("wallet-receive-token-for-wallet")
                token:              viewModel.transactionToken
                qrCode:             viewModel.transactionTokenQR
                isValidToken:       receiveView.isValid()
                onTokenCopied: {
                    receiveView.saveAddress();
                    onClosed();
                }
            }
            TokenInfoPanel {
                Layout.fillWidth:   true
                //% "For exchange or mining pool"
                title:              qsTrId("wallet-receive-token-for-exchange")
                token:              viewModel.receiverAddress
                qrCode:             viewModel.receiverAddressQR
                isValidToken:       receiveView.isValid()
                disabledText:       !viewModel.isPermanentAddress && !viewModel.isNonInteractive ?
                                        //% "One-time uses token is not supported by exchanges or mining pools yet. Please switch to the permanent."
                                        qsTrId("wallet-receive-exchanges-one-time-not-supported")
                                        :
                                        viewModel.isShieldedTx ?
                                            viewModel.isNonInteractive ? 
                                                //% "Non-interactive transaction is not supported by exchanges or mining pools yet."
                                                qsTrId("wallet-receive-exchanges-not-supported2") :
                                                //% "Max privacy transaction is not supported by exchanges or mining pools yet."
                                                qsTrId("wallet-receive-exchanges-not-supported") : ""

                onTokenCopied: {
                    receiveView.saveAddress();
                    onClosed();
                }
            }

        }
    }

    //
    // Footers
    //
    SFText {
        Layout.alignment:      Qt.AlignHCenter
        Layout.preferredWidth: 418
        Layout.topMargin:      30
        font.pixelSize:        14
        font.italic:           true
        color:                 Style.content_main
        wrapMode:              Text.WordWrap
        horizontalAlignment:   Text.AlignHCenter
        //% "Send token or address to the sender over an external secure channel or scan the QR code."
        text: qsTrId("wallet-receive-addr-message")
    }

    SFText {
        Layout.alignment:      Qt.AlignHCenter
        Layout.preferredWidth: 338
        Layout.topMargin:      20
        Layout.bottomMargin:   50
        font.pixelSize:        14
        font.italic:           true
        color:                 Style.content_disabled
        wrapMode:              Text.WordWrap
        horizontalAlignment:   Text.AlignHCenter
        //% "For the transaction to complete, you should get online during the 12 hours after Beams are sent."
        text: qsTrId("wallet-receive-text-online-time")
    }

    Item {
        Layout.fillHeight: true
    }
}
