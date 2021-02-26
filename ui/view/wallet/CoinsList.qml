import QtQuick 2.15
import QtQuick.Layouts 1.3
import "../controls"

RowLayout {
    id: control

    property var icons
    property var names
    readonly property int count: icons ? icons.length : 0

    spacing: 0
    clip: true

    Repeater {
        model: Math.min(3, control.count)

        SvgImage {
            Layout.leftMargin: index == 0 ? 20 : -6
            source: icons[index]
            sourceSize: Qt.size(20, 20)
            z: -index
        }
    }

    SFText {
        id: extraCount
        Layout.fillWidth: true
        Layout.leftMargin: 7
        visible: count > 3

        text:            ["+", count - 3].join("")
        font.pixelSize:  12
        font.styleName:  "Regular"
        font.weight:     Font.Normal
        color:           Style.content_main
    }

    SFText {
        id: coinName
        Layout.fillWidth: true
        Layout.leftMargin: 10
        visible: count == 1

        text:            control.names ? control.names[0] : ""
        font.pixelSize:  14
        font.styleName:  "Regular"
        font.weight:     Font.Normal
        color:           Style.content_main
        elide:           Text.ElideRight
    }

    Item {
        Layout.fillWidth: true
        visible: !extraCount.visible && ! coinName.visible
    }
}
