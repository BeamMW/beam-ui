import QtQuick.Layouts
import QtQuick
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

    property var               currencies
    readonly property bool     isValid: error.length == 0

    property int                currencyIdx:   currCombo.currentIndex
    readonly property string    currencyUnitNoId:  currencies[currencyIdx].unitName
    readonly property string    currencyUnit:  currencies[currencyIdx].unitName +
                                    ((currencies[currencyIdx].assetId > 0) ? " (%1)".arg(currencies[currencyIdx].assetId) : "")
    readonly property bool      isBeam:        !!currencies[currencyIdx].isBEAM

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
    property bool     showRate:     control.rateUnit != "" && control.rateUnit != control.currencyUnitNoId
    readonly property bool isExchangeRateAvailable: control.rate != "0"
    property alias    filterAssets: currCombo.filterAssets
    property bool     activeFieldIsSecond: false

    // the editable USD row stays inside the box whenever a rate is available (kept on error too,
    // so the field doesn't change shape when insufficient-funds validation kicks in)
    readonly property bool showSecond: control.showRate && control.isExchangeRateAvailable

    SFText {
        font.pixelSize:   14
        font.styleName:   "Bold"
        font.weight:      Font.Bold
        color:            Style.content_main
        text:             control.title
        visible:          text.length > 0
    }

    onCurrencyIdxChanged: {
        if (multi) {
            if (control.currencyIdx != currCombo.currentIndex) {
                currCombo.currentIndex = control.currencyIdx
            }
        }
    }

    // Single amount box: BEAM row + hairline divider + USD row (mock: one field, two sections).
    Item {
        Layout.fillWidth: true
        implicitHeight: amountBox.implicitHeight

        // one shared rounded background for the whole field (individual inputs are boxless)
        Rectangle {
            anchors.fill: parent
            radius: 10
            color: Style.content_main
            opacity: (ainput.activeFocus || secondInput.activeFocus || ainput.hovered) ? 0.1 : 0.05
        }

        ColumnLayout {
            id: amountBox
            anchors.fill: parent
            spacing: 0

            // ----- primary (BEAM/asset) row -----
            Item {
                Layout.fillWidth: true
                Layout.leftMargin:  16
                Layout.rightMargin: 8
                implicitHeight: ainput.implicitHeight

                SFTextInput {
                    id:               ainput
                    anchors.left:     parent.left
                    anchors.right:    parent.right
                    underlineVisible: false
                    font.pixelSize:   36
                    font.styleName:   "Light"
                    font.weight:      Font.Light
                    // accent when the BEAM field is active, gray when the USD field is
                    color:            !isValid ? Style.validator_error : (control.activeFieldIsSecond ? Style.content_secondary : control.color)
                    backgroundColor:  !isValid ? Style.validator_error : Style.content_main
                    validator:        RegularExpressionValidator {regularExpression: new RegExp(ainput.getRegExpPattern())}
                    text:             formatDisplayedAmount()
                    readOnly:         control.readOnlyA
                    rightPadding:     currCombo.width

                    function stripAmountText() {
                        return text ? text.replace(/\.0*$|(\.\d*[1-9])0+$/,'$1') : "0"
                    }

                    onTextChanged: {
                        // if nothing then "0", remove insignificant zeroes and "." in floats
                        if (ainput.activeFocus) {
                            control.activeFieldIsSecond = false   // set BEFORE amount so the USD field re-derives
                            control.amount = stripAmountText()
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
                        // Allow integers up to 12 digits (max safe for uint64 with 8 decimals).
                        // Exact overflow protection is handled by the C++ backend (decode_base10)
                        // and the isEnoughAmount / balance validation.
                        var pattern = "^([1-9][0-9]{0,11}|0)(\\.[0-9]{0,%1}[1-9])?$";
                        return pattern.arg(currencies[currencyIdx].decimals - 1);
                    }

                    Connections {
                        target: control
                        function onAmountChanged () {
                            var formatted = ainput.formatDisplayedAmount()

                            if (!ainput.activeFocus)
                            {
                                ainput.text = formatted
                            }
                            else {
                                if (formatted && formatted != ainput.stripAmountText()) {
                                    // we tolerate only insignificants 0 at the end of floats
                                    // so if user entered 0.000100 we do not strip last 2 zeroes at the end while in focus
                                    BeamGlobals.fatal("Absolute value of the amount should not be changed while control is in focus")
                                }
                            }
                        }
                    }
                }

                CustomComboBox {
                    id:                  currCombo
                    x:                   ainput.width - currCombo.width
                    y:                   10
                    dropSpacing:         18
                    spacing:             0
                    fontPixelSize:       20
                    dropFontPixelSize:   14
                    currentIndex:        control.currencyIdx
                    color:               !isValid ? Style.validator_error : control.currColor
                    // coin icon/name dim to ~50% when the USD field is active
                    opacity:             control.activeFieldIsSecond ? 0.5 : 1.0
                    underlineColor:      "transparent"
                    enabled:             multi
                    colorConst:          true
                    model:               control.currencies
                    textRole:            "unitNameWithId"
                    textMaxLenDrop:      10
                    enableScroll:        true
                    showBackground:      false
                    leftPadding:         30
                    maxTextWidth:        100
                    dropDownIconSixe:    Qt.size(9, 5)
                    dropDownIconRightMargin: 14
                    //% "Enter asset name..."
                    searchPlaseholder: qsTrId("amount-input-asset-search")

                    onActivated: function(index) {
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

                    onHoveredChanged: {
                        ainput.highlight = currCombo.hovered;
                    }
                }
            }

            // ----- hairline divider between the two sections -----
            Rectangle {
                visible:             control.showSecond
                Layout.fillWidth:    true
                Layout.leftMargin:   16
                Layout.rightMargin:  16
                implicitHeight:      1
                color:               Style.content_secondary
                opacity:             0.25
            }

            // ----- second (USD) row -----
            RowLayout {
                visible:            control.showSecond
                Layout.fillWidth:   true
                Layout.leftMargin:  16
                Layout.rightMargin: 16
                Layout.topMargin:   6
                Layout.bottomMargin: 8
                spacing:            6

                SFTextInput {
                    id:             secondInput
                    property string userText: ""   // raw text the user typed; echoed verbatim while USD drives the amount
                    underlineVisible: false
                    Layout.fillWidth: true
                    font.pixelSize: 14
                    enabled:        control.isExchangeRateAvailable && !control.readOnlyA
                    // accent when the USD field is active, gray otherwise
                    color:          control.activeFieldIsSecond ? Style.accent_incoming : Style.content_secondary
                    validator:      RegularExpressionValidator { regularExpression: /^[0-9]*[.,]?[0-9]*$/ }
                    // While the USD field is the active driver, echo the user's own text verbatim. Do NOT
                    // re-derive it from control.amount here: calcAmountInSecondCurrency re-rounds to USD's 2
                    // decimals, so a sub-cent entry like "0.0001" would collapse to "0" the moment the BEAM
                    // amount updates. When BEAM drives, show the converted (display) value.
                    text:           control.activeFieldIsSecond
                                      ? secondInput.userText
                                      : (control.isExchangeRateAvailable
                                          ? BeamGlobals.calcAmountInSecondCurrency(control.amount, control.rate, control.rateUnit)
                                          : "")
                    onTextEdited: {
                        control.activeFieldIsSecond = true
                        secondInput.userText = secondInput.text
                        control.amount = Utils.calcAmountFromSecondCurrency(secondInput.text, control.rate, currencies[currencyIdx].decimals)
                    }
                    Connections {
                        target: control
                        function onAmountChanged() {
                            // re-derive the USD display only when BEAM (not this field) is the active driver
                            if (!control.activeFieldIsSecond)
                                secondInput.text = control.isExchangeRateAvailable
                                    ? BeamGlobals.calcAmountInSecondCurrency(control.amount, control.rate, control.rateUnit)
                                    : ""
                        }
                    }
                }
                SFText {
                    text:           control.rateUnit
                    font.pixelSize: 14
                    // "USD" turns white when the USD field is active, gray otherwise
                    color:          control.activeFieldIsSecond ? Style.content_main : Style.content_secondary
                }
            }
        }
    }

    SFText {
        visible:        control.showRate && !errmsg.visible && !control.isExchangeRateAvailable
        font.pixelSize: 14
        font.italic:    true
        color:          Style.content_secondary
        //% "Exchange rate to %1 is not available"
        text:           qsTrId("general-exchange-rate-not-available").arg(control.rateUnit)
    }

    //
    // error
    //
    SFText {
        Layout.fillWidth:     true
        Layout.leftMargin:    16   // align with the box content inset
        Layout.rightMargin:   16
        Layout.topMargin:     6
        id:                   errmsg
        color:                Style.validator_error
        font.pixelSize:       14
        font.italic:          true
        visible:              !isValid
        wrapMode:             "WordWrap"
    }
}
