import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control

    property alias  amount:          amountCtrl.amount
    property alias  maturingTotal:   amountCtrl.lockedAmount
    property alias  maturingRegular: maturingCtrl.amount
    property alias  change:          changeCtrl.amount
    property alias  maturingMP:      maxPrivacyCtrl.amount
    property alias  unitName:        amountCtrl.unitName
    property alias  rateUnit:        amountCtrl.rateUnit
    property alias  rate:            amountCtrl.rate
    property alias  icon:            amountCtrl.iconSource
    property alias  color:           back.leftColor
    property alias  borderColor:     back.leftBorderColor
    property bool   selected:        false
    property var    onClicked:       null
    property int    assetId:         0
    property bool   isAsset:         false

    property string assetName
    property string smallestUnitName
    property string shortDesc
    property string longDesc

    readonly property bool hasAmountTip: amountCtrl.hasTip || control.maturingTotal != "0" || control.change != "0"

    padding: 0
    leftPadding: 20
    rightPadding: 20

    background: PanelGradient {
        id: back
        leftColor:        Style.currencyPaneLeftBEAM
        rightColor:       Style.currencyPaneRight
        rightBorderColor: Style.currencyPaneRight
        leftBorderColor:  Style.currencyPaneLeftBEAM
        borderWidth:      control.selected ? 1 : 0

        implicitWidth:    control.width
        implicitHeight:   control.height
    }

    AlphaTipPopup {
        id:           assetTip
        defBkColor:   Qt.rgba(55 / 255, 93  / 255, 123 / 255, 0.75)
        defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
        parent:       Overlay.overlay
        visible:      false
        modal:        false
        dim:          true
        x:            amountCtrl.tipX
        y:            amountCtrl.tipY
        width:        (state == "ainfo" ? ainfoData.preferredWidth : amountData.preferredWidth ) + leftPadding + rightPadding

        Overlay.modeless: MouseArea {
            anchors.fill: parent
            onWheel: assetTip.close()
            acceptedButtons:  Qt.NoButton
            propagateComposedEvents: true
        }

        contentItem: Item { ColumnLayout {
            spacing: 0
            id: stateLayout

            state: control.hasAmountTip ? "amount" : "ainfo"
            states: [
                State {
                    name: "amount"
                    PropertyChanges { target: amountTab; state: "active" }
                },
                State {
                    name: "ainfo"
                    PropertyChanges { target: infoTab; state: "active" }
                }
            ]

            Row {
                spacing: 0
                Layout.alignment: Qt.AlignHCenter
                TxFilter {
                    id: amountTab
                    //% "Amount"
                    label:  qsTrId("general-amount")
                    onClicked: stateLayout.state = "amount"
                    visible: control.hasAmountTip
                    inactiveColor: Style.content_secondary
                }
                TxFilter {
                    id: infoTab
                    //% "Asset Info"
                    label:  qsTrId("general-asset-info")
                    onClicked: stateLayout.state = "ainfo"
                    visible: control.isAsset
                    inactiveColor: Style.content_secondary
                }
            }

            Item {
                height: 15
                Layout.fillWidth: true
            }

            GridLayout {
                id:            amountData
                columns:       2
                columnSpacing: 24
                rowSpacing:    14
                visible:       stateLayout.state == "amount"

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Amount"
                    text:  qsTrId("general-amount")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                BeamAmount {
                    amount:         control.amount
                    unitName:       control.unitName
                    rateUnit:       control.rateUnit
                    rate:           control.rate
                    color:          assetTip.defTextColor

                    font.styleName:  "DemiBold"
                    font.weight:     Font.DemiBold
                    font.pixelSize:  13
                    maxPaintedWidth: false
                }

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Maturing"
                    text:    qsTrId("general-maturing")
                    color:   assetTip.defTextColor
                    visible: maturingCtrl.visible

                    font {
                       styleName:  "Light"
                       weight:     Font.Light
                       pixelSize:  13
                    }
                }

                BeamAmount {
                    id:           maturingCtrl
                    unitName:     control.unitName
                    rateUnit:     control.rateUnit
                    rate:         control.rate
                    color:        assetTip.defTextColor
                    visible:      amount != "0"

                    font.styleName:  "DemiBold"
                    font.weight:     Font.DemiBold
                    font.pixelSize:  13
                    maxPaintedWidth: false
                }

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Change"
                    text:    qsTrId("general-change")
                    color:   assetTip.defTextColor
                    visible: changeCtrl.visible

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                BeamAmount {
                    id:           changeCtrl
                    unitName:     control.unitName
                    rateUnit:     control.rateUnit
                    rate:         control.rate
                    color:        assetTip.defTextColor
                    visible:      amount != "0"

                    font.styleName:  "DemiBold"
                    font.weight:     Font.DemiBold
                    font.pixelSize:  13
                    maxPaintedWidth: false
                }

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Max privacy"
                    text:    qsTrId("general-max-privacy")
                    color:   assetTip.defTextColor
                    visible: maxPrivacyCtrl.visible

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                Column {
                    spacing: 4

                    BeamAmount {
                        id:           maxPrivacyCtrl
                        unitName:     control.unitName
                        rateUnit:     control.rateUnit
                        rate:         control.rate
                        color:        assetTip.defTextColor
                        visible:      amount != "0"

                        font.styleName:  "DemiBold"
                        font.weight:     Font.DemiBold
                        font.pixelSize:  13
                        maxPaintedWidth: false
                    }

                    LinkButton {
                        visible:  maxPrivacyCtrl.visible
                        fontSize: 13
                        //% "More details"
                        text: qsTrId("more-details")

                        onClicked: {
                            main.openMaxPrivacyCoins(control.assetId, control.unitName, control.maturingMP)
                        }
                    }
                }
            }

            GridLayout {
                id:            ainfoData
                columns:       2
                columnSpacing: 24
                rowSpacing:    14
                visible:       stateLayout.state == "ainfo"

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Asset ID"
                    text:  qsTrId("info-asset-id")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text:  control.assetId.toString()
                    color: Style.content_main

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    Layout.alignment: Qt.AlignTop
                    visible: nameCtrl.visible

                    //% "Asset name"
                    text:  qsTrId("info-asset-name")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    id: nameCtrl
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text:  control.assetName
                    color: Style.content_main
                    visible: !!this.text

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Unit Name"
                    text:  qsTrId("info-asset-unit")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text: control.unitName
                    color: Style.content_main
                    visible: !!this.text

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    visible: smallestCtrl.visible

                    //% "Smallest unit"
                    text:  qsTrId("info-asset-smallest")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    id: smallestCtrl
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text:  "Ca"
                    color: Style.content_main
                    visible: !!this.text

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    visible: shortDescCtrl.visible

                    //% "Short description"
                    text:  control.shortDesc.length ? qsTrId("info-asset-short") : ""
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    id: shortDescCtrl
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text: control.shortDesc
                    color: Style.content_main
                    visible: !!this.text

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    visible: longDescCtrl.visible

                    //% "Long description"
                    text:  qsTrId("info-asset-long")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                SFText {
                    id: longDescCtrl
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap
                    visible: !!this.text

                    text: control.longDesc
                    color: Style.content_main

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }
            }
        }}
    }

    contentItem: ColumnLayout {
        BeamAmount {
            id:                amountCtrl
            Layout.fillWidth:  true
            spacing:           12
            iconSize:          Qt.size(24, 24)
            copyMenuEnabled:   true
            showDrop:          control.hasAmountTip || control.isAsset
            dropSize:          Qt.size(8, 4.8)
            tipCtrl:           assetTip
            font.styleName:    "Normal"
            font.weight:       Font.Normal
            font.pixelSize:    16
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: function (mouse) {
            var icon = amountCtrl.dropIcon
            var mousePoint = control.mapToItem(icon, Qt.point(mouse.x, mouse.y))
            var iconArea = Qt.rect(-8, -8, icon.width + 16, icon.height + 16)

            if (mousePoint.x >= iconArea.x && mousePoint.y > iconArea.y &&
                mousePoint.x <= iconArea.x + iconArea.width &&
                mousePoint.y <= iconArea.y + iconArea.height)
            {
                assetTip.open()
                mouse.accepted =  true
                return
            }

            if (control.onClicked) {
                mouse.accepted =  true
                control.onClicked()
                return
            }
        }
    }
}
