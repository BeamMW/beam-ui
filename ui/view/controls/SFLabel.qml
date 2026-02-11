import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "."

Label {
    id: control
    property bool copyMenuEnabled: false
    property bool enableBackgroundRect: true
    property color selectionColor: control.palette.highlight
    property color selectedTextColor: control.palette.highlightedText

    signal copyText()

    font { 
        family:     "Proxima Nova"
        styleName:  "Regular"
        weight:     Font.Normal
        pixelSize:  14
    }

    background: Rectangle {
        id: backgroundRect
        visible: false// enableBackgroundRect & contextMenu.opened
        anchors.left: control.left
        anchors.top: control.top
        height: control.height
        width: control.width
        color: selectionColor
    }

    MouseArea {
        enabled: control.copyMenuEnabled
        anchors.left: control.left
        anchors.top: control.top
        height: control.height
        width: control.width
        acceptedButtons: Qt.RightButton

        onClicked: function(mouse) {
            var contextMenu = contextMenuComponent.createObject(control)
            contextMenu.x = mouse.x
            contextMenu.y = mouse.y
            contextMenu.open()

        }
    }

    Component {
        id: contextMenuComponent
        ContextMenu {
            modal: true
            dim: false
            enabled: parent.copyMenuEnabled
            property color prevColor
            Action {
                //% "Copy"
                text: qsTrId("general-copy")
                icon.source: "qrc:/assets/icon-copy.svg"
                enabled: parent.enabled && parent.copyMenuEnabled
                onTriggered: parent.copyText()
            }
            onOpened: {
                if (parent.enableBackgroundRect)
                    parent.background.visible = true;
                
                prevColor = parent.color
                parent.color = parent.selectedTextColor;
            }
            onClosed: {
                parent.color = prevColor;
                parent.background.visible = false;
            }
        }
    }
}
