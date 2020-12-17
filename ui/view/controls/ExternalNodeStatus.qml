import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."

Item {
    id: rootItem

    property string status
    property int radius: 5

    width:  radius * 2
    height: radius * 2

    Rectangle {
        id: indicator
        x: 0
        y: 0
        width:  rootItem.radius * 2
        height: rootItem.radius * 2
        radius: rootItem.radius
        border.width: 1
    }

    DropShadow {
        id: shadow
        anchors.fill: indicator
        radius:  rootItem.radius
        samples: 9
        source:  indicator
        color:   indicator.color
        visible: color != "transparent"
    }

    states: [
        State {
            name: "uninitialized"
            when: (rootItem.status === "uninitialized")
            PropertyChanges { target: indicator; border.color: Style.content_main }
            PropertyChanges { target: indicator; color: "transparent" }
            PropertyChanges { target: indicator; opacity: 0.3 }
            PropertyChanges { target: shadow; visible: false}
        },
        State {
            name: "disconnected"
            when: (rootItem.status === "disconnected")
            PropertyChanges { target: indicator; border.color: Style.content_main }
            PropertyChanges { target: indicator; color: Style.content_main }
            PropertyChanges { target: indicator; opacity: 0.3 }
            PropertyChanges { target: shadow; visible: false}
        },
        State {
            name: "connected"
            when: (rootItem.status === "connected")
            PropertyChanges { target: indicator; border.color: Style.online }
            PropertyChanges { target: indicator; color: Style.online }
            PropertyChanges { target: indicator; opacity: 1 }
            PropertyChanges { target: shadow; visible: true}
        },
        State {
            name: "error"
            when: (rootItem.status === "error")
            PropertyChanges { target: indicator; border.color: Style.accent_fail }
            PropertyChanges { target: indicator; color: Style.accent_fail }
            PropertyChanges { target: indicator; opacity: 1 }
            PropertyChanges { target: shadow; visible: true}
        }
    ]
}
