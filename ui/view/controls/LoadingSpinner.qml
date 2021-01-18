import QtQuick 2.11

Item {
    id: root
    property int radius:        30
    property int elementsCount: 12
    property color color:       "#FFFFFF"
    property alias running:     timer.running

    property int innerRadius:   24
    property int currentIndex:  0

    width:  radius * 2
    height: radius * 2

    Repeater {
        id: repeater
        model: root.elementsCount
        delegate: Component {
            Rectangle {
                property int angle:    (360 / repeater.model) * index
                property int maxIndex: root.currentIndex + 3
                property int minIndex: root.currentIndex

                width:   root.radius - root.innerRadius
                height:  width
                x:       getPosOnCircle(angle).x
                y:       getPosOnCircle(angle).y
                radius:  width
                color:   root.color
                // TODO: refactor
                opacity: (index >= minIndex && index <= maxIndex) || (index === 0 && root.currentIndex + 1 > repeater.model - 1) ? 1 : 0.1

                Behavior on opacity { NumberAnimation { duration: 500 } }

                function toRadian(degree) {
                    return (degree * 3.14159265) / 180.0;
                }

                function getPosOnCircle(angleInDegree) {
                    var centerX = root.width / 2;
                    var centerY = root.height / 2;
                    var posX = 0;
                    var posY = 0;

                    posX = centerX + root.innerRadius * Math.cos(toRadian(angleInDegree));
                    posY = centerY - root.innerRadius * Math.sin(toRadian(angleInDegree));
                    return Qt.point(posX, posY);
                }
            }
        }
    }

    Timer {
        id:       timer
        interval: 300
        repeat:   true
        running:  true
        onTriggered: {
            if (root.currentIndex === 0) {
                root.currentIndex = repeater.model - 1;
            }
            else {
                root.currentIndex--;
            }
        }
    }
}