import QtQuick 2.11
import QtQuick.Window 2.2

Image {
    id: root

    property real dpr: (Screen.devicePixelRatio == 1.0 ? 2 : Screen.devicePixelRatio)

    sourceSize: Qt.size(originalSizeImage.sourceSize.width * dpr, originalSizeImage.sourceSize.height * dpr)

    width:  originalSizeImage.sourceSize.width
    height: originalSizeImage.sourceSize.height
    smooth: true

    Image {
        id: originalSizeImage
        source: root.source
        visible: false
   }
}
