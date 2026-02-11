import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

CheckBox {
    id: control
    property color mainColor: control.checked ? Style.active : Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)

    indicator: Rectangle {
        implicitWidth:  15
        implicitHeight: 15
        x:              control.leftPadding
        y:              parent.height / 2 - height / 2
        border.color:   mainColor
        border.width:   1
        radius:         1
        color:          control.checked ? mainColor : "transparent"
        Image {
            visible: control.checked
            source:  "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/check.png"
            anchors.fill: parent
        }
    }
    contentItem: Text {
        text:  control.text
        color: mainColor
        font {
            family:     "Proxima Nova"
            styleName:  "Regular"
            weight:     Font.Normal
            pixelSize:  14
        }
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
