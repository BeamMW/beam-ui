import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import "."

T.TextArea {
    id: control

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            placeholderText ? placeholder.implicitWidth + leftPadding + rightPadding : 0)
                            || contentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding,
                             background ? background.implicitHeight : 0,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    font { 
        family: "Proxima Nova"
        styleName: "Regular"
    }

    padding: 6
    leftPadding: 8

    color: control.palette.text
    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    property alias backgroundColor : backgroundRect.color
    backgroundColor: Style.content_main

    PlaceholderText {
        id: placeholder
        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: control.font
        opacity: 0.5
        color: control.color
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText && (!control.activeFocus || control.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: backgroundRect
        anchors.fill: parent
        radius: 10
        opacity: (control.activeFocus || control.hovered)? 0.1 : 0.05
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        hoverEnabled: true

        onClicked: function(mouse) {
            var selectStart = control.selectionStart
            var selectEnd = control.selectionEnd
            var curPos = control.cursorPosition
            contextMenu.x = mouse.x
            contextMenu.y = mouse.y
            contextMenu.open()
            control.cursorPosition = curPos
            control.select(selectStart, selectEnd)
        }
    }

    ContextMenu {
        id: contextMenu
        modal: true
        dim: false
        Action {
            //% "Copy"
            text: qsTrId("general-copy")
            icon.source: "qrc:/assets/icon-copy.svg"
            enabled: control.enabled
            onTriggered: {
                if (control.selectedText.length > 0) {
                    control.copy();
                }
                else {
                    control.selectAll();
                    control.copy();
                    control.deselect();
                }
            }
        }
        Action {
            //% "Paste"
            text: qsTrId("general-paste")
            icon.source: "qrc:/assets/icon-edit.svg"
            enabled: control.canPaste
            onTriggered: {
                control.paste()
            }
        }
    }
}