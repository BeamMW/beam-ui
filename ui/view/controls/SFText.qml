import QtQuick 2.15
import QtQuick.Window 2.2

Text {
	font { 
		family:    "ProximaNova"
		styleName: "Regular"
		weight:    Font.Normal
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
