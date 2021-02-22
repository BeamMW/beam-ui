import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    property int inTxCnt
    property int outTxCnt
    property alias amount:      amountCtrl.amount
    property alias unitName:    amountCtrl.unitName
    property alias rateUnit:    amountCtrl.rateUnit
    property alias rate:        amountCtrl.rate
    property alias icon:        amountCtrl.iconSource
    property alias color:       back.leftColor
    property alias borderColor: back.leftBorderColor
    property bool  selected:    false
    property var   onTip:       null
    property var   onClicked:   null

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

    function calcMaxWidth () {
        return control.availableWidth // - (txIcon.visible ? txIcon.width + contentRow.spacing : 0)
    }

    contentItem: ColumnLayout {
        id: contentRow
        spacing: 15

        BeamAmount {
            id:                amountCtrl
            Layout.fillWidth:  true
            spacing:           12
            lightFont:         false
            fontSize:          16
            iconSize:          Qt.size(24, 24)
            copyMenuEnabled:   true
            maxPaintedWidth:   calcMaxWidth()
            maxUnitChars:      6
            showDrop:          true
            dropSize:          Qt.size(8, 4.8)
        }

        /*
        SvgImage
        {
            id:      txIcon
            source:  control.outTxCnt && control.inTxCnt ? "qrc:/assets/icon-inout-transactions.svg" : control.outTxCnt ? "qrc:/assets/icon-outgoing-transaction.svg" :  "qrc:/assets/icon-incoming-transaction.svg"
            visible: control.outTxCnt || control.inTxCnt

            Layout.preferredWidth: 17
            Layout.preferredHeight: 17

            MouseArea {
                id: txIconArea
                anchors.fill: parent
                hoverEnabled: true

                onContainsMouseChanged: {
                    if (onTip) {
                        onTip(containsMouse, formTipText(),
                              txIcon.parent.x + txIcon.x + txIcon.width,
                              txIcon.parent.y + txIcon.y + txIcon.height
                        )
                    }
                }
            }
        }*/
    }

    MouseArea {
        anchors.fill: control
        onClicked: {
            if (control.onClicked) {
                control.onClicked()
            }
        }
    }
}
