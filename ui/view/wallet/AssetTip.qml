import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

AlphaTipPopup {
    id:           assetTip
    defBkColor:   Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.2)
    defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
    parent:       Overlay.overlay
    visible:      false
    modal:        false
    dim:          true
    width:        (state == "ainfo" ? ainfoData.preferredWidth : balanceData.preferredWidth ) + leftPadding + rightPadding
    rightPadding: 0

    property int  assetId: -1
    property var  assetInfo
    property var  onLink
    property var  onMPDetails
    property bool hasBalanceTip: false

    Overlay.modeless: MouseArea {
        anchors.fill: parent
        onWheel: function () {
            assetTip.visible = false
        }
        acceptedButtons:  Qt.NoButton
        propagateComposedEvents: true
    }

    Component {
        id: longTipText

        SFText {
            TextMetrics {
                id: textMetrics
                text: longText

                font {
                    family:        "ProximaNova"
                    styleName:     "Regular"
                    weight:        Font.Normal
                    pixelSize:     13
                }
            }

            id: textCrtl
            wrapMode: textMetrics.width > 240 ? Text.Wrap : Text.NoWrap

            text:        longText
            color:       Style.content_main
            linkEnabled: true
            textFormat:  Text.RichText
            width:       textMetrics.width > 240 ? 240 : textMetrics.width

            onLinkActivated: {
                assetTip.onLink(link)
            }

            font {
                pixelSize: 13
                styleName: "Normal"
                weight:    Font.Normal
            }
        }
    }

    property var maxScrollHeight: main.height - assetTip.y - tabsRow.height - stateLayout.spacing - assetTip.topPadding - assetTip.bottomPadding

    contentItem: Item { ColumnLayout {
        id: stateLayout
        spacing: 15

        state: assetTip.hasBalanceTip ? "balance" : "ainfo"
        states: [
            State {
                name: "balance"
                PropertyChanges { target: balanceTab; state: "active" }
            },
            State {
                name: "ainfo"
                PropertyChanges { target: infoTab; state: "active" }
            }
        ]

        Row {
            spacing: 0
            id: tabsRow
            Layout.alignment: Qt.AlignHCenter
            Layout.rightMargin: 14
            Layout.leftMargin: 14

            TxFilter {
                id: balanceTab
                //% "Balance"
                label:  qsTrId("general-balance")
                onClicked: stateLayout.state = "balance"
                visible: assetTip.hasBalanceTip
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

        ScrollView {
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: assetTip.visible && contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            visible: stateLayout.state == "balance"

            property var minWidth: tabsRow.width + tabsRow.Layout.leftMargin + tabsRow.Layout.rightMargin
            Layout.maximumHeight: maxScrollHeight
            Layout.minimumWidth: minWidth
            leftPadding: balanceData.width < minWidth ? (minWidth - balanceData.width) / 2 : 0

            GridLayout {
                id:                  balanceData
                columns:             2
                columnSpacing:       24
                rowSpacing:          14

                SFText {
                    //% "Available"
                    text:  qsTrId("balance-available")
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

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                SFText {
                    visible: amountRegularCtrl.visible

                    //% "Regular"
                    text:  qsTrId("balance-regular")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                BeamAmount {
                    id:             amountRegularCtrl
                    amount:         assetInfo.amountRegular
                    unitName:       assetInfo.unitName
                    rateUnit:       assetInfo.rateUnit
                    rate:           assetInfo.rate
                    color:          assetTip.defTextColor
                    visible:        amount != "0"

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                SFText {
                    visible: amountShieldedCtrl.visible

                    //% "Shielded"
                    text:  qsTrId("balance-shielded")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                BeamAmount {
                    id:             amountShieldedCtrl
                    amount:         assetInfo.amountShielded
                    unitName:       assetInfo.unitName
                    rateUnit:       assetInfo.rateUnit
                    rate:           assetInfo.rate
                    color:          assetTip.defTextColor
                    visible:        amount != "0"

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                Rectangle {
                    Layout.fillWidth:  true
                    Layout.columnSpan: 2

                    height:   1
                    color:    "white"
                    opacity:  0.1
                    visible:  lockedCtrl.visible
                    Layout.rightMargin: 14
                }

                SFText {
                    //% "Locked"
                    text:    qsTrId("balance-locked")
                    color:   assetTip.defTextColor
                    visible: lockedCtrl.visible

                    font {
                       styleName:  "Light"
                       weight:     Font.Light
                       pixelSize:  13
                    }
                }

                BeamAmount {
                    id:           lockedCtrl
                    unitName:     assetInfo.unitName
                    rateUnit:     assetInfo.rateUnit
                    rate:         assetInfo.rate
                    color:        assetTip.defTextColor
                    amount:       assetInfo.locked
                    visible:      amount != "0"

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                SFText {
                    //% "Maturing"
                    text:    qsTrId("balance-maturing")
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
                    amount:       assetInfo.maturingRegular
                    visible:      amount != "0"

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                SFText {
                    //% "Change"
                    text:    qsTrId("balance-change")
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

                    font.styleName:  "Normal"
                    font.weight:     Font.Normal
                    font.pixelSize:  13
                    maxPaintedWidth: false
                    maxUnitChars:    20
                    Layout.rightMargin: 14
                }

                SFText {
                    //% "Max privacy"
                    text:    qsTrId("balance-mp")
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
                    Layout.rightMargin: 14
                    id: maxPrivacyCtrl
                    visible: assetInfo.maturingMP != "0"

                    BeamAmount {

                        unitName:     assetInfo.unitName
                        rateUnit:     assetInfo.rateUnit
                        rate:         assetInfo.rate
                        color:        assetTip.defTextColor
                        amount:       assetInfo.maturingMP

                        font.styleName:  "Normal"
                        font.weight:     Font.Normal
                        font.pixelSize:  13
                        maxPaintedWidth: false
                        maxUnitChars:    20
                    }

                    LinkButton {
                        visible:  maxPrivacyCtrl.visible
                        fontSize: 13
                        //% "More details"
                        text: qsTrId("more-details")

                        onClicked: {
                            assetTip.onMPDetails()
                        }
                    }
                }
            }
        }

        ScrollView {
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: assetTip.visible && contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            visible: stateLayout.state == "ainfo"

            property var minWidth: tabsRow.width + tabsRow.Layout.leftMargin + tabsRow.Layout.rightMargin
            Layout.maximumHeight:  maxScrollHeight
            Layout.minimumWidth:   minWidth
            leftPadding:           ainfoData.width < minWidth ? (minWidth - ainfoData.width) / 2 : 0

            GridLayout {
                id:                  ainfoData
                columns:             2
                columnSpacing:       24
                rowSpacing:          14

                SFText {
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
                    Layout.rightMargin: 14
                    text:  assetInfo.id.toString()
                    color: Style.content_main

                    font {
                        pixelSize: 13
                        styleName: "Normal"
                        weight:    Font.Normal
                    }
                }

                SFText {
                    visible: !!assetInfo.assetName

                    //% "Asset name"
                    text:  qsTrId("info-asset-name")
                    color: assetTip.defTextColor

                    font {
                        pixelSize: 13
                        styleName: "Light"
                        weight:    Font.Light
                    }
                }

                Loader {
                    property string longText:  assetInfo.assetName
                    sourceComponent:           longTipText
                    Layout.rightMargin:        14
                    visible:                   !!assetInfo.assetName
                }

                SFText {
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
                    Layout.rightMargin:        14
                    visible:                   !!assetInfo.unitName
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
                    Layout.rightMargin:        14
                    visible:                   !!assetInfo.smallestUnitName
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
                    Layout.rightMargin:        14
                    visible:                   !!assetInfo.shortDesc
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
                    Layout.rightMargin:        14
                    visible:                   !!assetInfo.longDesc
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
                    Layout.rightMargin:        14
                    visible:                   !!longText
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

                    Layout.rightMargin:        14
                    visible:                   !!longText
                }

                Item {
                    height: 16
                }
                OpenInBlockchainExplorer {
                    Layout.rightMargin: 14
                    onTriggered: function() {
                        var url = BeamGlobals.getExplorerUrl() + "assets/details/" + assetInfo.id;
                        Utils.openExternalWithConfirmation(url);
                    }
                }
            }
        }
    }}
}