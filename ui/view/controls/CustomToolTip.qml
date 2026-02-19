import QtQuick
import QtQuick.Controls
import "."

ToolTip {
    id: control

    delay:   500
    timeout: 2000
    padding: 8

    contentItem: SFText {
        text:           control.text
        font.pixelSize: 12
        color:          Style.content_main
        wrapMode:       Text.WordWrap
    }

    background: Rectangle {
        color:        Style.background_popup
        radius:       6
        border.width: 1
        border.color: Style.separator
    }
}
