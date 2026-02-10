import QtQuick
import Qt5Compat.GraphicalEffects
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