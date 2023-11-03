import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

ColumnLayout
{
    property bool isSqueezedHeight: false

    spacing: 0

    Item {
        Layout.fillWidth:       true
        Layout.preferredHeight: 60
    }

    SvgImage
    {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 329
        Layout.preferredHeight: 329
        source: "qrc:/assets/start-logo.svg"
    }

    SFText
    {
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 13

        //% "Confidential DeFi Platform and Cryptocurrency"
        text:       qsTrId("logo-description")
        color:      Style.content_main
        opacity:    0.7

        font {
            styleName:  "Bold"
            weight:     Font.Bold
            pixelSize:  16
        }
    }

    Item {
        Layout.fillWidth:       true
        Layout.preferredHeight: isSqueezedHeight ? 10 : 40 
    }
}
