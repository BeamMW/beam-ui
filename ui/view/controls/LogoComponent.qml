import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "."

ColumnLayout {
    spacing: 0
    Item {
        Layout.fillWidth:       true
        Layout.fillHeight:      true
        Layout.preferredHeight: 50
    }

    SvgImage {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 329
        Layout.preferredHeight: 329
        source: "qrc:/assets/start-logo.svg"
    }

    Item {
        Layout.fillWidth:       true
        Layout.fillHeight:      true
        Layout.preferredHeight: 20
    }

    SFText {
        Layout.alignment: Qt.AlignHCenter

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
}
