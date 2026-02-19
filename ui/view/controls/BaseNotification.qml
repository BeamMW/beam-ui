import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "."

Popup {
    id: control

    property var verticalOffset: 0
    property var nextVerticalOffset: 0
    property alias lifetime: closeTimer.interval

    modal:        false
    parent:       Overlay.overlay
    x:            parent.width - width - 20
    y:            parent.height - height - 20 - verticalOffset
    z:            100
    padding:      20
    closePolicy:  Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    ParallelAnimation {
        id: showAnimation
        NumberAnimation { target: control; property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.InOutQuad }
        NumberAnimation { target: control; property: "height"; from: 0.0; to: height; easing.type: Easing.InOutQuad }
    }

    ParallelAnimation {
        id: hideAnimation
        NumberAnimation { target: control; property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InOutQuad }
        NumberAnimation { target: control; property: "height"; from: height; to: 0.0; easing.type: Easing.InOutQuad }
        onStopped: control.close()
    }

    Timer {
        id: closeTimer
        interval: 3000
        onTriggered: {
            hideAnimation.start();
        }
    }

    onOpened: {
        showAnimation.start();
        if (closeTimer.interval) {
            closeTimer.start()
        }
    }

    background: Rectangle {
        id: rect
        radius: 10
        color: Style.background_popup
        anchors.fill: parent
    }

    contentItem: Item {
    }
}
