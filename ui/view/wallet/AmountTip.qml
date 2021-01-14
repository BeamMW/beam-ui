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

    Component {
        id: assetsList

        GridLayout {
            columnSpacing: 15
            rowSpacing:    10
            columns:       2

            SFText {
                font.pixelSize:       12
                font.styleName:       "Light"
                font.weight:          Font.Light
                Layout.alignment:     Qt.AlignTop
                color:                Style.content_main
                text:                 title
                Layout.bottomMargin:  5
                visible:              title.length > 0
            }

            function amountInfo () {
                var totalCnt = 0
                var unit = ""

                for (var index = 0; index < control.progress.length; ++index) {
                    var info = control.progress[index]
                    if (parseFloat(info[displayProp])) {
                        totalCnt++
                        unit = info.unitName
                    }
                }

                return {
                    unitName: totalCnt == 1 ? unit : totals.unitName,
                    cnt: totalCnt
                }
            }

            BeamAmount {
                amount:               totals[displayProp] || ""
                unitName:             amountInfo().unitName
                rateUnit:             totals.rateUnit
                rate:                 totals.rate
                spacing:              15
                lightFont:            false
                fontSize:             12
                vSpacing:             1
                Layout.bottomMargin:  5
                visible:              title.length > 0
            }

            Repeater {
                model: control.progress.length
                SvgImage {
                    visible:           amountInfo().cnt > 1 && parseFloat(control.progress[index][displayProp])
                    Layout.column:     0
                    Layout.row:        index + (title.length > 0 ? 1 : 0)
                    Layout.alignment:  Qt.AlignHCenter | Qt.AlignVCenter
                    source:            control.progress[index].icon
                    sourceSize:        Qt.size(20, 20)
                }
            }

            Repeater {
               model: control.progress.length
               BeamAmount {
                   Layout.column:     1
                   Layout.row:        index + (title.length > 0 ? 1 : 0)
                   Layout.alignment:  Qt.AlignLeft | Qt.AlignVCenter
                   visible:           amountInfo().cnt > 1 && parseFloat(control.progress[index][displayProp])
                   amount:            control.progress[index][displayProp] || ""
                   unitName:          control.progress[index].unitName
                   rateUnit:          control.progress[index].rateUnit
                   rate:              control.progress[index].rate
                   spacing:           15
                   lightFont:         false
                   fontSize:          12
                   vSpacing:          1
                   maxUnitChars:      6
               }
            }
        }
    }

    contentItem: Column {
        spacing: 5

        Loader {
            sourceComponent: assetsList
            property string  title: control.title
            property string  displayProp: control.displayProp
        }

        Loader {
            sourceComponent: assetsList
            property string  title: control.title2
            property string  displayProp: control.displayProp2
        }
    }
}
