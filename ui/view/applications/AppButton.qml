import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "../controls"

Button {
    id: control
    
    palette.button:      Qt.rgba(0, 246, 210, 0.1)
    palette.buttonText:  Style.active

    property int   radius:         19
    property bool showAdditional: true

    signal clicked1()
    signal clicked2()

    font { 
        family: "Proxima Nova"
        pixelSize: 14
        styleName: control.checkable ? "Regular" : "Bold"
        weight: control.checkable ? Font.Normal : Font.Bold
    }

    height: 38
    Layout.preferredHeight: 38
    leftPadding: 16
    rightPadding: 16
    activeFocusOnTab: true

    spacing:     8
    icon.width:  16
    icon.height: 16

    contentItem: RowLayout {
        id: rowLayout
        IconLabel {
            spacing:  control.spacing
            mirrored: control.mirrored
            display:  control.display
            rightPadding: 8

            icon:  control.icon
            text:  control.text
            font:  control.font
            color: control.palette.buttonText
        }

        Rectangle {
            id: separator
            height: 17
            width: 1
            border.width: 1
            visible: control.showAdditional
        }

        IconLabel {
            spacing:  control.spacing
            mirrored: control.mirrored
            display:  control.display
            leftPadding: 12
            visible: control.showAdditional

            icon.source: "qrc:/assets/icon-chevron-down.svg"
        }
    }

    MouseArea {
        anchors.fill:  parent
        onClicked: function (event) {
            console.log("button ", rowLayout.x, " ",  separator.x, " ", event.x);
            if (control.showAdditional) {
                if (event.x < rowLayout.x + separator.x) {
                    clicked1();
                } else {
                    clicked2();
                }
            } else {
                clicked1();
            }
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) control.clicked();
    }

    Rectangle {
        anchors.fill: parent
        radius:  control.radius
        color:   Style.background_main
        visible: !control.enabled
    }

    background: Rectangle {
        id:      rect
        radius:  control.radius
        color:   control.palette.button
        border.color: Style.active
    }
}
