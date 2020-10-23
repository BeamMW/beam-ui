import QtQuick.Layouts 1.11
import QtQuick 2.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
    id: control

    function getTotalFeeTitle() {
        //% "%1 Transaction fee (est)"
        return qsTrId("general-fee-total").arg(control.currencyUnit)
    }

    function getFeeInSecondCurrency(feeValue) {
        return Utils.formatFeeToSecondCurrency(
            feeValue,
            control.rate,
            control.rateUnit);
    }

    function getAmountInSecondCurrency() {
        return Utils.formatAmountToSecondCurrency(
            control.amountIn,
            control.rate,
            control.rateUnit);
    }

    function clearFocus() {
        ainput.focus = false
    }

    // TODO: get rid of the "Currency" enum completely
    function defCurrList () {
        var result = []
        var append = function (currid) {
            result.push({
                "isBEAM":         currid == Currency.CurrBeam,
                "unitName":       BeamGlobals.getCurrencyUnitName(currid),
                "defaultFee":     BeamGlobals.getDefaultFee(currid),
                "recommededFee":  BeamGlobals.getRecommendedFee(currid),
                "minimumFee":     BeamGlobals.getMinimalFee(currid, false),
                "feeLabel":       BeamGlobals.getFeeRateLabel(currid, false),
                "calcTotalFee":   function(fee) {return BeamGlobals.calcTotalFee(currid, fee)}
            })
        }
        append(Currency.CurrBeam)
        append(Currency.CurrBitcoin)
        append(Currency.CurrLitecoin)
        append(Currency.CurrQtum)
        append(Currency.CurrBitcoinCash)
        append(Currency.CurrBitcoinSV)
        append(Currency.CurrDogecoin)
        append(Currency.CurrDash)
        return result
    }

    property var               currencies:  defCurrList()
    readonly property bool     isValidFee:  hasFee ? feeInput.isValid : true
    readonly property bool     isValid:     error.length == 0 && isValidFee

    property int               currencyIdx:     0
    readonly property string   currencyUnit:    currencies[currencyIdx].unitName
    readonly property bool     isBeam:          !!currencies[currencyIdx].isBEAM
    readonly property string   defaultFee:      currencies[currencyIdx].defaultFee
    readonly property string   recommendedFee:  currencies[currencyIdx].recommededFee
    readonly property string   minimumFee:      currencies[currencyIdx].minimumFee
    readonly property string   feeLabel:        currencies[currencyIdx].feeLabel
    readonly property var      calcTotalFee:    currencies[currencyIdx].calcTotalFee

    property string   title
    property string   color:        Style.accent_incoming
    property string   currColor:    Style.content_main
    property bool     hasFee:       false
    property bool     currFeeTitle: false
    property bool     multi:        false // changing this property in runtime would reset bindings, don't do this

    property string   amount:       "0"

    // TODO: this is insanely bad, fix and never do it again
    property string   amountIn:     "0"  // public property for binding. Use it to avoid binding overriding

    property int      fee:          defaultFee
    property alias    error:        errmsg.text
    property bool     readOnlyA:    false
    property bool     readOnlyF:    false
    property bool     resetAmount:  true
    property var      amountInput:  ainput
    property bool     showTotalFee: false
    property string   rate:         "0"
    property string   rateUnit:     ""
    property bool     showSecondCurrency: control.rateUnit != "" && control.rateUnit != control.currencyUnit
    readonly property bool isExchangeRateAvailable: control.rate != "0"

    SFText {
        font.pixelSize:   14
        font.styleName:   "Bold"
        font.weight:      Font.Bold
        color:            Style.content_main
        text:             control.title
        visible:          text.length > 0
    }

    RowLayout {
        Layout.fillWidth: true

        SFTextInput {
            id:               ainput
            Layout.fillWidth: true
            font.pixelSize:   36
            font.styleName:   "Light"
            font.weight:      Font.Light
            color:            error.length ? Style.validator_error : control.color
            backgroundColor:  error.length ? Style.validator_error : Style.content_main
            validator:        RegExpValidator {regExp: /^(([1-9][0-9]{0,7})|(1[0-9]{8})|(2[0-4][0-9]{7})|(25[0-3][0-9]{6})|(0))(\.[0-9]{0,7}[1-9])?$/}
            selectByMouse:    true
            text:             formatDisplayedAmount()
            readOnly:         control.readOnlyA

            onTextChanged: {
                // if nothing then "0", remove insignificant zeroes and "." in floats
                if (ainput.activeFocus) {
                    control.amount = text ? text.replace(/\.0*$|(\.\d*[1-9])0+$/,'$1') : "0"
                }
            }

            onActiveFocusChanged: {
                // we intentionally break binding here
                text = formatDisplayedAmount()
                if (activeFocus) cursorPosition = positionAt(ainput.getMousePos().x, ainput.getMousePos().y)
            }

            function formatDisplayedAmount() {
                return control.amountIn == "0" ? "" : (ainput.activeFocus ? control.amountIn : Utils.uiStringToLocale(control.amountIn))
            }

            Connections {
                target: control
                onAmountInChanged: {
                    if (!ainput.activeFocus) {
                        // we intentionally break binding here
                        ainput.text = ainput.formatDisplayedAmount()
                    }
                }
            }
        }

        SFText {
            Layout.topMargin:   22
            font.pixelSize:     24
            font.letterSpacing: 0.6
            color:              control.currColor
            text:               control.currencyUnit
            visible:            !multi
        }

        CustomComboBox {
            id:                  currCombo
            Layout.topMargin:    22
            Layout.minimumWidth: 95
            spacing:             0
            fontPixelSize:       24
            fontLetterSpacing:   0.6
            currentIndex:        control.currencyIdx
            color:               control.currColor
            visible:             multi
            model:               control.currencies
            textRole:            "unitName"

            onActivated: {
                if (multi) control.currencyIdx = index
                if (resetAmount) control.amount = 0
            }
        }
    }

    //
    // Second
    //
    SFText {
        id:             amountSecondCurrencyText
        visible:        control.showSecondCurrency && !errmsg.visible /* && !showTotalFee*/  // show only on send side
        font.pixelSize: 14
        font.italic:    !isExchangeRateAvailable
        opacity:        isExchangeRateAvailable ? 0.5 : 1
        color:          isExchangeRateAvailable ? Style.content_secondary : Style.accent_fail
        text:           {
            if (showTotalFee)
                return ""
            if (isExchangeRateAvailable)
                return getAmountInSecondCurrency()
            //% "Exchange rate to %1 is not available"
            return qsTrId("general-exchange-rate-not-available").arg(control.rateUnit)
        }
    }

    //
    // error
    //
    SFText {
        Layout.fillWidth:     true
        Layout.minimumHeight: 35
        id:                   errmsg
        color:                Style.validator_error
        font.pixelSize:       14
        font.italic:          true
        visible:              error.length
        wrapMode:             "WordWrap"
    }


    //
    // Fee
    //
    GridLayout {
        columns:       2
        Layout.topMargin: 50
        visible:       control.hasFee
        ColumnLayout {
            Layout.maximumWidth:  198
            Layout.alignment:     Qt.AlignTop
            visible:              control.hasFee

            SFText {
                font.pixelSize:   14
                font.styleName:   "Bold"
                font.weight:      Font.Bold
                color:            Style.content_main
                text: {
                    if (control.isBEAM) {
                        return control.currFeeTitle ?
                            //% "BEAM Transaction fee"
                            qsTrId("beam-transaction-fee") :
                            //% "Transaction fee"
                            qsTrId("general-fee")
                    }
                    //% "%1 Transaction fee rate"
                    return qsTrId("general-fee-rate").arg(control.currencyUnit)
                }
            }

            FeeInput {
                id:                       feeInput
                Layout.fillWidth:         true
                fee:                      control.fee
                recommendedFee:           control.recommendedFee
                minFee:                   control.minimumFee
                feeLabel:                 control.feeLabel
                color:                    control.color
                readOnly:                 control.readOnlyF
                showSecondCurrency:       control.showSecondCurrency
                isExchangeRateAvailable:  control.isExchangeRateAvailable
                rateAmount:               getFeeInSecondCurrency(control.fee)
                rateUnit:                 control.rateUnit
                Connections {
                    target: control
                    onFeeChanged: feeInput.fee = control.fee
                    onCurrencyChanged: feeInput.fee = control.defaultFee
                }
            }
        }
       
        ColumnLayout {
            Layout.alignment:     Qt.AlignLeft | Qt.AlignTop
            visible:              showTotalFee && control.hasFee && !control.isBEAM
            SFText {
                font.pixelSize:   14
                font.styleName:   "Bold"
                font.weight:      Font.Bold
                color:            Style.content_main
                text:             getTotalFeeTitle()
            }
            SFText {
                id:               totalFeeLabel
                Layout.topMargin: 6
                font.pixelSize:   14
                color:            Style.content_main
                text:             control.calcTotalFee(control.fee)
            }
            SFText {
                id:               feeTotalInSecondCurrency
                visible:          control.showSecondCurrency && control.isExchangeRateAvailable
                Layout.topMargin: 6
                font.pixelSize:   14
                opacity:          0.5
                color:            Style.content_secondary
                text:             getFeeInSecondCurrency(parseInt(totalFeeLabel.text, 10))
            }
        }
    }

    SFText {
        enabled:               control.hasFee && !control.isBEAM
        visible:               enabled
        Layout.topMargin:      20
        Layout.preferredWidth: 370
        font.pixelSize:        14
        font.italic:           true
        wrapMode:              Text.WordWrap
        color:                 Style.content_secondary
        lineHeight:            1.1
        //% "Remember to validate the expected fee rate for the blockchain (as it varies with time)."
        text:                  qsTrId("settings-fee-rate-note")
    }

    Binding {
        target:   control
        property: "fee"
        value:    feeInput.fee
    }
}
