import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
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
                color:  Qt.rgba(33/255, 71/255, 99/255, 0.7)
                radius: 10
            }
            Rectangle {
                anchors.fill: parent
                color: defBkColor
                radius: 10
            }
        }
        FastBlur {
            anchors.fill:       back
            source:             back
            radius:             4
            transparentBorder:  true
        }
    }
}
