import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.1
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    property int inTxCnt
    property int outTxCnt
    property alias amount:    amountCtrl.amount
    property alias symbol:    amountCtrl.currencySymbol
    property alias symbol2:   amountCtrl.secondCurrencyLabel
    property alias rate:      amountCtrl.secondCurrencyRate
    property alias icon:      amountCtrl.iconSource
    property bool  selected:  false
    property var   onTip:     null
    property var   onClicked: null

    padding: 0
    leftPadding:  20
    rightPadding: 20

    background: PanelGradient {
        leftColor:        Style.currencyPaneLeftBEAM
        rightColor:       Style.currencyPaneRight
        leftBorderColor:  Qt.rgba( 0 / 255, 242 / 255, 209 / 255, 0.99)
        rightBorderColor: Qt.rgba( 0 / 255, 89 / 134, 209 / 255, 0.4)
        borderWidth:      control.selected ? 1 : 0

        implicitWidth:    control.width
        implicitHeight:   control.height
    }

    function formTipText () {
        //% "%1 incoming transactions"
        var inf = qsTrId("asset-incoming-tip")

        //% "%1 outgoing transactions"
        var outf = qsTrId("asset-outgoing-tip")

        //% "%1 active transactions\n(%2 incoming, %3 outgoing)"
        var inoutf = qsTrId("asset-inout-tip")

        if (control.inTxCnt && control.outTxCnt) {
            return inoutf.arg(control.inTxCnt + control.outTxCnt).arg(control.inTxCnt).arg(control.outTxCnt)
        }

        if (control.inTxCnt) {
            return inf.arg(control.inTxCnt)
        }

        return outf.arg(control.outTxCnt)
    }

    contentItem: RowLayout {
        spacing: 15

        BeamAmount {
            id:                amountCtrl
            Layout.fillWidth:  true
            spacing:           15
            lightFont:         false
            fontSize:          16
            iconSize:          Qt.size(22, 22)
            copyMenuEnabled:   true
        }

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
                              txIcon.parent.y + txIcon.y + txIcon.height,
                        )
                    }
                }
            }
        }
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
