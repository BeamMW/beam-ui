import QtQuick 2.11
import QtGraphicalEffects 1.1

Rectangle {
    id: control

    property color leftColor
    property color rightColor
    property color leftBorderColor
    property color rightBorderColor
    property int   radius: 10
    property int   borderWidth: 0

    color: "transparent"

    Rectangle {
       width:  parent.height
       height: parent.width
       anchors.centerIn: parent
       anchors.alignWhenCentered: false

       rotation: 90
       radius:   control.radius
       opacity:  0.3

       gradient: Gradient {
            GradientStop { position: 0.0; color: control.rightColor }
            GradientStop { position: 1.0; color: control.leftColor }
       }
    }

    LinearGradient {
        id:            borderFill
        anchors.fill:  parent
        start:         Qt.point(parent.width, 0)
        end:           Qt.point(parent.height, 0)
        visible:       false

        gradient: Gradient {
            GradientStop { position: 0.0; color: control.rightBorderColor }
            GradientStop { position: 1.0; color: control.leftBorderColor }
        }
    }

   Rectangle {
        id:            broderMask
        radius:        control.radius
        border.width:  control.borderWidth
        anchors.fill:  parent
        color:         "transparent"
        visible:       false
   }

   OpacityMask {
       anchors.fill: parent
       source:       borderFill
       maskSource:   broderMask
       visible:      control.borderWidth != 0
   }
}
