import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "."

Control {
    id: control
    property string text
    Layout.fillWidth: true

    contentItem: SFText {
        horizontalAlignment: Text.AlignLeft
        verticalAlignment:   Text.AlignVCenter
        text:  control.text
        color: Style.content_main

        font {
            styleName:      "Bold"
            weight:         Font.Bold
            pixelSize:      14
            letterSpacing:  3.11
            capitalization: Font.AllUppercase
        }
    }

    leftPadding:   0
    rightPadding:  20
    topPadding:    0
    bottomPadding: 10
}
