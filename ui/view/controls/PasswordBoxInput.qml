import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.impl 2.4
import QtQuick.Templates 2.4 as T
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
        family: "Proxima Nova"
        styleName: "Regular"
        letterSpacing : 4
    }

    padding: 10
    leftPadding: 16
    rightPadding: showEye ? 50 : 16

    selectionColor: control.palette.highlight
    selectedTextColor: control.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    property bool  focusablePlaceholder: false
    property bool  hasError: false
    property color inputColor: Style.content_main
    color: hasError ? Style.validator_error : inputColor
    property bool showEye: false
    echoMode: TextInput.Password
    passwordCharacter: "•"

    selectByMouse: true

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
        color:               control.hasError ? Style.validator_error : control.color
        verticalAlignment:   control.verticalAlignment
        horizontalAlignment: control.horizontalAlignment
        visible:             (focusablePlaceholder || !control.activeFocus) && !control.length && !control.preeditText && text
        elide:               Text.ElideRight
        wrapMode:            control.wrapMode
    }

    background: Rectangle {
        id: backgroundRect
        width: control.width
        height: control.height
        opacity: control.hasError ? 0.15 : ((control.activeFocus || control.hovered)? 0.1 : 0.05)
        radius: 10
        color: control.hasError ? Style.validator_error : Style.content_main
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

    SvgImage {
        source: control.echoMode == TextInput.Password ? "qrc:/assets/icon-eye.svg" : "qrc:/assets/icon-eye-crossed.svg"
        x: control.width - 40
        y: 9
        visible: showEye && control.activeFocus

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.PointingHandCursor
            onClicked: function () {
                if (control.echoMode == TextInput.Password) {
                    control.echoMode = TextInput.Normal;
                } else if (control.echoMode == TextInput.Normal) {
                    control.echoMode = TextInput.Password;
                }
            }
        }
    }

    Keys.onShortcutOverride: event.accepted = event.matches(StandardKey.Paste)
    Keys.onPressed: {
        if (event.matches(StandardKey.Paste)) {
            event.accepted = true
            control.paste()
            control.textPasted()
        }
    }
}
