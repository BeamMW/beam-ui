import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

AlphaTip {
    id: control

    property var totals
    property var progress

    property string  title
    property string  displayProp
    property string  title2
    property string  displayProp2

    contentItem:  GridLayout {
        columnSpacing: 15
        rowSpacing:    10
        columns:       2
        rows:          2

        SFText {
            font.pixelSize:       12
            font.styleName:       "Light"
            font.weight:          Font.Light
            color:                lockedTip.defTextColor
            Layout.alignment:     Qt.AlignTop
            text:                 control.title2
            visible:              control.title2.length
            Layout.bottomMargin:  5
        }

        BeamAmount {
            amount:               control.displayProp2.length ? control.totals[control.displayProp2] : ""
            unitName:             control.totals.unitName
            rateUnit:             control.totals.rateUnit
            rate:                 control.totals.rate
            spacing:              15
            lightFont:            false
            fontSize:             12
            vSpacing:             1
            visible:              control.title2.length
            Layout.bottomMargin:  5
        }

        SFText {
            font.pixelSize:   12
            font.styleName:   "Light"
            font.weight:      Font.Light
            color:            lockedTip.defTextColor
            Layout.alignment: Qt.AlignTop
            text:             control.title
            visible:          control.title.length
        }

        BeamAmount {
            amount:    control.totals[control.displayProp]
            unitName:  control.totals.unitName
            rateUnit:  control.totals.rateUnit
            rate:      control.totals.rate
            spacing:   15
            lightFont: false
            fontSize:  12
            vSpacing:  1
            visible:   control.title.length
        }

        Repeater {
            model: control.progress.length
            SvgImage {
                visible:           control.progress.length > 1 && parseFloat(control.progress[index][control.displayProp])
                Layout.column:     0
                Layout.row:        index + (control.title2.length ? 2 : 1)
                Layout.alignment:  Qt.AlignHCenter | Qt.AlignVCenter
                source:            control.progress[index].icon
                sourceSize:        Qt.size(20, 20)
            }
        }

        Repeater {
           model: control.progress.length
           BeamAmount {
               Layout.column:     1
               Layout.row:        index + (control.title2.length ? 2 : 1)
               Layout.alignment:  Qt.AlignLeft | Qt.AlignVCenter
               visible:           control.progress.length > 1 && parseFloat(control.progress[index][control.displayProp])
               amount:            control.progress[index][control.displayProp]
               unitName:          control.progress[index].unitName
               rateUnit:          control.progress[index].rateUnit
               rate:              control.progress[index].rate
               spacing:           15
               lightFont:         false
               fontSize:          12
               vSpacing:          1
           }
        }
    }
}

