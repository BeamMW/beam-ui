import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

AlphaTipPopup {
    id:           assetTip
    defBkColor:   Qt.rgba(55 / 255, 93  / 255, 123 / 255, 0.75)
    defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
    parent:       Overlay.overlay
    visible:      false
    modal:        false
    dim:          true
    width:        (state == "ainfo" ? ainfoData.preferredWidth : amountData.preferredWidth ) + leftPadding + rightPadding

    property var  assetInfo
    property bool hasAmountTip: false

    Overlay.modeless: MouseArea {
        anchors.fill: parent
        onWheel: assetTip.close()
        acceptedButtons:  Qt.NoButton
        propagateComposedEvents: true
    }

    Component {
        id: longTipText

        ScrollView {
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

            RowLayout {
                SFText {
                    id: textCrtl
                    Layout.maximumWidth: 240
                    wrapMode: Text.Wrap

                    text: longText
                    color: Style.content_main
                    visible: !!this.text
                    linkEnabled: true
                    textFormat: Text.RichText

                    onLinkActivated: {
                        Utils.openExternalWithConfirmation(link)
                    }

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }
            }
        }
    }

    contentItem: Item { ColumnLayout {
        spacing: 0
        id: stateLayout

        state: assetTip.hasAmountTip ? "amount" : "ainfo"
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
                visible: assetTip.hasAmountTip
                inactiveColor: Style.content_secondary
            }
            TxFilter {
                id: infoTab
                //% "Asset Info"
                label:  qsTrId("general-asset-info")
                onClicked: stateLayout.state = "ainfo"
                visible: assetInfo.id != 0
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
                amount:         assetInfo.amount
                unitName:       assetInfo.unitName
                rateUnit:       assetInfo.rateUnit
                rate:           assetInfo.rate
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
                unitName:     assetInfo.unitName
                rateUnit:     assetInfo.rateUnit
                rate:         assetInfo.rate
                color:        assetTip.defTextColor
                amount:       assetInfo.maturingTotal
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
                unitName:     assetInfo.unitName
                rateUnit:     assetInfo.rateUnit
                rate:         assetInfo.rate
                color:        assetTip.defTextColor
                amount:       assetInfo.change
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
                    unitName:     assetInfo.unitName
                    rateUnit:     assetInfo.rateUnit
                    rate:         assetInfo.rate
                    color:        assetTip.defTextColor
                    amount:       assetInfo.maturingMP
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
                        main.openMaxPrivacyCoins(assetInfo.id, assetInfo.unitName, assetInfo.maturingMP)
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

                text:  assetInfo.id.toString()
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

                text:  assetInfo.assetName
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

                //% "Unit name"
                text:  qsTrId("info-asset-unit")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.unitName
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      35
            }

            SFText {
                visible: !!assetInfo.smallestUnitName

                //% "Smallest unit name"
                text:  qsTrId("info-asset-smallest")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.smallestUnitName
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      35
            }

            SFText {
                visible: !!assetInfo.shortDesc

                //% "Short description"
                text:  qsTrId("info-asset-short")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.shortDesc
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      70
            }

            SFText {
                visible: !!assetInfo.longDesc

                //% "Long description"
                text:  qsTrId("info-asset-long")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.longDesc
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      70
            }

            SFText {
                visible: !!assetInfo.siteUrl

                //% "Website"
                text:  qsTrId("info-asset-site")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.siteUrl ? [Style.linkStyle, "<a href='", assetInfo.siteUrl, "'>", assetInfo.siteUrl, "</a>"].join("") : ""
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      20
            }

            SFText {
                visible: !!assetInfo.siteUrl

                //% "Whitepaper"
                text:  qsTrId("info-asset-paper")
                color: assetTip.defTextColor

                font {
                    pixelSize: 13
                    styleName: "Light"
                    weight:    Font.Light
                }
            }

            Loader {
                property string longText:  assetInfo.whitePaper ? [Style.linkStyle, "<a href='", assetInfo.whitePaper, "'>", assetInfo.whitePaper, "</a>"].join("") : ""
                sourceComponent:           longTipText
                Layout.maximumWidth:       240
                Layout.maximumHeight:      20
            }
        }
    }}
}