import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    padding: 20

    contentItem: ColumnLayout {
        spacing: 0
        SFText {
            text: "In Progress"
            color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
            font {
                styleName:      "Bold"
                weight:         Font.Bold
                pixelSize:      14
                letterSpacing:  3.11
                capitalization: Font.AllUppercase
            }
        }

        Row {
            spacing: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            /*Rectangle {
                width: parent.width / 2
                height: parent.height
            }

            Rectangle {
                width: parent.width / 2
                height: parent.height
            }*/
        }
    }

    background: Rectangle {
        radius: 10
        color: Style.background_second
    }
}
