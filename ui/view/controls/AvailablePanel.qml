import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

Control {
    id: control
    z:  10

    property string available
    property string locked
    property string lockedMaturing
    property string lockedMaturingMP
    property string sending
    property string receiving
    property string receivingChange
    property string receivingIncoming
    property string secondCurrencyLabel
    property string secondCurrencyRateValue

    property var onOpenExternal: null
    signal copyValueText()

    background: Rectangle {
        id:       panel
        color:    "transparent"

        Rectangle {
            width:  parent.height
            height: parent.width
            anchors.centerIn: parent
            anchors.alignWhenCentered: false
            rotation: 90
            radius:   10
            opacity:  0.3
            gradient: Gradient {
                GradientStop { position: 0.0; color: Style.swapCurrencyPaneGrRight }
                GradientStop { position: 1.0; color: Style.swapCurrencyPaneGrLeftBEAM }
            }
        }
    }

    leftPadding:   20
    rightPadding:  20
    topPadding:    8
    bottomPadding: 12

    Control {
        id:            lockedTip
        visible:       (lockedArea.containsMouse || lockedTipArea.containsMouse || maturingMoreDetailsArea.containsMouse) && parseFloat(locked) > 0
        x:             lockedAmount.x + lockedAmount.parent.x + lockedAmount.parent.parent.x + lockedAmount.width / 2 - lockedTip.width / 2
        y:             lockedAmount.y + lockedAmount.parent.y + lockedAmount.height + 7

        leftPadding:   14
        rightPadding:  14
        topPadding:    13
        bottomPadding: 13

        background: Rectangle {
            anchors.fill: parent
            color:  Qt.rgba(255, 255, 255, 0.15)
            radius: 10
            MouseArea {
                id: lockedTipArea
                anchors.fill: parent
                hoverEnabled: true
            }
        }

        contentItem:  GridLayout {
            columnSpacing: 15
            rowSpacing:    10
            columns:       2
            rows:          2

            SFText {
                id:             maturingLabel
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Maturing"
                text:           qsTrId("available-panel-maturing")
                Layout.alignment: Qt.AlignTop
                visible:           parseFloat(lockedMaturing) > 0
            }

            BeamAmount {
                amount:            lockedMaturing
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
                spacing:           15
                lightFont:         false
                fontSize:          12
                visible:           maturingLabel.visible
            }

            SFText {
                id:             maxPrivacyLabel
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Max privacy"
                text:           qsTrId("available-panel-maturing-mp")
                Layout.alignment: Qt.AlignTop
                visible:           parseFloat(lockedMaturingMP) > 0
            }

            BeamAmount {
                amount:            lockedMaturingMP
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
                spacing:           15
                lightFont:         false
                fontSize:          12
                visible:           maxPrivacyLabel.visible
            }

            SFText {
                id:             maturingMoreDetailsLabel
                font.pixelSize: 14
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Style.active
                //% "More details"
                text:             qsTrId("available-panel-maturing-details")
                Layout.alignment: Qt.AlignTop
                visible:           maxPrivacyLabel.visible
                MouseArea {
                    id: maturingMoreDetailsArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        mpDialog.open()
                    }
                }
            }
        }
    }

    Control {
        id:            receivingTip
        visible:       receivingArea.containsMouse && parseFloat(receiving) > 0
        x:             receivingAmount.x + receivingAmount.parent.x + receivingAmount.parent.parent.x + receivingAmount.width / 2 - receivingTip.width / 2
        y:             receivingAmount.y + receivingAmount.parent.y + receivingAmount.height + 15

        leftPadding:   14
        rightPadding:  14
        topPadding:    13
        bottomPadding: 13

        background: Rectangle {
            anchors.fill: parent
            color:  Qt.rgba(255, 255, 255, 0.15)
            radius: 10
        }

        contentItem:  GridLayout {
            columnSpacing: 15
            rowSpacing:    10
            columns:       2
            rows:          2

            SFText {
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Change"
                text:           qsTrId("available-panel-change")
            }

            BeamAmount {
                amount:            receivingChange
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
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
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
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
        RowLayout{
            Layout.preferredWidth: parseFloat(receiving) > 0 || parseFloat(sending) > 0 ? parent.width / 2 : parent.width
            BeamAmount {
                amount:            available
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
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
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
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
                Layout.leftMargin: 20
                amount:            sending
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
                color:             Style.accent_outgoing
                lightFont:         false
                fontSize:          16
                copyMenuEnabled:   true
                //% "Sending"
                caption:           qsTrId("available-panel-sending")
                showZero:          false
                prefix:            "-"
            }

            Item {
                Layout.fillWidth: true
            }

            BeamAmount {
                id:                receivingAmount
                amount:            receiving
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        control.secondCurrencyLabel
                secondCurrencyRateValue:    control.secondCurrencyRateValue
                color:             Style.accent_incoming
                lightFont:         false
                fontSize:          16
                copyMenuEnabled:   true
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