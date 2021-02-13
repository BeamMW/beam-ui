import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import "."

Pane {
    id: control

    property alias title:         headerTitle.text
    property alias titleTip:      headerTextLabel.text
    property alias content:       placeholder.contentItem
    property color backgroundColor:  Style.background_second
    spacing: 0
    padding: 20

    contentItem: Item {
        ColumnLayout {
            spacing: 0
            RowLayout {
                Layout.alignment: Qt.AlignTop
                SFText {
                    id: headerTitle
                    Layout.fillWidth: headerTextLabel.text.length == 0
                    color:              Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, control.enabled ? 0.5 : 0.15)

                    font {
                        styleName:      "Bold"
                        weight:         Font.Bold
                        pixelSize:      14
                        letterSpacing:  3.11
                        capitalization: Font.AllUppercase
                    }

                    visible:              text.length > 0
                }
                SFText {
                    id:                 headerTextLabel
                    Layout.fillWidth:   true
                    color:              Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)

                    font {
                        styleName:      "Bold"
                        weight:         Font.Bold
                        pixelSize:      14
                        letterSpacing:  0.35
                    }
                    visible:              text.length > 0 && control.enabled
                }
            }

            Control {
                id:                placeholder
                Layout.fillWidth:  true
                Layout.topMargin:  20
                Layout.alignment:  Qt.AlignTop
                visible:           control.enabled
            }
        }
    }

    background: Rectangle {
        radius:  10
        color:   control.enabled ? control.backgroundColor : Qt.rgba(control.backgroundColor.r, control.backgroundColor.g, control.backgroundColor.b, control.backgroundColor.a*0.3)
    }
}
