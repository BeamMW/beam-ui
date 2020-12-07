function formatDateTime(datetime, localeName) {
    var maxTime = new Date(4294967295000);
    if (datetime >= maxTime) {
        //: time never string
        //% "Never"
        return qsTrId("time-never");
    }
    var timeZoneShort = datetime.getTimezoneOffset() / 60 * (-1);
    return datetime.toLocaleDateString(localeName)
         + " | "
         + datetime.toLocaleTimeString(localeName)
         + (timeZoneShort >= 0 ? " (GMT +" : " (GMT ")
         + timeZoneShort
         + ")";
}

function formatSecondCurrency(convertedAmount, amount,  exchangeRate, secondCurrLabel) {
    if (convertedAmount == "0" && amount != "0") { 
        var subLabel = BeamGlobals.getCurrencySubunitFromLabel(secondCurrLabel);
        //% "< 1 %1"
        return qsTrId("format-small-amount").arg(subLabel); 
    }
    if (convertedAmount != "") {
        //% "%1 %2"
        return qsTrId("format-amount").arg(Utils.uiStringToLocale(convertedAmount)).arg(secondCurrLabel);
    }
    return "";
}

function formatAmountToSecondCurrency(amount, exchangeRate, secondCurrLabel) {
    var convertedAmount = BeamGlobals.calcAmountInSecondCurrency(amount, exchangeRate, secondCurrLabel);
    return formatSecondCurrency(convertedAmount, amount, exchangeRate, secondCurrLabel);
}

function formatFeeToSecondCurrency(amount, exchangeRate, secondCurrLabel) {
    if (exchangeRate == "0") {
        return "- " + secondCurrLabel;
    }
    var convertedAmount = BeamGlobals.calcFeeInSecondCurrency(amount, exchangeRate, secondCurrLabel);
    return formatSecondCurrency(convertedAmount, amount, exchangeRate, secondCurrLabel);
}

// @arg amount - any number or float string in "C" locale
function uiStringToLocale (amount) {
    var locale = Qt.locale();
    var parts  = amount.toString().split(".");

    // Trim leading zeros
    var intPart = parseInt(parts[0], 10);
    var left = isNaN(intPart) ? parts[0] : intPart.toString();

    left = left.replace(/(\d)(?=(?:\d{3})+\b)/g, "$1" + locale.groupSeparator);
    return parts[1] ? [left, parts[1]].join(locale.decimalPoint) : left;
}

function localeDecimalToCString(amount) {
    var locale = Qt.locale();
    return amount
        .split(locale.groupSeparator)
        .join('')
        .split(locale.decimalPoint)
        .join('.');
}

function isSqueezedHeight(parentHeight) {
    return parentHeight <= 768;
}

function getLogoTopGapSize(parentHeight) {
    return parentHeight * (isSqueezedHeight(parentHeight) ? 0.13 : 0.18)
}

function openExternal(externalLink, settings, dialog, onFinish) {
    var onFinishCallback = onFinish && (typeof onFinish === "function")
        ? onFinish
        : function () {};
    if (settings.isAllowedBeamMWLinks) {
        Qt.openUrlExternally(externalLink);
        onFinishCallback();
    } else {
        dialog.externalUrl = externalLink;
        dialog.onOkClicked = function () {
            settings.isAllowedBeamMWLinks = true;
            onFinishCallback();
        };
        dialog.onCancelClicked = function() {
            onFinishCallback();
        };
        dialog.open();
    }
}

function openExternalWithConfirmation(externalLink, onFinish) {
    var settingsViewModel = Qt.createQmlObject("import Beam.Wallet 1.0; SettingsViewModel {}", main);
    var component = Qt.createComponent("controls/OpenExternalLinkConfirmation.qml");
    var externalLinkConfirmation = component.createObject(main);
    Utils.openExternal(
        externalLink,
        settingsViewModel,
        externalLinkConfirmation, onFinish);
}

function navigateToDownloads() {
    openExternalWithConfirmation("https://www.beam.mw/downloads")
}

function currenciesList() {
    return [
        BeamGlobals.getCurrencyLabel(Currency.CurrBeam),
        BeamGlobals.getCurrencyLabel(Currency.CurrBitcoin),
        BeamGlobals.getCurrencyLabel(Currency.CurrLitecoin),
        BeamGlobals.getCurrencyLabel(Currency.CurrQtum),
        // TODO disable BCH
        //BeamGlobals.getCurrencyLabel(Currency.CurrBitcoinCash),
        BeamGlobals.getCurrencyLabel(Currency.CurrDogecoin),
        BeamGlobals.getCurrencyLabel(Currency.CurrDash),
        BeamGlobals.getCurrencyLabel(Currency.CurrEthereum),
        BeamGlobals.getCurrencyLabel(Currency.CurrDai),
        BeamGlobals.getCurrencyLabel(Currency.CurrTether),
        BeamGlobals.getCurrencyLabel(Currency.CurrWrappedBTC)
    ]
}

const maxAmount   = "262799999";
const minAmount   = "0.00000001";

function createObject(component, parent, props) {
    return Qt.createComponent(component)
             .createObject(parent, props);
}

function createControl(component, parent, props) {
    return createObject(["controls/", component, ".qml"].join(""), parent, props)
}

function createDialog(component, props) {
    return createControl([component, "Dialog"].join(''), main, props)
}

function openDialog(component, props) {
    createDialog(component, props).open()
}

function formatHours(hours) {
    var dd = Math.floor(hours / 24);
    var hh = hours;
    if (dd) {
        hh = hours - dd * 24;
    }

    if (hh == 1) {
        //: time "hour" string
        //% "hour"
        hh = hh + " " + qsTrId("time-hour");
    } else if (hh == 0){
        hh = "";
    } else {
        //: time "hours" string
        //% "hours"
        hh = hh + " " + qsTrId("time-hours");
    }

    if (dd) {
        if (dd == 1) {
            //: time "day" string
            //% "day"
            dd = dd + " " + qsTrId("time-day");
        } else {
            //: time "days" string
            //% "days"
            dd = dd + " " + qsTrId("time-days");
        }

        return dd + " " + hh;

    } else {
        return hh;
    }
}
