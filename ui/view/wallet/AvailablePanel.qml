import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control

    property string available
    property string locked
    property string lockedMaturing
    property string sending
    property string receiving
    property string receivingChange
    property string receivingIncoming
    property string secondCurrencyUnitName
    property string secondCurrencyRate

    property var onOpenExternal: null
    signal copyValueText()

    background: PanelGradient {
        leftColor:  Style.currencyPaneLeftBEAM
        rightColor: Style.currencyPaneRight
    }

    leftPadding:   20
    rightPadding:  20
    topPadding:    8
    bottomPadding: 12

    AlphaTip {
        id:            lockedTip
        visible:       lockedArea.containsMouse && parseFloat(locked) > 0
        x:             lockedAmount.x + lockedAmount.parent.x + lockedAmount.parent.parent.x + lockedAmount.width / 2 - lockedTip.width / 2
        y:             lockedAmount.y + lockedAmount.parent.y + lockedAmount.height + 15

        contentItem:  GridLayout {
            columnSpacing: 15
            rowSpacing:    10
            columns:       2
            rows:          2

            SFText {
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          lockedTip.defTextColor
                //% "Maturing"
                text:           qsTrId("available-panel-maturing")
            }

            BeamAmount {
                amount:            lockedMaturing
                unitName:          BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                spacing:           15
                lightFont:         false
                fontSize:          12
            }
        }
    }

    AlphaTip {
        id:            receivingTip
        visible:       receivingArea.containsMouse && parseFloat(receiving) > 0
        x:             receivingAmount.x + receivingAmount.parent.x + receivingAmount.parent.parent.x + receivingAmount.width / 2 - receivingTip.width / 2
        y:             receivingAmount.y + receivingAmount.parent.y + receivingAmount.height + 15

        contentItem:  GridLayout {
            columnSpacing: 15
            rowSpacing:    10
            columns:       2
            rows:          2

            SFText {
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          lockedTip.defTextColor
                //% "Change"
                text:           qsTrId("available-panel-change")
            }

            BeamAmount {
                amount:            receivingChange
                unitName:          BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                spacing:           15
                lightFont:         false
                fontSize:          12
                color:             Style.accent_incoming
                prefix:            "+"
            }

            SFText {
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Incoming"
                text:           qsTrId("available-panel-incoming")
            }

            BeamAmount {
                amount:            receivingIncoming
                unitName:          BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                spacing:           15
                lightFont:         false
                fontSize:          12
                color:             Style.accent_incoming
                prefix:            "+"
            }
        }
    }

    contentItem: RowLayout {
        spacing: 0
        RowLayout {
            Layout.preferredWidth: parseFloat(receiving) > 0 || parseFloat(sending) > 0 ? parent.width / 2 : parent.width
            BeamAmount {
                amount:            available
                unitName:          BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                secondCurrencyUnitName:   control.secondCurrencyUnitName
                secondCurrencyRate:    control.secondCurrencyRate
                spacing:           15
                lightFont:         false
                fontSize:          16
                iconSource:        "qrc:/assets/icon-beam.svg"
                iconSize:          Qt.size(22, 22)
                copyMenuEnabled:   true
                //% "Available"
                caption:           qsTrId("available-panel-available")
            }

            Item {
                Layout.fillWidth: true
            }

            BeamAmount {
                id:                lockedAmount
                amount:            locked
                unitName:          BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                secondCurrencyUnitName: control.secondCurrencyUnitName
                secondCurrencyRate:  control.secondCurrencyRate
                lightFont:         false
                fontSize:          16
                copyMenuEnabled:   true
                //% "Locked"
                caption:           qsTrId("available-panel-locked")
                visible:           parseFloat(locked) > 0
                showDrop:          true

                 MouseArea {
                    id: lockedArea
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        RowLayout{
            Layout.preferredWidth: parent.width / 2
            visible: parseFloat(receiving) > 0 || parseFloat(sending) > 0

            Rectangle {
                color:   Qt.rgba(255, 255, 255, 0.1)
                width:   1
                height:  45
            }

            BeamAmount {
                Layout.leftMargin:     20
                amount:                sending
                unitName:              BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                secondCurrencyUnitName:   control.secondCurrencyUnitName
                secondCurrencyRate:    control.secondCurrencyRate
                color:                 Style.accent_outgoing
                lightFont:             false
                fontSize:              16
                copyMenuEnabled:       true

                //% "Sending"
                caption:    qsTrId("available-panel-sending")
                showZero:   false
                prefix:     "-"
            }

            Item {
                Layout.fillWidth: true
            }

            BeamAmount {
                id:                    receivingAmount
                amount:                receiving
                unitName:              BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
                secondCurrencyUnitName:   control.secondCurrencyUnitName
                secondCurrencyRate:    control.secondCurrencyRate
                color:                 Style.accent_incoming
                lightFont:             false
                fontSize:              16
                copyMenuEnabled:       true

                //% "Receiving"
                caption:           qsTrId("available-panel-receiving")
                showZero:          false
                prefix:            "+"
                showDrop:          parseFloat(receiving) > 0

                MouseArea {
                    id: receivingArea
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}
