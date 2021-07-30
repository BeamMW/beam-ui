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
    property string appName:     ""
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

    readonly property bool isSpend:  {
        for (var idx = 0; idx < amounts.length; ++idx) {
            if (amounts[idx].spend) {
                return true
            }
        }
        return false
    }

    readonly property bool isReceive: {
        for (var idx = 0; idx < amounts.length; ++idx) {
            if (!amounts[idx].spend) {
                return true
            }
        }
        return false
    }

    defaultFocusItem: BeamGlobals.needPasswordToSpend() ? requirePasswordInput : cancelButton
    title: {
        if (control.appMode) {
            if (isReceive && isSpend) {
                //% "Confirm withdraw & deposit"
                return qsTrId("send-app-twoway-confirmation-title")
            }
            if (isSpend) {
                //% "Confirm deposit from the wallet"
                return qsTrId("send-app-spend-confirmation-title")
            }
            if (isReceive) {
                //% "Confirm withdraw to the wallet"
                return qsTrId("send-app-receive-confirmation-title")
            }
            //% "Confirm application transaction"
            return qsTrId("send-app-confirmation-title")
        }
        //% "Confirm transaction details"
        return qsTrId("send-confirmation-title")
    }

    okButtonText: control.appMode ?
        //% "Confirm"
        qsTrId("general-confirm"):
        //% "Send"
        qsTrId("general-send")

    okButtonColor: control.isSpend ? Style.accent_outgoing : Style.accent_incoming
    okButtonIconSource: {
        if (control.appMode) {
            if (control.isSpend && control.isReceive) {
                return "qrc:/assets/icon-send-receive-blue.svg"
            }
            if (control.isSpend) {
                return "qrc:/assets/icon-send-blue.svg"
            }
            return "qrc:/assets/icon-receive-blue.svg"
        }
        return "qrc:/assets/icon-send-blue.svg"
    }

    okButtonEnable: isEnough && (BeamGlobals.needPasswordToSpend() ? !!requirePasswordInput.text : true)
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"

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
    contentItem: Item { ColumnLayout {
        spacing: 22

        GridLayout {
            Layout.alignment:  Qt.AlignHCenter
            Layout.fillWidth:  false
            Layout.fillHeight: false
            columnSpacing:     typeLabel.visible ? 14 : 30
            rowSpacing:        14
            columns:           2

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
                visible:                typeLabel.visible
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
            // Amounts
            //
            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_disabled
                //% "Amount"
                text: qsTrId("general-amount") + ":"
            }

            SFText {
                Layout.alignment:  Qt.AlignTop
                Layout.leftMargin: 27
                font.pixelSize:    14
                color:   Style.content_main
                visible: !control.hasAmounts

                font {
                    pixelSize: 14
                    styleName: "Bold"
                    weight:    Font.Bold
                }

                text: "-"
            }

            ColumnLayout {
                Layout.maximumWidth: 290
                visible:  control.hasAmounts
                spacing:  8

                Repeater {
                    model: control.amounts

                    BeamAmount  {
                        amount:           modelData.amount
                        unitName:         (assetsProvider ? assetsProvider.assets[modelData.assetID] : modelData).unitName
                        rate:             (assetsProvider ? assetsProvider.assets[modelData.assetID] : modelData).rate
                        prefix:           control.showPrefix ? (modelData.spend ? "- " : "+ ") : ""
                        rateUnit:         control.rateUnit
                        maxPaintedWidth:  false
                        maxUnitChars:     7
                        color:            modelData.spend ? Style.accent_outgoing : Style.accent_incoming
                        iconSize:         Qt.size(20, 20)
                        iconSource:       (assetsProvider ? assetsProvider.assets[modelData.assetID] : modelData).icon || ""
                        iconAnchorCenter: false

                        font.pixelSize:   14
                        font.styleName:   "DemiBold"
                        font.weight:      Font.DemiBold
                        rateFontSize:     12
                        copyMenuEnabled:  true
                    }
                }
            }

            //
            // Fee
            //
            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize:   14
                color:            Style.content_disabled
                //% "Fee"
                text:             [qsTrId("send-regular-fee"), ":"].join("")
            }

            BeamAmount  {
                Layout.maximumWidth: 290

                amount:           control.fee
                unitName:         control.feeUnit
                rate:             control.feeRate
                rateUnit:         control.rateUnit
                maxPaintedWidth:  false
                maxUnitChars:     7
                color:            Style.content_main
                iconSize:         Qt.size(20, 20)
                iconSource:       assetsProvider ? assetsProvider.assets[0].icon : ""
                iconAnchorCenter: false

                font.pixelSize:   14
                font.styleName:   "DemiBold"
                font.weight:      Font.DemiBold
                rateFontSize:     12
                copyMenuEnabled:  true
            }
        }

        SFText {
            Layout.topMargin: 8
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter

            visible:  !!text
            color:    !isEnough ? Style.validator_error : Style.content_disabled
            wrapMode: Text.Wrap

            font {
                italic: true
                pixelSize: 14
            }

            text: {
                //% "There is not enough funds to complete the transaction"
                if (!isEnough) return qsTrId("send-not-enough")

                if (control.appMode) {
                    if (isSpend && isReceive) {
                        //% "%1 Dapp will change the balances of your wallet"
                        return qsTrId("send-twoway-warning").arg(control.appName)
                    }

                    if (isSpend) {
                        //% "%1 Dapp will take the funds from your wallet"
                        return qsTrId("send-dapp-spend-warning").arg(control.appName)
                    }

                    if (isReceive) {
                        //% "%1 Dapp will send the funds to your wallet"
                        return qsTrId("send-dapp-receive-warning").arg(control.appName)
                    }

                    //% "The transaction fee would be deducted from your balance"
                    return qsTrId("send-contract-only-fee")
                }

                return ""
            }
        }

        //
        // Password confirmation
        //
        Rectangle {
            Layout.fillWidth:  true
            height:   1
            color:    Qt.rgba(1, 1, 1, 0.1)
            visible:  isEnough && BeamGlobals.needPasswordToSpend()
        }

        SFText {
            id:                   requirePasswordLabel
            visible:              isEnough && BeamGlobals.needPasswordToSpend()
            horizontalAlignment:  Text.AlignHCenter
            Layout.fillWidth:     true
            font.pixelSize:       14
            color:                Style.content_disabled
            //% "To approve the transaction please enter your password"
            text:                 qsTrId("send-confirmation-pwd-require-message")
        }

        Column {
            Layout.fillWidth:    true
            Layout.minimumWidth: 340
            spacing: 0
            visible: isEnough && BeamGlobals.needPasswordToSpend()

            SFTextInput {
                id:               requirePasswordInput
                width:            parent.width
                font.pixelSize:   14
                color:            requirePasswordError.text ? Style.validator_error : Style.content_main
                backgroundColor:  requirePasswordError.text ? Style.validator_error : Style.content_main
                echoMode:         TextInput.Password
                leftPadding:      14
                rightPadding:     14

                onAccepted: function () {
                    control.okButton.clicked()
                }

                onTextChanged: function () {
                    requirePasswordError.text = ""
                }

                background: Rectangle  {
                    color:        Qt.rgba(1, 1, 1, 0.05)
                    radius:       10

                }
            }

            SFText {
                id:              requirePasswordError
                color:           Style.validator_error
                font.pixelSize:  12
                font.italic:     true
            }
        }

        SFText {
            id:                     onlineMessageText
            horizontalAlignment:    Text.AlignHCenter
            Layout.maximumWidth:    420
            font.pixelSize:         14
            color:                  Style.content_disabled
            wrapMode:               Text.WordWrap
            visible:                isEnough && isOnline
            //% "For the transaction to complete, the recipient must get online within the next 12 hours and you should get online within 2 hours afterwards."
            text:                   qsTrId("send-confirmation-pwd-text-online-time")
        }

        Item {
            height: 1
        }
    }}
}