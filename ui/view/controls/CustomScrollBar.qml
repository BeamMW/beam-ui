import QtQuick
import QtQuick.Controls
import "."

ScrollBar {
    id: control

    policy: ScrollBar.AsNeeded

    onPositionChanged: {
        if (size < 1.0) {
            scrollHandle.show()
        }
    }

    contentItem: Rectangle {
        id: scrollHandle
        implicitWidth:  6
        implicitHeight: 6
        radius:         3
        color:          control.pressed ? Style.content_secondary : Style.content_main
        visible:        control.size < 1.0

        function show() {
            hideDelay.stop()
            opacity = control.pressed ? 0.6 : 0.8
            hideDelay.restart()
        }

        property bool shouldShow: control.pressed || control.hovered || control.active

        onShouldShowChanged: {
            if (shouldShow) {
                hideDelay.stop()
                opacity = control.pressed ? 0.6 : 0.8
            } else {
                hideDelay.restart()
            }
        }

        Timer {
            id: hideDelay
            interval: 1000
            onTriggered: scrollHandle.opacity = 0.0
        }

        opacity: 0.0
        Behavior on opacity {
            NumberAnimation { duration: 100 }
        }
    }

    background: Rectangle {
        implicitWidth:  6
        implicitHeight: 6
        color:          "transparent"
    }
}
