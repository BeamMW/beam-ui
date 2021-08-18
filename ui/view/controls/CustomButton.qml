import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

Button {
    id: control
    
    palette.button:      checkable ? (checked ? Style.active : "transparent") : Style.background_button
    palette.buttonText:  checkable ? (checked ? Style.content_opposite : Style.content_secondary) : Style.content_main
    opacity:             enabled   ? 1.0 : 0.45

    property int   radius:         checkable ? 10 : 50
    property bool  allLowercase:   !text.startsWith("I ")
    property bool  showHandCursor: false
    property bool  hasShadow:      true
    property alias border:         rect.border


    font { 
        family: "SF Pro Display"
        pixelSize: 14
        styleName: control.checkable ? "Regular" : "Bold"
        weight: control.checkable ? Font.Normal : Font.Bold
        capitalization: allLowercase && !control.checkable ? Font.AllLowercase : Font.MixedCase
    }

    height: 38
    Layout.preferredHeight: 38
    leftPadding: 30
    rightPadding: 30
    activeFocusOnTab: true

    spacing:     15
    icon.width:  16
    icon.height: 16

    contentItem: IconLabel {
        spacing:  control.spacing
        mirrored: control.mirrored
        display:  control.display

        icon:  control.icon
        text:  control.text
        font:  control.font
        color: control.palette.buttonText

        MouseArea {
            anchors.fill:  parent
            hoverEnabled:  true
            enabled:       false
            cursorShape:   control.showHandCursor ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }
    
    Keys.onPressed: {
        if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) control.clicked();
    }

    background: Rectangle {
        id:      rect
        radius:  control.radius
        color:   control.palette.button
    }

    DropShadow {
        id: drop_shadow
        anchors.fill: rect
        radius:  7
        samples: 9
        color:   Style.content_main
        source:  rect
        visible: control.hasShadow && (control.visualFocus || control.hovered || control.checked)
    }
}
