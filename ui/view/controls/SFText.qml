import QtQuick
import QtQuick.Window

Text {
    font { 
        family:     "Proxima Nova"
        styleName:  "Regular"
        weight:     Font.Normal
        pixelSize:  14
    }
    property alias linkEnabled: linkMouseArea.enabled
    MouseArea {
        id: linkMouseArea
        enabled: false
        anchors.fill: parent
        acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
