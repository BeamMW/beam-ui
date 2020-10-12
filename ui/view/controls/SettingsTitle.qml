import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
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

    leftPadding:   20
    rightPadding:  20
    topPadding:    25
    bottomPadding: 10
}
