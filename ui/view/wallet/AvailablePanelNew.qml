import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control
    padding: 20

    property alias   icon:      avctrl.iconSource
    property alias   unitName:  avctrl.unitName
    property alias   available: avctrl.amount
    property string  rateUnit:  ""
    property string  rate:      "0"
    property string  assetName: ""
    property bool    compact:   true

    contentItem: ColumnLayout {
        spacing: 0

        Row {
            Layout.fillWidth: true
            spacing: compact ? 0 : bigSeparator.x - title.width

            SFText {
                id: title
                //% "Available"
                text: qsTrId("available-panel-available")
                color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                font {
                    styleName:      "Bold"
                    weight:         Font.Bold
                    pixelSize:      14
                    letterSpacing:  3.11
                    capitalization: Font.AllUppercase
                }
            }

            Rectangle {
                id:      smallSeparator
                width:   1
                color:   Qt.rgba(Style.background_second.r, Style.background_second.g, Style.background_second.b, 0.1)
                height:  parent.height
                visible: !control.compact
            }
        }

        RowLayout {
            spacing: 0
            Layout.fillHeight: true
            Layout.fillWidth:  true

            RowLayout {
                id:                 fullLayout
                visible:            !control.compact
                Layout.fillHeight:  true
                Layout.alignment:   Qt.AlignTop
                spacing:            75
            }

            ColumnLayout {
                id: compactLayout
                visible:                control.compact
                spacing:                25
                Layout.preferredWidth:  control.availableWidth / 2
                Layout.alignment:       Qt.AlignTop

                BeamAmount {
                    visible: true
                    id: avctrl
                    Layout.topMargin:  20
                    Layout.alignment:  Qt.AlignTop
                    spacing:           15
                    lightFont:         false
                    boldFont:          true
                    fontSize:          16
                    iconSize:          Qt.size(22, 22)
                    copyMenuEnabled:   true
                    //% "%1 Available"
                    caption:           qsTrId("wallet-beam-available").arg(Utils.limitText(control.assetName, 15))
                    maxPaintedWidth:   control.compact ? control.availableWidth / 2 : 0
                    maxUnitChars:      7
                }

                BeamAmount {
                    id: totalctrl
                    Layout.topMargin:   control.compact ? 0 : 20
                    Layout.rightMargin: control.compact ? 0 : 55
                    Layout.alignment:   Qt.AlignTop
                    spacing:            15
                    lightFont:          false
                    fontSize:           16
                    iconSize:           Qt.size(22, 22)
                    iconSource:         "qrc:/assets/icon-usd.svg"
                    copyMenuEnabled:    true
                    showZero:           false
                    //% "Total Available"
                    caption:           qsTrId("wallet-total-available")
                    amount:            BeamGlobals.calcAmountInSecondCurrency(control.available, control.rate, control.rateUnit)
                    unitName:          control.rateUnit
                    maxPaintedWidth:   control.compact ? control.availableWidth / 2 : 0
                }
            }

            Rectangle {
                id:     bigSeparator
                color:  Qt.rgba(Style.background_second.r, Style.background_second.g, Style.background_second.b, 0.1)
                width:  1
                Layout.fillHeight: true
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth:  true

                SvgImage {
                    id:      tealSmall
                    y:       parent.height / 2 - height / 2 - 10
                    width:   Math.min(parent.width - 10, control.compact ? 185 * 1.5 : 420 * 1.25)
                    x:       parent.width / 2 - this.width / 2 + 10
                    source:  control.compact ? "qrc:/assets/graph-teal-small.svg" : "qrc:/assets/graph-teal-big.svg"
                }

                SvgImage {
                    x:      tealSmall.x
                    y:      tealSmall.y
                    width:  tealSmall.width
                    source: control.compact ? "qrc:/assets/graph-white-small.svg" : "qrc:/assets/graph-white-big.svg"
                }
            }
        }
    }

    background: Rectangle {
        radius: 10
        color: Style.background_second
    }

    function changeLayout () {
        var  currParent = control.compact ? compactLayout : fullLayout
        avctrl.parent = currParent
        totalctrl.parent = currParent
    }

    Component.onCompleted: changeLayout()
    onCompactChanged: changeLayout()
}
