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
    id: receiveView
    property var defaultFocusItem: null//addressComment

    // callbacks set by parent
    property var onClosed: function() {}
    property string token: ""

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

        Component.onCompleted: {
            viewModel.initialize(token);
        }
    }

    function isValid() {
        return viewModel.commentValid
    }

    SaveAddressDialog {
        id:              saveAddressDialog
        //% "Do you want to name the contact?"
        dialogTitle:     qsTrId("save-contact-title")
        //% "No name"
        text:            qsTrId("save-address-no-name")
        //% "Enter the name to this contact"
        placeholderText: qsTrId("contact-name-prompt")

        onAccepted: {
            saveAddressWithNameAndClose(text)
        }
        onRejected: {
            saveAddressWithNameAndClose("") 
        }
    }

    function saveAddressWithNameAndClose(name) {
        saveAddressWithName(name);
        receiveView.onClosed();
    }

    function saveAddressWithName(name) {
        viewModel.addressComment = name;
        viewModel.saveReceiverAddress();
    }

    function saveReceiverAddress() {
        saveAddressWithName(viewModel.addressComment)
    }

    function saveAddressAndClose() {
        // TODO: uncomment
        //if (receiveView.isValid() && viewModel.isPermanentAddress) {
        //    saveAddressDialog.open();
        //} else {
        //    saveAddressWithNameAndClose("") 
        //}
        saveAddressWithNameAndClose(viewModel.addressComment)
    }

    function saveAddress() {
        if (receiveView.isValid()) 
            viewModel.saveReceiverAddress();
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
            onClicked:      {
                receiveView.saveAddress();
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
            //% "Receive"
            text:               qsTrId("wallet-receive-title")
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
                    Layout.alignment:   Qt.AlignTop
                    Layout.fillWidth:   true
                    Layout.preferredWidth: 400
                    spacing:            10
                    Panel {
                        //% "Address Type"
                        title:                   qsTrId("general-address-type")
                        Layout.fillWidth:        true
                        content: 
                        ColumnLayout {
                            spacing: 20
                            property bool isShieldedSupported: statusbarModel.isConnectionTrusted && statusbarModel.isOnline
                            Pane {
                                padding:    2
                                visible:    parent.isShieldedSupported
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
                                            viewModel.isPermanentAddress = false;
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
                                        checked:            viewModel.isShieldedTx
                                        onToggled: {
                                            viewModel.isShieldedTx = true;
                                            viewModel.isPermanentAddress = true;
                                        }
                                    }
                                }
                            }

                            SFText {
                                Layout.fillWidth:   true
                                visible:            !parent.isShieldedSupported
                                color:              Style.content_secondary
                                font.italic:        true
                                font.pixelSize:     14
                                wrapMode:           Text.WordWrap
                                //% "Connect to integrated or own node to enable sending max privacy transactions"
                                text:               qsTrId("wallet-receive-max-privacy-unsupported")
                            }
                            
                        }
                    }
                    //
                    // Requested amount
                    //
                    FoldablePanel {
                        //% "Requested amount"
                        title:                   qsTrId("receive-request")
                        //% "(optional)"
                        headerText:              qsTrId("receive-request-optional")
                        Layout.fillWidth:        true
                        //
                        // Amount
                        //
                        content: AmountInput {
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

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title:                   qsTrId("general-comment")
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
                        //% "Online address"
                        title:              !viewModel.isShieldedTx ? 
                            //% "Online address"
                            qsTrId("wallet-receive-online-address")
                            :
                            //% "Max Privacy Address"
                            qsTrId("wallet-receive-max-privacy-address")
                        headerText:         !viewModel.isShieldedTx ? 
                            //% "(for wallet)"
                            qsTrId("wallet-receive-address-for-wallet")
                            :
                            ""
                        //% "Online address (for wallet)"
                        addressLabel:       qsTrId("wallet-receive-address-for-wallet-label")
                        token:              viewModel.transactionToken
                        isValidToken:       receiveView.isValid()
                        onTokenCopied: {
                            receiveView.saveReceiverAddress();
                        }
                        onClosed: receiveView.onClosed()

                        headerVisible:  !viewModel.isShieldedTx
                        headerItem: RowLayout {
                            spacing:    10
                            SFText {
                                //% "One-time use"
                                text:  qsTrId("address-one-time")
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
                                padding:     0
                                checked:     viewModel.isPermanentAddress
                                Binding {
                                    target:   viewModel
                                    property: "isPermanentAddress"
                                    value:    permanentTokenSwitch.checked
                                }
                            }
                            
                            SFText {
                                //% "Permanent"
                                text: qsTrId("address-permanent")
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
                            Item {
                                Layout.fillWidth:   true
                            }
                        }
                    }
                    TokenInfoPanel {
                        Layout.fillWidth:     true
                        title:                qsTrId("wallet-receive-online-address")
                        //% "(for exchange or mining pool)"
                        headerText:           qsTrId("wallet-receive-address-for-exchange")
                        //% "Online address (for exchange or mining pool)"
                        addressLabel:         qsTrId("wallet-receive-address-for-exchange-label")
                        token:                viewModel.receiverAddressForExchange
                        amount:               viewModel.amountToReceive
                        isValidToken:         receiveView.isValid()
                        visible:              !viewModel.isShieldedTx
                        onTokenCopied: {
                            viewModel.saveExchangeAddress();
                        }
                        onClosed: receiveView.onClosed()
                    }
                    TokenInfoPanel {
                        Layout.fillWidth:     true
                        //% "Offline address"
                        title:                qsTrId("wallet-receive-offline-address")
                        //% "(for wallet)"
                        headerText:           qsTrId("wallet-receive-address-for-wallet")
                        footerVisible:        true
                        footerItem: SFText {
                            font.pixelSize:        14
                            font.italic:           true
                            color:                 Style.content_disabled
                            /*% "Supports %1 payments."*/
                            text: qsTrId("wallet-receive-offline-payments").arg(10)
                        }
                        token:                viewModel.offlineToken
                        showQrCode:           false
                        isValidToken:         receiveView.isValid()
                        visible:              !viewModel.isShieldedTx && viewModel.offlineToken.length > 0
                        ignoreStoredVouchers: true
                        onTokenCopied: {
                            viewModel.saveOfflineAddress();
                        }
                        onClosed: receiveView.onClosed()
                    }
                }
            }

            //
            // Footers
            //
            SFText {
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 428
                Layout.topMargin:      30
                font.pixelSize:        14
                font.italic:           true
                color:                 Style.content_disabled
                wrapMode:              Text.WordWrap
                horizontalAlignment:   Text.AlignHCenter
                /*% "Min transaction fee to send Max privacy coins is %1."*/
                text: qsTrId("wallet-receive-addr-message").arg("~%1 BEAM".arg(Utils.uiStringToLocale("0.01")))
                visible:               viewModel.isShieldedTx
            }

            SFText {
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 338
                Layout.topMargin:      30
                Layout.bottomMargin:   50
                font.pixelSize:        14
                font.italic:           true
                color:                 Style.content_disabled
                wrapMode:              Text.WordWrap
                horizontalAlignment:   Text.AlignHCenter
                //% "For the transaction to complete, you should get online during the 12 hours after Beams are sent."
                text: qsTrId("wallet-receive-text-online-time")
                visible:               !viewModel.isShieldedTx
            }

            Item {
                Layout.fillHeight: true
            }
        }  // ColumnLayout
    }  // ScrollView
}
