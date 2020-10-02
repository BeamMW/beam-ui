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

    /*width:  parent.height
    height: parent.width
    anchors.centerIn: parent
    anchors.alignWhenCentered: false

    rotation: 90
    radius:   10
    opacity:  0.3

    gradient: Gradient {
        GradientStop { position: 0.0; color: control.rightColor }
        GradientStop { position: 1.0; color: control.leftColor }
    }
    */

    color: "transparent"
    anchors.fill: parent

    Item {
        /*width:  parent.height
        height: parent.width

        anchors.centerIn: parent
        anchors.alignWhenCentered: false
        rotation: 90
        opacity:  0.8*/
        anchors.fill: parent

        Rectangle {
            id:            borderFill
            radius:        control.radius
            visible:       false
            anchors.fill:  parent

           /* gradient: Gradient {
                GradientStop { position: 0.0; color: control.rightBorderColor }
                GradientStop { position: 1.0; color: control.leftBorderColor }
            }
            */

            gradient: LinearGradient {
                anchors.fill: parent
                start: Qt.point(parent.width, 0)
                end: Qt.point(0, parent.height)
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "white" }
                    GradientStop { position: 1.0; color: "red" }
                }
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
        }
    }

    Rectangle {
        id: filler

        width:  parent.height
        height: parent.width
        anchors.centerIn: parent
        anchors.alignWhenCentered: false

        rotation: 90
        radius:   10
        opacity:  0.3

        gradient: Gradient {
            GradientStop { position: 0.0; color: control.rightColor }
            GradientStop { position: 1.0; color: control.leftColor }
        }
    }
}
