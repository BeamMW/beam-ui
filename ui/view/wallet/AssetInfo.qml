import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control

    property var    assetInfo
    property var    onClicked
    property alias  color:         back.leftColor
    property alias  borderColor:   back.leftBorderColor
    property bool   selected:      false

    readonly property bool hasBalanceTip: amountCtrl.hasTip || assetInfo.locked != "0" || assetInfo.amountShielded != "0"

    padding: 0
    leftPadding: 20
    rightPadding: 20

    Text {
        id: fakeTip
    }

    background: PanelGradient {
        id: back
        leftColor:        assetInfo.color
        rightColor:       Style.coinPaneRight
        rightBorderColor: Style.coinPaneRight
        leftBorderColor:  assetInfo.selectionColor
        borderWidth:      control.selected ? 1 : 0

        implicitWidth:    control.width
        implicitHeight:   control.height
    }

    contentItem: ColumnLayout {
        BeamAmount {
            id:                amountCtrl
            amount:            assetInfo.amount
            lockedAmount:      assetInfo.locked
            unitName:          assetInfo.unitName
            rateUnit:          assetInfo.rateUnit
            rate:              assetInfo.rate
            iconSource:        assetInfo.icon
            Layout.fillWidth:  true
            spacing:           12
            iconSize:          Qt.size(26, 26)
            copyMenuEnabled:   true
            showDrop:          control.hasBalanceTip || assetInfo.id != 0
            dropSize:          Qt.size(8, 4.8)
            tipCtrl:           fakeTip
            font.styleName:    "Normal"
            font.weight:       Font.Normal
            font.pixelSize:    16
            showTip:           true
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
                mousePoint.y <= iconArea.y + iconArea.height &&
                amountCtrl.showDrop)
            {
                var assetTip = Qt.createComponent("AssetTip.qml").createObject(Overlay.overlay, {
                    hasBalanceTip: control.hasBalanceTip,
                    assetId: control.assetInfo.id,
                    assetInfo: control.assetInfo
                });

                amountCtrl.tipCtrl = assetTip
                assetTip.x = amountCtrl.tipX
                assetTip.y = amountCtrl.tipY

                assetTip.onVisibleChanged.connect(function () {
                    if (!assetTip.visible) {
                        assetTip.destroy()
                        if (amountCtrl) {
                            amountCtrl.tipCtrl = fakeTip
                        }
                    }
                })

                control.Component.onDestruction.connect(function() {
                    assetTip.visible = false
                })

                appWindow.onActiveChanged.connect(function (active) {
                    if (!active) {
                        assetTip.visible = false
                    }
                })

                assetTip.onWidthChanged.connect(function () {
                    assetTip.x = amountCtrl.tipX
                    assetTip.y = amountCtrl.tipY
                })

                assetTip.onLink = function (link) {
                    assetTip.visible = false
                    Utils.openExternalWithConfirmation(link)
                }

                assetTip.onMPDetails = function () {
                    assetTip.visible = false
                    main.openMaxPrivacyCoins(assetInfo.id, assetInfo.unitName, assetInfo.maturingMP)
                }

                assetTip.open()
                mouse.accepted = true
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
