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
    property var defaultFocusItem: null // addressComment

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

    function copyAndClose() {
        if (isValid()) {
            BeamGlobals.copyToClipboard(viewModel.transactionToken)
            viewModel.saveReceiverAddress();
            receiveView.onClosed()
        }
    }

    function copyAndSave() {
         if (isValid()) {
            BeamGlobals.copyToClipboard(viewModel.transactionToken)
            viewModel.saveReceiverAddress();
         }
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

    //
    // Title row
    //
    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30

        //% "Receive"
        text: qsTrId("wallet-receive-title")
        onBack: function () {
            if (isValid()) viewModel.saveReceiverAddress();
            receiveView.onClosed()
        }
    }

    QR {
        id: qrCode
        address: viewModel.transactionToken
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
                        //% "Transaction type"
                        title: qsTrId("general-tx-type")
                        Layout.fillWidth: true

                        content: ColumnLayout {
                            spacing: 20
                            id: addressType
                            property bool isShieldedSupported: statusbarModel.isConnectionTrusted && statusbarModel.isOnline

                            Pane {
                                padding: 2
                                background: Rectangle {
                                    color:  Qt.rgba(1, 1, 1, 0.1)
                                    radius: 16
                                }
                                ButtonGroup {id: txTypeGroup}
                                RowLayout {
                                    spacing: 0
                                    CustomButton {
                                        Layout.preferredHeight: 30
                                        Layout.preferredWidth: maxPrivacyCheck.width
                                        id: regularCheck

                                        //% "Regular"
                                        text:               qsTrId("tx-regular")
                                        ButtonGroup.group:  txTypeGroup
                                        checkable:          true
                                        hasShadow:          false
                                        checked:            !viewModel.isShieldedTx
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary

                                        onToggled: function () {
                                            viewModel.isMaxPrivacy = false
                                        }
                                    }
                                    CustomButton {
                                        Layout.preferredHeight: 30
                                        Layout.minimumWidth: 137
                                        id: maxPrivacyCheck
                                        //% "Max privacy"
                                        text:               qsTrId("tx-max-privacy")
                                        ButtonGroup.group:  txTypeGroup
                                        checkable:          true
                                        checked:            viewModel.isShieldedTx
                                        enabled:            addressType.isShieldedSupported
                                        hasShadow:          false
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary
                                        onToggled: function () {
                                            viewModel.isMaxPrivacy = true
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
                                //% "Connect to integrated or own node to enable receiving max privacy and offline transactions"
                                text:               qsTrId("wallet-receive-max-privacy-unsupported")
                            }
                            
                        }
                    }

                    //
                    // Requested amount
                    //
                    FoldablePanel {
                        //% "Requested amount"
                        title:             qsTrId("receive-request")
                        //% "(optional)"
                        titleTip:          qsTrId("receive-request-optional")
                        Layout.fillWidth:  true
                        folded:            true

                        //
                        // Amount
                        //
                        content: AmountInput {
                            id:        receiveAmountInput
                            amountIn:  viewModel.amountToReceive
                            rate:      viewModel.rate
                            rateUnit:  viewModel.rateUnit
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
                        title:            qsTrId("general-comment")
                        Layout.fillWidth: true
                        folded:           false

                        content: ColumnLayout {
                            spacing: 0

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
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    Panel {
                        //% "New Address"
                        title: qsTrId("receive-new-addr")
                        Layout.fillWidth: true

                        content: RowLayout {
                            spacing: 0

                            Image {
                                Layout.preferredWidth:  130
                                Layout.preferredHeight: 130
                                fillMode:               Image.PreserveAspectFit
                                source:                 qrCode.data
                            }

                            Column {
                                id: tokenColumn

                                Layout.fillWidth: true
                                Layout.leftMargin: 20
                                Layout.rightMargin: 2
                                Layout.alignment: Qt.AlignVCenter

                                SFText {
                                    text:  viewModel.transactionToken
                                    width: parent.width
                                    color: Style.content_main
                                    elide: Text.ElideMiddle
                                }

                                LinkButton {
                                    //% "More details"
                                    text:       qsTrId("more-details")
                                    linkColor:  Style.accent_incoming
                                    onClicked:  function () {
                                    }
                                }
                            }

                            SvgImage {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.bottomMargin: 10

                                source: "qrc:/assets/icon-copy.svg"
                                sourceSize: Qt.size(16, 16)
                                opacity: receiveView.isValid() ? 1.0 : 0.45

                                MouseArea {
                                   anchors.fill: parent
                                   acceptedButtons: Qt.LeftButton
                                   cursorShape: receiveView.isValid() ? Qt.PointingHandCursor : Qt.ArrowCursor
                                   onClicked: function () {
                                        receiveView.copyAndSave()
                                   }
                                }
                            }
                        }
                    }
                }
            }

            CustomButton {
                id: copyButton
                Layout.topMargin:       30
                Layout.alignment:       Qt.AlignHCenter
                //% "Copy and close"
                text:                   qsTrId("wallet-receive-copy-and-close")
                Layout.preferredHeight: 38
                palette.buttonText:     Style.content_opposite
                icon.color:             Style.content_opposite
                palette.button:         Style.accent_incoming
                icon.source:            "qrc:/assets/icon-copy.svg"
                enabled:                receiveView.isValid()

                onClicked: function () {
                    receiveView.copyAndClose()
                }
            }

            //
            // Footers
            //
            SFText {
                property string mpLockTimeLimit: viewModel.mpTimeLimit
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 428
                Layout.topMargin:      15
                font.pixelSize:        14
                font.italic:           true
                color:                 Style.content_disabled
                wrapMode:              Text.WordWrap
                horizontalAlignment:   Text.AlignHCenter
                visible:               viewModel.isShieldedTx
                text: (mpLockTimeLimit != "0" ?
                    //% "Max Privacy transaction can last at most %1 hours."
                    qsTrId("wallet-receive-addr-message-mp").arg(mpLockTimeLimit) :
                    //% "Max Privacy transaction can last indefinitely."
                    qsTrId("wallet-receive-addr-message-mp-no-limit")) + "\n" +
                    //% "Min transaction fee to send Max privacy coins is %1."
                    qsTrId("wallet-receive-addr-message").arg("~%1 BEAM".arg(Utils.uiStringToLocale("0.01")))
            }

            SFText {
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 338
                Layout.topMargin:      15
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
        }  // ColumnLayout
    }  // ScrollView
}
