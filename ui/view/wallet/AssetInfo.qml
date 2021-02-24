import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control

    property int inTxCnt
    property int outTxCnt

    property alias  amount:      amountCtrl.amount
    property alias  maturing:    maturingCtrl.amount
    property alias  change:      changeCtrl.amount
    property alias  maxPrivacy:  maxPrivacyCtrl.amount
    property alias  unitName:    amountCtrl.unitName
    property alias  rateUnit:    amountCtrl.rateUnit
    property alias  rate:        amountCtrl.rate
    property alias  icon:        amountCtrl.iconSource
    property alias  color:       back.leftColor
    property alias  borderColor: back.leftBorderColor
    property bool   selected:    false
    property var    onClicked:   null
    property int    assetId:     0
    property bool   isAsset:     false

    property string assetName
    property string smallestUnitName
    property string shortDesc
    property string longDesc

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

    AlphaTip {
        id:           assetTip
        defBkColor:   Qt.rgba(55 / 255, 93  / 255, 123 / 255, 0.75)
        defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
        parent:       Overlay.overlay
        visible:      (amountCtrl.hasTip || control.isAsset) && (amountCtrl.inTipArea || tipArea.containsMouse)
        vGap:         5
        x:            amountCtrl.tipX
        y:            amountCtrl.tipY - vGap
        z:            100

        state: amountCtrl.hasTip ? "amount" : "ainfo"

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

        MouseArea {
            anchors.fill:     parent
            hoverEnabled:     true
            acceptedButtons:  Qt.LeftButton
            z: -1
        }

        MouseArea {
            id:               tipArea
            anchors.fill:     parent
            hoverEnabled:     true
            acceptedButtons:  Qt.NoButton
            z: 1

            onWheel: function(data) {
                data.accepted = true
            }
        }

        contentItem: ColumnLayout {
            spacing: 0
            z: 0

            Item {
                height: assetTip.vGap
                Layout.fillWidth: true
            }

            Row {
                spacing: 0
                Layout.alignment: Qt.AlignHCenter
                TxFilter {
                    id: amountTab
                    //% "Amount"
                    label:  qsTrId("general-amount")
                    onClicked: assetTip.state = "amount"
                    visible: amountCtrl.hasTip
                }
                TxFilter {
                    id: infoTab
                    //% "Asset Info"
                    label:  qsTrId("general-asset-info")
                    onClicked: assetTip.state = "ainfo"
                    visible: control.isAsset
                }
            }

            Item {
                height: 15
                Layout.fillWidth: true
            }

            GridLayout {
                columns:       2
                columnSpacing: 24
                rowSpacing:    14
                visible:       assetTip.state == "amount"

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
                    maxUnitChars:   6
                    color:          assetTip.defTextColor

                    font.styleName: "DemiBold"
                    font.weight:    Font.DemiBold
                    font.pixelSize: 13
                }

                SFText {
                    Layout.alignment: Qt.AlignTop

                    //% "Maturing"
                    text:    qsTrId("general-maturing")
                    color:   assetTip.defTextColor
                    visible: maturingCtrl.visible

                    font {
                       styleName:  "DemiBold"
                       weight:     Font.DemiBold
                       pixelSize:  13
                    }
                }

                BeamAmount {
                    id:           maturingCtrl
                    unitName:     control.unitName
                    rateUnit:     control.rateUnit
                    rate:         control.rate
                    maxUnitChars: 6
                    color:        assetTip.defTextColor
                    visible:      amount != "0"

                    font.styleName: "DemiBold"
                    font.weight:    Font.DemiBold
                    font.pixelSize: 13
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
                    maxUnitChars: 6
                    color:        assetTip.defTextColor
                    visible:      amount != "0"

                    font.styleName: "DemiBold"
                    font.weight:    Font.DemiBold
                    font.pixelSize: 13
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

                BeamAmount {
                    id:           maxPrivacyCtrl
                    unitName:     control.unitName
                    rateUnit:     control.rateUnit
                    rate:         control.rate
                    maxUnitChars: 6
                    color:        assetTip.defTextColor
                    visible:      amount != "0"

                    font.styleName: "DemiBold"
                    font.weight:    Font.DemiBold
                    font.pixelSize: 13
                }
            }

            GridLayout {
                columns:       2
                columnSpacing: 24
                rowSpacing:    14
                visible:       assetTip.state == "ainfo"

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

                    text: control.shortDesc
                    color: Style.content_main

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }
            }
        }
    }

    contentItem: ColumnLayout {
        BeamAmount {
            id:                amountCtrl
            Layout.fillWidth:  true
            spacing:           12
            iconSize:          Qt.size(24, 24)
            copyMenuEnabled:   true
            maxPaintedWidth:   control.availableWidth
            maxUnitChars:      6
            showDrop:          amountCtrl.hasTip
            showDropCircular:  true
            dropSize:          Qt.size(8, 4.8)
            tipCtrl:           assetTip
            font.styleName:    "Normal"
            font.weight:       Font.Normal
            font.pixelSize:    16
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (control.onClicked) {
                control.onClicked()
                return
            }
        }
    }
}
