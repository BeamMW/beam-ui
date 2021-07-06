import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
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
    property bool   isOnline:    true
    property bool   appMode:     false
    property bool   showPrefix:  false
    property alias  comment:     commentCtrl.text
    property var    amounts
    property var    assetsProvider
    property bool   hasAmounts:  amounts && amounts.length > 0

    property string rateUnit: ""
    property string fee:      "0"
    property string feeRate:  "0"
    property string feeUnit:  BeamGlobals.beamUnit
    property bool   isEnough: true

    defaultFocusItem: BeamGlobals.needPasswordToSpend() ? requirePasswordInput : cancelButton

    //% "Send"
    okButtonText:            qsTrId("general-send")
    okButtonColor:           Style.accent_outgoing
    okButtonIconSource:      "qrc:/assets/icon-send-blue.svg"
    okButtonEnable:          isEnough && (BeamGlobals.needPasswordToSpend() ? !!requirePasswordInput.text : true)
    cancelButtonIconSource:  "qrc:/assets/icon-cancel-white.svg"

    beforeAccept: function () {
        if (BeamGlobals.needPasswordToSpend()) {
            if (!requirePasswordInput.text) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                return false
            }

            if (!BeamGlobals.isPasswordValid(requirePasswordInput.text)) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                requirePasswordInput.selectAll();
                requirePasswordError.text = qsTrId("general-pwd-invalid");
                return false
            }
        }
        return true
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
            Layout.bottomMargin:    10
            columnSpacing:          14
            rowSpacing:             14
            columns:                2

            //
            // Recipient/Address
            //
            SFText {
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Recipient"
                text:                   qsTrId("send-confirmation-recipient-label") + ":"
                verticalAlignment:      Text.AlignTop
                visible:                addressLabel.visible
            }

            SFLabel {
                id:                     addressLabel
                Layout.maximumWidth:    290
                wrapMode:               Text.NoWrap
                elide:                  Text.ElideMiddle
                font.pixelSize:         14
                color:                  Style.content_main
                copyMenuEnabled:        true
                onCopyText:             BeamGlobals.copyToClipboard(text)
                visible:                !!text
            }

            //
            // Comment
            //
            SFText {
                font.pixelSize:    14
                color:             Style.content_disabled
                //% "Comment"
                text:              qsTrId("general-comment") + ":"
                visible:           commentCtrl.visible
            }

            SFLabel {
                id:                   commentCtrl
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
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Transaction type"
                text:                   qsTrId("send-type-label") + ":"
                verticalAlignment:      Text.AlignTop
                visible:                typeLabel.text.length > 0
            }

            SFText {
                id:                     typeLabel
                Layout.maximumWidth:    290
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
                font.pixelSize:    14
                color:             Style.content_disabled
                //% "Amount"
                text: qsTrId("general-amount") + ":"
            }

            SFText {
                Layout.maximumWidth: 290
                font.pixelSize:      14
                color:               Style.content_main
                visible:             !control.hasAmounts
                maximumLineCount:    4
                wrapMode:            Text.Wrap
                //% "You would pay only transaction fee. It can be considerable for some contracts, so please check it below."
                text: qsTrId("send-contract-only-fee")
            }

            ColumnLayout {
                Layout.maximumWidth: 290
                Layout.fillWidth:    true
                visible:             control.hasAmounts
                spacing:             8

                Repeater {
                    model: control.amounts

                    BeamAmount  {
                        Layout.maximumWidth: 290
                        Layout.fillWidth:    true

                        amount:           modelData.amount
                        unitName:         (assetsProvider ? assetsProvider.assets[modelData.assetID] : modelData).unitName
                        rate:             (assetsProvider ? assetsProvider.assets[modelData.assetID] : modelData).rate
                        prefix:           control.showPrefix ? (modelData.spend ? "- " : "+ ") : ""
                        rateUnit:         control.rateUnit
                        maxPaintedWidth:  false
                        maxUnitChars:     15

                        color:            modelData.spend ? Style.accent_outgoing : Style.accent_incoming

                        font.pixelSize:   24
                        font.styleName:   "Bold"
                        font.weight:      Font.Bold
                        rateFontSize:     14
                        spacing:          15
                        copyMenuEnabled:  true
                    }
                }
            }

            //
            // Fee
            //
            SFText {
                font.pixelSize:         14
                color:                  Style.content_disabled
                //% "Fee"
                text:                   [qsTrId("send-regular-fee"), ":"].join("")
            }

            ColumnLayout {
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

            SFText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: !isEnough
                color: Style.validator_error
                font.italic: true
                font.pixelSize:         14
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                //% "There is not enough funds to complete the transaction"
                text: qsTrId("send-not-enough")
            }

            //
            // Password confirmation
            //
            SFText {
                id:                     requirePasswordLabel
                visible:                isEnough && BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.topMargin:       8
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_main
                //% "To broadcast your transaction please enter your password"
                text:                   qsTrId("send-confirmation-pwd-require-message")
            }

            Column {
                Layout.fillWidth:    true
                Layout.minimumWidth: 340
                Layout.columnSpan:   2
                spacing: 0
                visible: isEnough && BeamGlobals.needPasswordToSpend()

                SFTextInput {
                    id:                requirePasswordInput
                    width:             parent.width
                    font.pixelSize:    14
                    color:             requirePasswordError.text ? Style.validator_error : Style.content_main
                    backgroundColor:   requirePasswordError.text ? Style.validator_error : Style.content_main
                    echoMode:          TextInput.Password

                    onAccepted: function () {
                        control.okButton.clicked()
                    }

                    onTextChanged: function () {
                        requirePasswordError.text = ""
                    }
                }

                Item {
                    SFText {
                        id:              requirePasswordError
                        color:           Style.validator_error
                        font.pixelSize:  12
                        font.italic:     true
                    }
                }
            }

            SFText {
                id:                     onlineMessageText
                Layout.columnSpan:      2
                horizontalAlignment:    Text.AlignHCenter
                Layout.maximumWidth:    420
                font.pixelSize:         14
                color:                  Style.content_disabled
                wrapMode:               Text.WordWrap
                visible:                isEnough && isOnline
                //% "For the transaction to complete, the recipient must get online within the next 12 hours and you should get online within 2 hours afterwards."
                text:                   qsTrId("send-confirmation-pwd-text-online-time")
            }
        }
    }
}