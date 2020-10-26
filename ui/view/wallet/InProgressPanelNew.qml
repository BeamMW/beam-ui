import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"
import "."

Control {
    id: control
    padding: 20

    property var  progress
    property var  totals
    property real separatorWidth: 1
    property real fillerWidth: 20

    function xUp(ctrl) {
        var x = 0
        do
        {
            x += ctrl.x
            ctrl = ctrl.parent
        }
        while (ctrl != null)
        return x
    }

    function yUp(ctrl) {
        var y = 0
        do
        {
            y += ctrl.y
            ctrl = ctrl.parent
        }
        while (ctrl != null)
        return y
    }

    AmountTip {
        id:           lockedTip
        visible:      lockedArea.containsMouse && lockedCtrl.showDrop
        x:            xUp(lockedCtrl) - control.padding / 2
        y:            yUp(lockedCtrl) + lockedCtrl.height + 5
        parent:       Overlay.overlay
        totals:       control.totals
        progress:     control.progress
        displayProp:  control.totals.lockedMaturingMP == "0" ? "lockedMaturing" : "lockedMaturingMP"
        displayProp2: control.totals.lockedMaturingMP == "0" ? "lockedMaturingMP" : "lockedMaturing"

        title: control.totals.lockedMaturingMP == "0" ?
            //% "Maturing"
            qsTrId("available-panel-maturing") :
            //% "Max Privacy"
            qsTrId("available-panel-maturing-mp")

        title2: control.totals.lockedMaturingMP == "0" ?
            qsTrId("available-panel-maturing-mp") :
            qsTrId("available-panel-maturing")
    }

    AmountTip {
        id:           receivingTip
        visible:      receivingArea.containsMouse && receivingCtrl.showDrop
        x:            xUp(receivingCtrl) - control.padding / 2
        y:            yUp(receivingCtrl) + receivingCtrl.height + 5
        parent:       Overlay.overlay
        totals:       control.totals
        progress:     control.progress
        displayProp:  control.totals.receivingIncoming == "0" ? "receivingChange" : "receivingIncoming"
        displayProp2: control.totals.receivingIncoming == "0" ? "receivingIncoming" : "receivingChange"

        title:  control.totals.receivingIncoming == "0" ?
                  //% "Change"
                  qsTrId("available-panel-change") :
                  //% "Incoming"
                  qsTrId("available-panel-incoming")

        title2: control.totals.receivingIncoming == "0" ?
            qsTrId("available-panel-incoming") :
            qsTrId("available-panel-change")
    }

    AmountTip {
        id:           sendingTip
        visible:      sendingArea.containsMouse && sendingCtrl.showDrop
        x:            xUp(sendingCtrl) - control.padding / 2
        y:            yUp(sendingCtrl) + sendingCtrl.height + 5
        parent:       Overlay.overlay
        totals:       control.totals
        progress:     control.progress
        displayProp:  "sending"
    }

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

        RowLayout {
            spacing: 0
            Layout.fillHeight: true
            Layout.fillWidth:  true

            ColumnLayout {
                spacing: 20
                Layout.preferredWidth: control.availableWidth / 2 - fillerWidth
                Layout.alignment: Qt.AlignTop
                Layout.topMargin: 15

                Column {
                    id: iconsCtrl
                    spacing: 6

                    SFLabel {
                        visible:        text.length > 0
                        font.pixelSize: 12
                        font.styleName: "Light"
                        font.weight:    Font.Light
                        color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.7)
                        //% "Assets"
                        text:qsTrId("inprogress-assets")
                    }

                    Row {
                        spacing: 10
                        Repeater {
                            model: Math.min(4, control.progress.length)
                            SvgImage {
                                source: control.progress[index].icon
                            }
                        }

                        SFText {
                            visible: control.progress.length > 4
                            text: ["+", control.progress.length - 4].join("")
                            anchors.verticalCenter: parent.verticalCenter

                            font.pixelSize:   17
                            font.styleName:   "Regular"
                            font.weight:      Font.Normal
                            color:            Style.content_main
                        }
                    }
                }

                BeamAmount {
                    id:               lockedCtrl
                    amount:           control.totals.locked
                    unitName:         control.totals.unitName
                    rateUnit:         control.totals.rateUnit
                    rate:             control.totals.rate
                    lightFont:        false
                    fontSize:         16
                    copyMenuEnabled:  true

                    //% "Locked"
                    caption:     qsTrId("available-panel-locked")
                    visible:     parseFloat(control.totals.locked) > 0
                    showDrop:    true
                    showZero:    false

                    MouseArea {
                        id: lockedArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
            }

            Rectangle {
                id:     separator
                color:  Qt.rgba(Style.background_second.r, Style.background_second.g, Style.background_second.b, 0.1)
                Layout.preferredWidth:  1
                Layout.fillHeight:      true
            }

            Rectangle {
                id:     filler
                color:  "transparent"
                Layout.preferredWidth:  20
                height: parent.height
            }

            ColumnLayout {
                spacing: 20
                Layout.preferredWidth: control.availableWidth / 2 - fillerWidth - separatorWidth
                Layout.alignment: Qt.AlignTop
                Layout.topMargin: parseFloat(control.rate) ? 0 : 15

                BeamAmount {
                    id:               sendingCtrl
                    amount:           control.totals.sending
                    unitName:         control.totals.unitName
                    rateUnit:         control.totals.rateUnit
                    rate:             control.totals.rate
                    color:            Style.accent_outgoing
                    lightFont:        false
                    fontSize:         16
                    copyMenuEnabled:  true
                    maxPaintedWidth:  control.availableWidth / 2 - fillerWidth - separatorWidth
                    showDrop:         parseFloat(control.totals.sending) > 0 && control.progress.length > 1
                    width:            150

                    //% "Sending"
                    caption:    qsTrId("available-panel-sending")
                    showZero:   false
                    prefix:     "-"

                    MouseArea {
                        id: sendingArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                BeamAmount {
                    id:               receivingCtrl
                    amount:           control.totals.receiving
                    unitName:         control.totals.unitName
                    rateUnit:         control.totals.rateUnit
                    rate:             control.totals.rate
                    color:            Style.accent_incoming
                    lightFont:        false
                    fontSize:         16
                    copyMenuEnabled:  true
                    showDrop:         parseFloat(control.totals.receiving) > 0
                    maxPaintedWidth:  control.availableWidth / 2 - separator.width - fillerWidth - separatorWidth

                    //% "Receiving"
                    caption:    qsTrId("available-panel-receiving")
                    showZero:   false
                    prefix:     "+"

                    MouseArea {
                        id: receivingArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
            }
        }
    }

    background: Rectangle {
        radius: 10
        color: Style.background_second
    }
}
