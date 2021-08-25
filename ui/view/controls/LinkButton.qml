import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

Control {
    id: control

    property color linkColor : Style.active
    readonly property string __linkColor: control.enabled ? linkColor : Qt.rgba(linkColor.r, linkColor.g, linkColor.b, 0.3)
    property string linkStyle: ["<style>a:link {color: '", __linkColor, "'; text-decoration: none;}</style>"].join("")
    property string text
    property bool   bold: false
    signal   clicked
    property alias fontSize: textCtrl.font.pixelSize

    contentItem: SFText {
        id:             textCtrl
        anchors.fill:   parent
        text:           [linkStyle, "<a href='#'>", control.text, "</a>"].join("")
        textFormat:     Text.RichText
        font.pixelSize: 14
        font.styleName: bold ? "Bold" : "Regular"
        font.weight:    bold ? Font.Bold : Font.Normal
        wrapMode:       Text.Wrap

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