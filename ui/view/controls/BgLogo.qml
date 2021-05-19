import QtQuick 2.11
import Beam.Wallet 1.0

Image {
    fillMode: Image.PreserveAspectCrop
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottomMargin: 20
    source: Style.bottom_background
    visible: Style.bottom_background.length != 0
}