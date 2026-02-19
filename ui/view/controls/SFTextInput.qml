import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Beam.Wallet 1.0
import "."

T.TextField {
    id: control
    signal textPasted()

    function getMousePos() {
        return {x: mouseArea.mouseX, y: mouseArea.mouseY}
    }

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            placeholderText ? placeholder.implicitWidth + leftPadding + rightPadding : 0)
                            || contentWidth + leftPadding + rightPadding
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding,
                             background ? background.implicitHeight : 0,
                             placeholder.implicitHeight + topPadding + bottomPadding)

    font { 
        family:         "Proxima Nova"
        styleName:      "Regular"
        pixelSize:      14
    }

    padding: 6
    leftPadding: 8

    color: control.palette.text
    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    property bool  focusablePlaceholder: false
    property alias backgroundColor : backgroundRect.color
    property alias underlineVisible : backgroundRect.visible
    backgroundColor: Style.content_main
    property bool highlight: false

    PlaceholderText {
        id:                  placeholder
        x:                   control.leftPadding
        y:                   control.topPadding
        width:               control.width - (control.leftPadding + control.rightPadding)
        height:              control.height - (control.topPadding + control.bottomPadding)
                             
        text:                control.placeholderText
        font.family:         control.font.family
        font.styleName:      control.font.styleName
        font.pixelSize:      control.font.pixelSize
        font.italic:         true
        opacity:             0.2
        color:               control.color
        verticalAlignment:   control.verticalAlignment
        horizontalAlignment: control.horizontalAlignment
        visible:             (focusablePlaceholder || !control.activeFocus) && !control.length && !control.preeditText && text
        elide:               Text.ElideRight
        wrapMode:            control.wrapMode
    }

    background: Rectangle {
        id: backgroundRect
        anchors.fill: parent
        radius: 10
        opacity: (control.activeFocus || control.hovered || control.highlight) ? 0.1 : 0.05
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        hoverEnabled: true
        id: mouseArea

        onClicked: {
            var selectStart = control.selectionStart
            var selectEnd = control.selectionEnd
            contextMenu.x = mouseX
            contextMenu.y = mouseY
            contextMenu.open()
            if (cursorPosition == selectEnd) control.select(selectStart, selectEnd)
            else control.select(selectEnd, selectStart)
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
            enabled: control.enabled && (control.echoMode === TextInput.Normal) && control.text.length
            onTriggered: {
                if (control.selectedText.length > 0) {
                    control.copy();
                }
                else {
                    BeamGlobals.copyToClipboard(control.text)
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
                control.textPasted()
            }
        }

        property bool inputFocus: false

        onAboutToShow: {
            // save input state before menu
            inputFocus = control.focus
            // we always force focus on menu
            control.forceActiveFocus()
        }

        onClosed: {
            // restore input state after menu
            if (inputFocus) {
                var selectStart = control.selectionStart
                var selectEnd   = control.selectionEnd
                control.forceActiveFocus()
                if (cursorPosition == selectEnd) control.select(selectStart, selectEnd)
                else control.select(selectEnd, selectStart)
            } else {
                backgroundRect.forceActiveFocus()
            }
        }
    }

    Keys.onShortcutOverride: function(event) { event.accepted = event.matches(StandardKey.Paste) }
    Keys.onPressed: function(event) {
        if (event.matches(StandardKey.Paste)) {
            event.accepted = true
            control.paste()
            control.textPasted()
        }
    }
}
