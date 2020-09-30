import QtQuick 2.11

Rectangle {
    id: control

    property color leftColor
    property color rightColor

    width:  parent.height
    height: parent.width
    anchors.centerIn: parent
    anchors.alignWhenCentered: false

    rotation: 90
    radius:   10
    opacity:  0.3

    border {
        width: control.borderSize
        color: control.borderColor
    }

    gradient: Gradient {
        GradientStop { position: 0.0; color: control.rightColor }
        GradientStop { position: 1.0; color: control.leftColor }
    }
}
