import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    id: control

    property color color:               Style.online
    property int circleLineWidth:       2
    property int animationDuration:     2000
    property int radius:                5

    implicitWidth:  2 * (radius + circleLineWidth)
    implicitHeight: 2 * (radius + circleLineWidth)

    Canvas {
        id: canvas_
        anchors.fill: parent
        onPaint: {
            var context = getContext("2d");
            context.arc(width/2, height/2, width/2 - control.circleLineWidth, 0, 1.6 * Math.PI);
            context.strokeStyle = control.color;
            context.lineWidth = control.circleLineWidth;
            context.stroke();
        }
    }

    RotationAnimator {
        target:     control
        from: 0
        to: 360
        duration:   control.animationDuration
        running:    control.visible
        loops:      Animation.Infinite
    }
}