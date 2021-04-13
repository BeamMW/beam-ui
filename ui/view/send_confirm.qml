import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ConfirmationDialog {
    onVisibleChanged: {
        if (!this.visible) {
            this.destroy();
        }
    }

    id: control
    parent: Overlay.overlay

    property alias  addressText: addressLabel.text
    property alias  typeText:    typeLabel.text
    property alias  isOnline:    onlineMessageText.visible
    property bool   appMode:     false
    property alias  comment:     commentCtrl.text

    property string amount:    "0"
    property string rate:      "0"
    property string rateUnit:  ""
    property string unitName:  BeamGlobals.beamUnit

    property string fee:       "0"
    property string feeRate:   "0"
    property string feeUnit:   BeamGlobals.beamUnit

    property Item defaultFocusItem: BeamGlobals.needPasswordToSpend() ? requirePasswordInput : cancelButton

    //% "Send"
    okButtonText:            qsTrId("general-send")
    okButtonColor:           Style.accent_outgoing
    okButtonIconSource:      "qrc:/assets/icon-send-blue.svg"
    okButtonEnable:          BeamGlobals.needPasswordToSpend() ? requirePasswordInput.text.length : true
    cancelButtonIconSource:  "qrc:/assets/icon-cancel-white.svg"

    function confirmationHandler() {
        if (BeamGlobals.needPasswordToSpend()) {
            if (!requirePasswordInput.text) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                return;
            }

            if (!BeamGlobals.isPasswordValid(requirePasswordInput.text)) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                requirePasswordInput.selectAll();
                requirePasswordError.text = qsTrId("general-pwd-invalid");
                return;
            }
        }

        accepted();
        close();
    }

    function openHandler() {
        BeamGlobals.showMessage("fee: " + control.fee)
        defaultFocusItem.forceActiveFocus(Qt.TabFocusReason);
    }

    function passworInputEnter() {
        okButton.forceActiveFocus(Qt.TabFocusReason);
        okButton.clicked();
    }

    topPadding: 30
    contentItem:
    ColumnLayout {
        spacing: 30

        SFText {
            id: title
            Layout.alignment:   Qt.AlignHCenter
            font.pixelSize:     18
            font.styleName:     "Bold";
            font.weight:        Font.Bold
            color:              Style.content_main
            text:               control.appMode ?
                                //% "Confirm Application Transaction"
                                qsTrId("send-app-confirmation-title") :
                                //% "Confirm transaction details"
                                qsTrId("send-confirmation-title")
        }

        GridLayout {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            columnSpacing:          14
            rowSpacing:             14
            columns:                2

            //
            // Recipient/Address
            //
            SFText {
                Layout.fillWidth:       false
                Layout.fillHeight:      true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Recipient"
                text:                   qsTrId("send-confirmation-recipient-label") + ":"
                verticalAlignment:      Text.AlignTop
            }

            SFLabel {
                id:                     addressLabel
                Layout.fillWidth:       true
                Layout.maximumWidth:    290
                Layout.minimumHeight:   16
                wrapMode:               Text.NoWrap
                elide:                  Text.ElideMiddle
                font.pixelSize:         14
                color:                  Style.content_main
                copyMenuEnabled:        true
                onCopyText:             BeamGlobals.copyToClipboard(text)
            }

            //
            // Comment
            //
            SFText {
                Layout.fillWidth:  false
                font.pixelSize:    14
                color:             Style.content_disabled
                //% "Comment"
                text:              qsTrId("general-comment") + ":"
                visible:           commentCtrl.visible
            }

            SFLabel {
                id:                   commentCtrl
                Layout.fillWidth:     true
                Layout.maximumWidth:  290
                wrapMode:             Text.Wrap
                elide:                Text.ElideRight
                font.pixelSize:       14
                color:                Style.content_main
                copyMenuEnabled:      true
                onCopyText:           BeamGlobals.copyToClipboard(text)
                maximumLineCount:     4
                visible:              !!text
            }

            //
            // Address type
            //
            SFText {
                Layout.fillWidth:       true
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Transaction type"
                text:                   qsTrId("send-type-label") + ":"
                verticalAlignment:      Text.AlignTop
                visible:                typeLabel.text.length > 0
            }

            SFText {
                id:                     typeLabel
                Layout.fillWidth:       true
                Layout.maximumWidth:    290
                Layout.minimumHeight:   16
                wrapMode:               Text.Wrap
                maximumLineCount:       2
                font.pixelSize:         14
                color:                  Style.content_main
                visible:                text.length > 0
            }

            //
            // Amount
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                Layout.topMargin:       8
                font.pixelSize:         14
                color: Style.content_disabled
                //% "Amount"
                text: qsTrId("general-amount") + ":"
                verticalAlignment: Text.AlignTop
            }

            BeamAmount {
                visible: true
                id: amountLabel
                spacing: 15
                font.pixelSize:    24
                font.styleName:    "Bold"
                font.weight:       Font.Bold
                rateFontSize:      14
                copyMenuEnabled:   true
                unitName:          control.unitName
                amount:            control.amount
                rate:              control.rate
                rateUnit:          control.rateUnit
                color:             Style.accent_outgoing
                maxPaintedWidth:   false
                Layout.fillWidth:  true
            }

            //
            // Fee
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Fee"
                text:                   [qsTrId("send-regular-fee"), ":"].join("")
                verticalAlignment:      Text.AlignTop
            }

            ColumnLayout {
                Layout.fillWidth:   true
                SFText {
                    id:              feeLabel
                    font.pixelSize:  14
                    color:           Style.content_main
                    text:            [Utils.uiStringToLocale(control.fee), control.feeUnit].join(" ")
                }
                SFText {
                    id:                 secondCurrencyFeeLabel
                    visible:            control.feeRate != "0"
                    font.pixelSize:     14
                    color:              Style.content_disabled
                    text:               Utils.formatAmountToSecondCurrency(control.fee, control.feeRate, control.rateUnit)
                }
            }

            //
            // Password confirmation
            //
            SFText {
                id:                     requirePasswordLabel
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.topMargin:       16
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_main
                //% "To broadcast your transaction please enter your password"
                text:                   qsTrId("send-confirmation-pwd-require-message")
            }

            SFTextInput {
                id:                     requirePasswordInput
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.fillWidth:       true
                focus:                  true
                activeFocusOnTab:       true
                font.pixelSize:         14
                color:                  Style.content_main
                echoMode:               TextInput.Password
                onAccepted:             passworInputEnter()
                onTextChanged:          if (requirePasswordError.text.length > 0) requirePasswordError.text = ""
            }

            SFText {
                Layout.fillWidth:       true
                Layout.columnSpan:      2
                id:                     requirePasswordError
                visible:                BeamGlobals.needPasswordToSpend()
                color:                  Style.validator_error
                font.pixelSize:         14
            }

            SFText {
                id:                     onlineMessageText
                Layout.columnSpan:      2
                Layout.topMargin:       0
                Layout.bottomMargin:    15
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       true
                Layout.maximumHeight:   60
                Layout.maximumWidth:    420
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_disabled
                wrapMode:               Text.WordWrap
                //% "For the transaction to complete, the recipient must get online within the next 12 hours and you should get online within 2 hours afterwards."
                text:                   qsTrId("send-confirmation-pwd-text-online-time")
            }
        }
    }
}