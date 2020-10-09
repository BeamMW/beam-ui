import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    leftPadding:  20
    rightPadding: 20
    spacing:      15

    property var onClicked: null
    property var icons:     ['asset-0.svg', 'asset-1.svg', 'asset-2.svg']

    background: PanelGradient {
         leftColor:         Style.currencyPaneConnect
         rightColor:        Style.currencyPaneConnect
         leftBorderColor:   Style.currencyPaneBorder
         rightBorderColor:  Style.currencyPaneBorder
         borderWidth:       1
    }

    contentItem: RowLayout {
        spacing: control.spacing

        Item {
            Layout.preferredWidth:  15 * 3
            Layout.preferredHeight: 22
            Repeater {
                model: control.icons
                SvgImage {
                    source: ['qrc:/assets/', model.modelData].join('')
                    width:  22
                    height: 22
                    x: index * 15
                }
            }
        }

        SFText {
            Layout.fillWidth: true
            font.pixelSize:   14
            color:            Style.active
            //% "Connect other assets"
            text: qsTrId("assets-connect-other")
        }
    }

    MouseArea {
        z: 100
        anchors.fill: control
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            if (control.onClicked) {
                control.onClicked()
            }
        }
    }
}
