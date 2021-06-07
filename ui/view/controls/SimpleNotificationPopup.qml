import QtQuick 2.0
import "."

NotificationPopup {
    id: copyPopup
    width: undefined
    height: undefined
    contentItem: SFText {
        font.pixelSize: 14;
        color: Style.content_main
        text: message
    }
}