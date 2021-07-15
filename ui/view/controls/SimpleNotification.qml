import QtQuick 2.0
import "."

BaseNotification {
    property alias message: txtCtrl.text
    contentItem: SFText {
        id: txtCtrl
        color: Style.content_main
        font.pixelSize: 14
    }
}
