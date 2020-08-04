import QtQuick.Layouts 1.11
import QtQuick 2.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
        id: control

        property bool    fillWidth:                  false
        property bool    readOnly:                   false
        property int     minFee:                     0
        property int     currency:                   Currency.CurrBeam
        property int     fee:                        BeamGlobals.getDefaultFee(control.currency)
        property string  feeLabel:                   undefined
        property string  color:                      Style.content_main
        readonly property bool isValid:              control.fee >= control.minFee
        property alias underlineVisible:             feeInput.underlineVisible
        property int inputPreferredWidth:            150
        property bool    showSecondCurrency:         false
        property bool    isExchangeRateAvailable:    false
        property string  secondCurrencyAmount:       ""
        property string  secondCurrencyLabel:        ""
        property string  minimumFeeNotificationText: ""

        RowLayout {
            Layout.fillWidth: true

            SFTextInput {
                id:                    feeInput
                Layout.fillWidth:      control.fillWidth && control.underlineVisible && !control.readOnly
                Layout.preferredWidth: inputPreferredWidth
                font.pixelSize:        14
                font.styleName:        "Regular"
                font.weight:           Font.Normal
                color:                 isValid ? control.color : Style.validator_error
                backgroundColor:       isValid ? Style.content_main : Style.validator_error
                selectByMouse:         true
                validator:             IntValidator {bottom: control.minFee}
                readOnly:              control.readOnly

                function formatFee() {
                    return control.fee ? control.fee.toLocaleString(activeFocus ? Qt.locale("C") : Qt.locale(), 'f', -128) : ""
                }

                onTextEdited: {
                    control.fee = text ? parseInt(text) : 0
                    feeInput.text = control.fee
                }
                onActiveFocusChanged: {
                    text = formatFee()
                    if (activeFocus) cursorPosition = positionAt(feeInput.getMousePos().x, feeInput.getMousePos().y)
                }

                Connections {
                    target: control
                    onFeeChanged: feeInput.text = feeInput.formatFee()
                    Component.onCompleted: feeInput.text = feeInput.formatFee()
                    onCurrencyChanged: {
                        control.fee = BeamGlobals.getDefaultFee(control.currency)
                    }
                }
            }

            SFText {
                font.pixelSize: 14
                color:          Style.content_main
                text:           control.feeLabel
                visible:        (control.feeLabel || "").length
            }

            Item {
                visible: !control.underlineVisible && control.readOnly
                Layout.fillWidth: true
            }
        }

        SFText {
            id:               feeInSecondCurrency
            visible:          !minimumFeeNotification.visible && control.showSecondCurrency
            font.pixelSize:   14
            opacity:          control.isExchangeRateAvailable ? 0.5 : 0.7
            color:            control.isExchangeRateAvailable ? Style.content_secondary : Style.accent_fail
            text:             control.isExchangeRateAvailable
                                ? control.secondCurrencyAmount
                                //% "Exchange rate to %1 is not available"
                                : qsTrId("general-exchange-rate-not-available").arg(control.secondCurrencyLabel)
        }


        SFText {
            id:               minimumFeeNotification
            text:             minimumFeeNotificationText.length ?
                                 minimumFeeNotificationText :
                                 //% "The minimum fee is %1 %2"
                                 qsTrId("general-fee-fail").arg(Utils.uiStringToLocale(control.minFee)).arg(control.feeLabel)
            color:            Style.validator_error
            font.pixelSize:   14
            font.italic:      true
            Layout.fillWidth: true
            wrapMode:         Text.WordWrap
            visible:          !control.isValid
        }
}