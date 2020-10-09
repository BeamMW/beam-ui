import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    property alias available:              avctrl.available
    property alias secondCurrencyUnitName: avctrl.secondCurrencyUnitName
    property alias secondCurrencyRate:     avctrl.secondCurrencyRate
    property alias icon:                   avctrl.icon
    property alias unitName:               avctrl.unitName
    property alias assetName:              avctrl.assetName
    property bool  showDetails:            true

    spacing: 10
    property real itemWidth: {
        return control.showDetails ? (control.availableWidth - control.spacing) / 2 : control.availableWidth
    }

    height: showDetails ? 200 : 130

    contentItem: Row {
        spacing: 10

        AvailablePanelNew {
            id:      avctrl
            width:   control.itemWidth
            height:  control.availableHeight
            compact: control.showDetails
        }

        InProgressPanelNew {
            id: ipctrl
            width:   control.itemWidth
            height:  control.availableHeight
            visible: showDetails
        }
    }

    MouseArea {
        anchors.fill: control
        onClicked: {
            control.showDetails = !control.showDetails
        }
    }
}
