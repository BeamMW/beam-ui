import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import "."

Pane {
    id: control

    property alias title: headerTitle.text
    property alias content: placeholder.contentItem
    spacing: 0
    padding: 20

    contentItem: ColumnLayout {
        spacing: 0
        //clip:    true

        SFText {
            id:    headerTitle
            color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
        
            font {
                styleName:      "Medium"
                weight:         Font.Medium
                pixelSize:      14
                letterSpacing:  3.11
                capitalization: Font.AllUppercase
            }
        
            Layout.bottomMargin:  25
            visible:              text.length > 0
        }
        
        Control {
            id:                placeholder
            Layout.fillWidth:  true
            Layout.fillHeight: true
            Layout.alignment:  Qt.AlignTop
        }
    }

    background: Rectangle {
        radius:  10
        color:   Style.background_second
    }
}
