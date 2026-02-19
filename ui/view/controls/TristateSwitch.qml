import QtQuick
import QtQuick.Layouts
import "."

Item {
    id: control
    state: control.choices[0]

    property var backgroundColor: Qt.rgba(0, 246/255, 210/255, 0.1)
    property var borderColor: Style.active
    property var choices: ["0", "1", "2"]
    property var labels: ["0", "1", "2"]

    property var activeColor:   Style.active
    property var inactiveColor: Qt.rgba(0,0,0,0)
    property var activeText:    Style.content_opposite
    property var inactiveText:  Style.content_disabled

    states: [
        State {
            name: control.choices[0]
        },
        State {
            name: control.choices[1]
        },
        State {
            name: control.choices[2]
        }
    ]

    Rectangle {
        anchors.fill: parent
        color: control.backgroundColor
        radius: 10
        border.width: 1
        border.color: control.borderColor
    }

    RowLayout {
        Layout.preferredWidth: control.width
        Layout.preferredHeight: control.height
        spacing: 1

        Repeater {
            model: control.choices
            Rectangle {
                Layout.topMargin: 2
                Layout.leftMargin: index == 0 ? 2 : 0
                Layout.rightMargin: index == 3 ? 2 : 0
                width: control.width / 3 - 2
                height: control.height - 4
                radius: 10
                color: control.state == control.choices[index] ? control.activeColor : control.inactiveColor

                MouseArea {
                    anchors.fill: parent
                    onClicked: function() {
                        control.state = control.choices[index]
                    }
                }

                SFText {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: control.state == control.choices[index] ? control.activeText : control.inactiveText
                    font.pixelSize: 12
                    text: control.labels[index]
                }
            }
        }
    }
}
