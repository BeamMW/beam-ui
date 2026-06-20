import QtQuick
import QtQuick.Controls
import "."

CustomComboBox {
    id: control

    property var languages: []
    property int languageIndex: 0
    signal languageActivated(string language)

    fontPixelSize: 14
    enableScroll:  true

    model:        languages
    currentIndex: languageIndex
    onActivated:  control.languageActivated(currentText)
}
