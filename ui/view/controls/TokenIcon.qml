import QtQuick
import "."

// Deterministic, local-only colored-circle icon for an ERC-20 token, matching the
// look of Confidential Asset icons (colored circle + glyph) without any network
// lookup: the color is a hash of the contract address (see beamui::ColorFromString),
// the glyph is just the token symbol's first letter.
Item {
    id: control

    property color  tokenColor: "#8192a3"
    property string symbol:     ""
    property int    size:       26

    width:          size
    height:         size
    implicitWidth:  size
    implicitHeight: size

    Rectangle {
        anchors.fill: parent
        radius:       width / 2
        color:        control.tokenColor
    }

    SFText {
        anchors.centerIn: parent
        text:             control.symbol.length ? control.symbol.charAt(0).toUpperCase() : "?"
        color:            "white"
        font.pixelSize:   control.size * 0.42
        font.bold:        true
    }
}
