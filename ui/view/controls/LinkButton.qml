import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Control {
    id: control

    property string linkColor: control.enabled ? Style.active : Qt.rgba(Style.active.r, Style.active.g, Style.active.b, 0.3)
    property string linkStyle: ["<style>a:link {color: '", linkColor, "'; text-decoration: none;}</style>"].join("")
    property string text
    property bool   bold: false
    signal   clicked

    contentItem: SFText {
        text:           [linkStyle, "<a href='#'>", control.text, "</a>"].join("")
        textFormat:     Text.RichText
        font.pixelSize: 14
        font.styleName: bold ? "Bold" : "Regular"
        font.weight:    bold ? Font.Bold : Font.Normal

        MouseArea {
            id:                area
            anchors.fill:      parent
            acceptedButtons:   Qt.LeftButton
            onClicked:         control.clicked()
            hoverEnabled:      true
            onPositionChanged: area.cursorShape = Qt.PointingHandCursor;
        }
    }
}