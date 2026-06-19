import QtQuick

Image {
    id: root

    // Display at the SVG's logical size, but render at device-pixel
    // resolution (sourceSize) so it stays crisp on HiDPI screens.
    sourceSize: originalSizeImage.sourceSize

    width:  originalSizeImage.implicitWidth
    height: originalSizeImage.implicitHeight
    smooth: true

    Image {
        id: originalSizeImage
        source: root.source
        visible: false
   }
}
