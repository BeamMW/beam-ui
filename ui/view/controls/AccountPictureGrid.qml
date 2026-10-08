import QtQuick
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "."

Grid {
    id: control

    property int  currentIndex: 0
    property real itemSize:     100

    columnSpacing: 20
    columns:       3
    rowSpacing:    4

    Repeater {
        model: 9
        delegate: Item {
            width:  control.itemSize
            height: control.itemSize

            Item {
                anchors.fill: parent
                visible: control.currentIndex == index
                Rectangle {
                    id:             itemBorder
                    anchors.fill:   parent
                    border.color:   Style.active
                    border.width:   3
                    color:          "transparent"
                    radius:         width / 2
                }
                DropShadow {
                    anchors.fill: parent
                    radius: 5
                    samples: 9
                    color: Style.active
                    source: itemBorder
                }
            }

            SvgImage {
                anchors.fill: parent
                source: BeamGlobals.getAccountPictureByIndex(index)
            }
            MouseArea {
                anchors.fill: parent
                cursorShape:  Qt.PointingHandCursor
                onClicked:    control.currentIndex = index
            }
        }
    }
}
