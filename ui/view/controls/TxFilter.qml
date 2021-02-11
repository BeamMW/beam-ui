import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

Item  {
    id: root
    
    property string label
    property var capitalization: Font.AllUppercase
    property alias font : text_label.font
    property bool showLed: true

    width: showLed ? led.width : text_label.width
    height: 20
    state: "normal"
    signal clicked()

    SFText {
        id: text_label

        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
        font.styleName: "DemiBold"; font.weight: Font.DemiBold
        font.capitalization: capitalization
        text: label
        color: Style.section

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.clicked()
        }
    }

    Rectangle {
        id: led

        anchors.bottom: parent.bottom
        width: text_label.width + 16*2
        height: 2
        color: Style.active
        visible: false
    }

    DropShadow {
        anchors.fill: led
        radius: 5
        samples: 9
        color: Style.active
        source: led

        visible: led.visible
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {target: text_label; font.weight: showLed ? Font.Normal : Font.Bold}
            PropertyChanges {target: text_label; font.styleName: showLed ? "Regular" : "Bold"}
        },
        State {
            name: "active"

            PropertyChanges {target: led; visible: showLed}
            PropertyChanges {target: text_label; opacity: 1.0}
            PropertyChanges {target: text_label; color: Style.content_main}
        }
    ]
}
