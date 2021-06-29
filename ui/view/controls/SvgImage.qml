import QtQuick 2.11
import QtQuick.Window 2.2

Image {
    id: root

	property real dpr: Screen.devicePixelRatio

    sourceSize.width:  originalSizeImage.sourceSize.width  * 2
    sourceSize.height: originalSizeImage.sourceSize.height * 2

    width:  originalSizeImage.sourceSize.width
    height: originalSizeImage.sourceSize.height

    Image {
        id: originalSizeImage
        source: root.source
        visible: false
    }
}
