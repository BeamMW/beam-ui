import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import "."

CustomDialog {
    id: control

    property string message

    modal: true

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    SFText {
        anchors.fill: parent
        padding: 20
        font.pixelSize: 14
        color: Style.content_main
        wrapMode: Text.Wrap
        horizontalAlignment : Text.AlignHCenter
        text: control.message
    }
}