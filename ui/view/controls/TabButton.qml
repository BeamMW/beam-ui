import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.15
import "."

TxFilter {
    showLed:        false
    opacity:        this.state != "active" ? 0.5 : 1
    activeColor:    Style.active
    inactiveColor:  Style.content_main

    font {
        styleName:      "DemiBold"
        weight:         Font.DemiBold
        pixelSize:      14
        letterSpacing:  3.11
        capitalization: Font.AllUppercase
    }
}