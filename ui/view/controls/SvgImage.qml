import QtQuick 2.11
import QtQuick.Window 2.2

Image {
    id: root

	property real dpr: Screen.devicePixelRatio

    sourceSize.width:  width  * dpr
    sourceSize.height: height * dpr

    width:  originalSizeImage.sourceSize.width
    height: originalSizeImage.sourceSize.height

    Image {
        id: originalSizeImage
        source: root.source
        visible: false
   }
}
