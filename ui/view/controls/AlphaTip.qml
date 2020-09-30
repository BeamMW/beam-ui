import QtQuick 2.11
import QtQuick.Controls 2.4

Control {
    id:            control
    leftPadding:   14
    rightPadding:  14
    topPadding:    13
    bottomPadding: 13

    property color defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)

    background: Rectangle {
        anchors.fill: parent
        color:  Qt.rgba(255, 255, 255, 0.15)
        radius: 10
    }
}
