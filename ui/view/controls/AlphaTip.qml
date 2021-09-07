import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12

Control {
    id:            control
    leftPadding:   14
    rightPadding:  14
    topPadding:    14
    bottomPadding: 14

    property color defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.85)
    property color defBkColor: Qt.rgba(1, 1, 1, 0.10)

    background: Item {
        Item {
            id: back
            anchors.fill: parent
            Rectangle {
                anchors.fill: parent
                color:  Qt.rgba(Style.background_main.r, Style.background_main.g, Style.background_main.b, 0.95)
                radius: 10
            }
            Rectangle {
                anchors.fill: parent
                color: defBkColor
                radius: 10
            }
        }
    }
}
