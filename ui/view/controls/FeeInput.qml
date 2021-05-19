import QtQuick.Layouts 1.11
import QtQuick 2.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
    id: control

    property int     maxInt:                     2147483647
    property bool    fillWidth:                  false
    property bool    readOnly:                   false
    property int     minFee:                     0
    property int     maxFee:                     0
    property int     recommendedFee:             0
    property int     currency:                   OldWalletCurrency.CurrBeam
    property int     fee:                        BeamGlobals.getDefaultFee(control.currency)
    property string  feeLabel:                   undefined
    property string  color:                      Style.content_main
    readonly property bool isValid:              control.fee >= control.minFee && control.fee >= control.recommendedFee
                                                 && (control.maxFee > 0 ? control.fee <= control.maxFee : true)
    property alias underlineVisible:             feeInput.underlineVisible
    property int inputPreferredWidth:            150
    property bool    showSecondCurrency:         false
    property bool    isExchangeRateAvailable:    false
    property string  rateAmount:                 ""
    property string  rateUnit:                   ""
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
                if (text) {
                    var val = parseInt(text);
                    control.fee = val > maxInt ? control.fee : val;
                } else {
                    control.fee = 0;
                }
                feeInput.text = control.fee
            }

            onActiveFocusChanged: {
                text = formatFee()
                if (activeFocus) cursorPosition = positionAt(feeInput.getMousePos().x, feeInput.getMousePos().y)
            }

            Connections {
                target: control
                Component.onCompleted: feeInput.text = feeInput.formatFee()

                function onFeeChanged () {
                    feeInput.text = feeInput.formatFee()
                }

                function onCurrencyChanged () {
                    control.fee = BeamGlobals.getDefaultFee(control.currency)
                }
            }
        }

        SFText {
            font.pixelSize: 14
            color:          isValid ? Style.content_main : Style.validator_error
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
        visible:          !minimumFeeNotification.visible&& !maximumFeeNotification.visible && control.showSecondCurrency && !recommendedFeeAbsent.visible
        font.pixelSize:   14
        font.italic:      !control.isExchangeRateAvailable
        opacity:          control.isExchangeRateAvailable ? 0.5 : 1
        color:            Style.content_secondary
        text:             control.isExchangeRateAvailable
                            ? control.rateAmount
                            //% "Exchange rate to %1 is not available"
                            : qsTrId("general-exchange-rate-not-available").arg(control.rateUnit)
    }


    SFText {
        id:               minimumFeeNotification
        text:             minimumFeeNotificationText.length ?
                                minimumFeeNotificationText :
                                    control.recommendedFee > 0 ?
                                        //% "The minimum recommended fee is %1 %2"
                                        qsTrId("general-recommended-fee-fail").arg(Utils.uiStringToLocale(control.recommendedFee)).arg(control.feeLabel) :
                                        //% "The minimum fee is %1 %2"
                                        qsTrId("general-fee-fail").arg(Utils.uiStringToLocale(control.minFee)).arg(control.feeLabel)
        color:            Style.validator_error
        font.pixelSize:   14
        font.italic:      true
        Layout.fillWidth: true
        wrapMode:         Text.WordWrap
        visible:          !control.isValid && !recommendedFeeAbsent.visible && !maximumFeeNotification.visible
    }

    SFText {
        id:               maximumFeeNotification
        //% "The maximum fee is %1 %2"
        text:             qsTrId("general-max-fee-fail").arg(Utils.uiStringToLocale(control.maxFee)).arg(control.feeLabel)
        color:            Style.validator_error
        font.pixelSize:   14
        font.italic:      true
        Layout.fillWidth: true
        wrapMode:         Text.WordWrap
        visible:          !control.isValid && control.maxFee > 0 && control.fee > control.maxFee
    }

    SFText {
        Layout.fillWidth:      true
        visible:               control.currency != OldWalletCurrency.CurrBeam && !recommendedFeeAbsent.visible
        font.pixelSize:        14
        font.italic:           true
        wrapMode:              Text.WordWrap
        color:                 Style.content_secondary
        lineHeight:            1.1
        //% "Remember to validate the expected fee rate for the blockchain (as it varies with time)."
        text:                  qsTrId("settings-fee-rate-note")
    }

    SFText {
        id:                    recommendedFeeAbsent
        Layout.fillWidth:      true
        visible:               control.currency != OldWalletCurrency.CurrBeam && control.recommendedFee == 0 && control.fee == 0
        font.pixelSize:        14
        font.italic:           true
        wrapMode:              Text.WordWrap
        color:                 Style.validator_error
        lineHeight:            1.1
/*% "Connection error: can't calculate the recommended fee rate. 
Check on the %1 blockchain by yourself. Low fees might take 
much longer for a transaction to complete."
*/
        text:                  qsTrId("settings-recommended-fee-rate-absent").arg(BeamGlobals.getCurrencyName(control.currency))
    }
}