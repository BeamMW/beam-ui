import QtQuick
import QtQuick.Controls
import "."

ScrollBar {
    id: control

    policy: ScrollBar.AsNeeded

    contentItem: Rectangle {
        implicitWidth:  6
        implicitHeight: 6
        radius:         3
        color:          Style.content_main
        opacity:        control.pressed ? 0.3 : (control.hovered ? 0.2 : 0.12)
        visible:        control.size < 1.0
    }

    background: Rectangle {
        implicitWidth:  6
        implicitHeight: 6
        color:          "transparent"
    }
}
