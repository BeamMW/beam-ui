import QtQuick 2.15
import QtQuick.Controls 2.15
import "."

Dialog {
    background: Rectangle {
        radius: 10
        color: Style.background_popup
        anchors.fill: parent
    }
    Overlay.modal: Rectangle {
        color: Qt.rgba(Style.background_main.r, Style.background_main.g, Style.background_main.b, 0.7)
    }
}
