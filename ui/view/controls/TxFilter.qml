import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import "."

Item  {
    id: root

    property alias  label:          text_label.text
    property var    capitalization: Font.AllUppercase
    property alias  font :          text_label.font
    property bool   showLed:        true

    implicitWidth:  showLed ? led.width : text_label.implicitWidth
    implicitHeight: showLed ? 20 : text_label.implicitHeight
    state: "normal"
    signal clicked()

    property var activeColor:   Style.content_main
    property var inactiveColor: Style.content_secondary

    SFText {
        id: text_label

        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
        font.styleName: "DemiBold"; font.weight: Font.DemiBold
        font.capitalization: capitalization

        MouseArea {
            anchors.fill:   parent
            cursorShape:    Qt.PointingHandCursor
            onClicked:      root.clicked()
        }
    }

    Rectangle {
        id: led

        anchors.bottom: parent.bottom
        width: text_label.implicitWidth + 12 * 2
        height: 2
        color: Style.active
        visible: false
    }

    DropShadow {
        anchors.fill:   led
        radius:         5
        samples:        9
        color:          Style.active
        source:         led

        visible:        led.visible
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {target: text_label; color: root.inactiveColor}
        },
        State {
            name: "active"
            PropertyChanges {target: led; visible: showLed}
            PropertyChanges {target: text_label; opacity: 1.0}
            PropertyChanges {target: text_label; color: root.activeColor}
        }
    ]
}
