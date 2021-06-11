import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import Beam.Wallet 1.0
import "."

ColumnLayout
{
    property bool isSqueezedHeight: false

    function themeName() {
        return Theme.name();
    }

    function isMainNet() {
        return themeName() == "mainnet";
    }

    spacing: 0

    Item {
        Layout.preferredHeight: 60
        visible: isMainNet()
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
        Layout.topMargin: isSqueezedHeight ? 20 : 40

        //% "Confidential DeFi Platform and Cryptocurrency"
        text: qsTrId("logo-description")
        color: Style.active

        font {
            styleName:  "DemiBold"
            weight:     Font.DemiBold
            pixelSize:  18
        }
    }

    SFText
    {
        visible: !isMainNet()
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: 20
        Layout.topMargin: isSqueezedHeight ? 10 : 40
        color: Style.content_secondary
        text: themeName()

        font {
            styleName:      "DemiBold"
            weight:         Font.DemiBold
            pixelSize:      18
            capitalization: Font.AllUppercase
        }
    }

    Item {
        Layout.preferredHeight: isSqueezedHeight ? 10 : 30 
    }
}
