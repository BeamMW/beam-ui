import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control
    padding: 20

    contentItem: RowLayout {
        spacing: 14

        SFText {
            text: "DEMOX"
            color: Style.content_main
            font {
                styleName:      "Regular"
                weight:         Font.Normal
                pixelSize:      20
                capitalization: Font.AllUppercase
            }
        }

        SvgImage {
            Layout.maximumHeight: 24
            Layout.maximumWidth:  24
            source: "qrc:/assets/icon-swap-currencies-raw.svg"
            MouseArea {
                anchors.fill:    parent
                acceptedButtons: Qt.LeftButton
                cursorShape:     Qt.PointingHandCursor
                onClicked: {
                }
            }
        }

        SFText {
            text: "BEAM"
            color: Style.content_main
            font {
                styleName:      "Regular"
                weight:         Font.Normal
                pixelSize:      20
                capitalization: Font.AllUppercase
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }

    background: Rectangle {
        radius: 10
        color: Style.background_second
    }
}
