import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.1

Control {
    id:            control

    leftPadding:   14
    rightPadding:  14
    topPadding:    13
    bottomPadding: 13

    property color defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
    property color defBkColor: Qt.rgba(1, 1, 1, 0.10)

    background: Item {
        Rectangle {
            anchors.fill: parent
            color: defBkColor
            radius: 10
            id: back
        }

        FastBlur {
            anchors.fill: back
            source: back
            radius: 4
            transparentBorder: true
        }
    }
}
