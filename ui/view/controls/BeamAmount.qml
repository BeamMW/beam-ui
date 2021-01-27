import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../utils.js" as Utils

Control {
    id: control
    spacing: 8

    property string  amount:              "0"
    property string  unitName:            BeamGlobals.beamUnit
    property string  rateUnit:            ""
    property string  rate:                "0"
    property string  color:               Style.content_main
    property bool    error:               false
    property bool    showZero:            true
    property bool    showDrop:            false
    property int     fontSize:            14
    property int     rateFontSize:        10
    property bool    lightFont:           true
    property bool    boldFont:            false
    property string  iconSource:          ""
    property size    iconSize:            Qt.size(0, 0)
    property alias   copyMenuEnabled:     amountText.copyMenuEnabled
    property alias   caption:             captionText.text
    property int     captionFontSize:     12
    property string  prefix:              ""
    property alias   fontSizeMode:        amountText.fontSizeMode
    property real    maxPaintedWidth:     0
    property int     maxUnitChars:        0
    property real    vSpacing:            5
    property var     tipParent:           Overlay.overlay

    function formatRate () {
        var formatted = Utils.formatAmountToSecondCurrency(control.amount, control.rate, control.rateUnit);
        return formatted == "" ?  ["-", control.rateUnit].join(" ") : control.prefix + formatted;
    }

    TextMetrics {
        id:     metrics
        font:   amountText.font
        elide:  Qt.ElideNone
    }

    function formatAmount (amount, uname) {
        return Utils.formatAmount(amount, uname, prefix, showZero)
    }

    function calcMaxTextWidth () {
        var pos = control.mapFromItem(amountText.parent, amountText.x, amountText.y)
        return control.maxPaintedWidth - pos.x - (showDrop ? drop.width + amountText.parent.spacing : 0)
    }

    function fitText () {
        return Utils.fitAmount(control.amount,
                               control.unitName,
                               control.prefix,
                               control.showZero,
                               metrics,
                               calcMaxTextWidth()
        )
    }

    onAmountChanged: {
        if (maxPaintedWidth) amountText.text = fitText()
    }

    onUnitNameChanged: {
        if (maxPaintedWidth) amountText.text = fitText()
    }

    onMaxPaintedWidthChanged: {
        if (maxPaintedWidth) amountText.text = fitText()
        else amountText.text = Qt.binding(function () {
                                            return formatAmount(control.amount, Utils.limitText(control.unitName, control.maxUnitChars))
                                          })
    }

    onWidthChanged: {
        if (maxPaintedWidth) amountText.text = fitText()
    }

    AlphaTip {
        id: tip

        visible:      amountTextArea.containsMouse && tipText.text != amountText.text
        defBkColor:   Qt.rgba(55 / 255, 93  / 255, 123 / 255, 0.75)
        defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
        parent:       control.tipParent

        x: {
            var xpos = Utils.xUp(amountText) + amountText.width / 2 - tip.width / 2
            if (xpos < 0) return 0
            return xpos + tip.width > root.width ? Utils.xUp(amountText) + amountText.width - tip.width : xpos
        }

        y: Utils.yUp(amountText) + amountText.height + 5

        contentItem: SFText {
            id:             tipText
            text:           formatAmount(control.amount, control.unitName)
            font.pixelSize: 12
            font.styleName: "Light"
            font.weight:    Font.Light
            color:          tip.defTextColor
        }
    }

    contentItem: Row {
        spacing: control.spacing

        SvgImage {
            id:          assetIcon
            source:      control.iconSource
            sourceSize:  control.iconSize
            visible:     !!control.iconSource
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            spacing: control.vSpacing

            SFLabel {
                id:             captionText
                visible:        text.length > 0
                font.pixelSize: captionFontSize
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.7)
            }

            Row {
                spacing: 5

                SFLabel {
                    id:               amountText
                    font.pixelSize:   fontSize
                    font.styleName:   lightFont ? "Light" : (boldFont ? "Bold" : "Regular")
                    font.weight:      lightFont ? Font.Light : (boldFont ? Font.Bold : Font.Normal)
                    color:            control.error ? Style.validator_error : control.color
                    onCopyText:       BeamGlobals.copyToClipboard(amount)
                    copyMenuEnabled:  true
                    text:             control.maxPaintedWidth ? fitText() : formatAmount(control.amount, Utils.limitText(control.unitName, control.maxUnitChars))

                    MouseArea {
                        id: amountTextArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                SvgImage {
                    visible:    showDrop
                    source:     "qrc:/assets/icon-down.svg"
                    sourceSize: Qt.size(5, 3)
                    id: drop
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            SFLabel {
                id:              secondCurrencyAmountText
                visible:         rateUnit.length > 0
                font.pixelSize:  control.rateFontSize
                font.styleName:  "Regular"
                font.weight:     Font.Normal
                color:           control.error ? Style.validator_error : Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                text:            formatRate()
                onCopyText:      BeamGlobals.copyToClipboard(secondCurrencyAmountText.text)
                copyMenuEnabled: true
            }
        }
    }
}