import QtQuick.Layouts 1.11
import QtQuick 2.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
    id: control

    function getAmountInSecondCurrency() {
        return Utils.formatAmountToSecondCurrency(
            control.amount,
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
            })
        }
        append(Currency.CurrBeam)
        append(Currency.CurrBitcoin)
        append(Currency.CurrLitecoin)
        append(Currency.CurrQtum)
        // TODO disable BCH
        //append(Currency.CurrBitcoinCash)
        append(Currency.CurrDogecoin)
        append(Currency.CurrDash)
        append(Currency.CurrEthereum)
        append(Currency.CurrDai)
        append(Currency.CurrUsdt)
        append(Currency.CurrWrappedBTC)
        return result
    }

    property var               currencies:  defCurrList()
    readonly property bool     isValid:     error.length == 0

    property int      currencyIdx:  currCombo.currentIndex
    readonly property string    currencyUnit:    currencies[currencyIdx].unitName
    readonly property bool      isBeam:          !!currencies[currencyIdx].isBEAM

    property string   rate:     currencies[currencyIdx].rate
    property string   rateUnit: currencies[currencyIdx].rateUnit

    property string   title
    property string   color:        Style.accent_incoming
    property string   currColor:    Style.content_main
    property bool     multi:        false // changing this property in runtime would reset bindings, don't do this
    property string   amount:       "0"

    property alias    error:        errmsg.text
    property bool     readOnlyA:    false
    property bool     resetAmount:  true
    property var      amountInput:  ainput
    property bool     showRate:     control.rateUnit != "" && control.rateUnit != control.currencyUnit
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
            validator:        RegExpValidator {regExp: new RegExp(ainput.getRegExpPattern())}
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
                text = formatDisplayedAmount()
                if (activeFocus) cursorPosition = positionAt(ainput.getMousePos().x, ainput.getMousePos().y)
            }

            function formatDisplayedAmount() {
                if (control.amount == "0") {
                    return ainput.activeFocus ? "": "0"
                }
                return ainput.activeFocus ? control.amount : Utils.uiStringToLocale(control.amount)
            }

            function getRegExpPattern() {
                var pattern = "^(([1-9][0-9]{0,7})|(1[0-9]{8})|(2[0-4][0-9]{7})|(25[0-3][0-9]{6})|(0))(\\.[0-9]{0,%1}[1-9])?$";
                return pattern.arg(BeamGlobals.getCurrencyDecimals(control.currency) - 1);
            }

            Connections {
                target: control
                function onAmountChanged () {
                    ainput.text = ainput.formatDisplayedAmount()
                }
            }
        }

        SFText {
            Layout.topMargin:   22
            font.pixelSize:     24
            font.letterSpacing: 0.6
            color:              error.length ? Style.validator_error : control.currColor
            text:               control.currencyUnit
            visible:            !multi
        }

        CustomComboBox {
            id:                  currCombo
            Layout.topMargin:    22
            Layout.minimumWidth: 95
            dropSpacing:         18
            spacing:             0
            fontPixelSize:       20
            dropFontPixelSize:   14
            dropOffset:          15
            currentIndex:        control.currencyIdx
            color:               error.length ? Style.validator_error : control.currColor
            underlineColor:      "transparent"
            visible:             multi
            model:               control.currencies
            textRole:            "unitName"
            textMaxLenDrop:      10
            textMaxLenDisplay:   5
            enableScroll:        true

            onActivated: {
                if (multi) {
                    ainput.text = "0"
                    control.amount = "0"
                    control.currencyIdx = index
                }
            }

            onModelChanged: {
                // changing model resets index selection, restore
                if (multi) currentIndex = control.currencyIdx
            }
        }
    }

    onCurrencyIdxChanged: {
        if (multi) {
            if (control.currencyIdx != currCombo.currentIndex) {
                currCombo.currentIndex = control.currencyIdx
            }
        }
    }

    //
    // Second
    //
    SFText {
        visible:        control.showRate && !errmsg.visible
        font.pixelSize: 14
        font.italic:    !isExchangeRateAvailable
        opacity:        isExchangeRateAvailable ? 0.5 : 1
        color:          Style.content_secondary
        text:           {
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
        id:                   errmsg
        color:                Style.validator_error
        font.pixelSize:       14
        font.italic:          true
        visible:              error.length
        wrapMode:             "WordWrap"
    }
}
