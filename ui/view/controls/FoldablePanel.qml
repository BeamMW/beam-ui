import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import "."

Pane {
    id: control

    property alias title:      headerTitle.text
    property alias headerText: headerTextLabel.text
    property alias content:    placeholder.contentItem
    property bool  folded:     true
    spacing: 0
    padding: 20

    contentItem: ColumnLayout {
        spacing: 0
        //clip:    true
        RowLayout {
            Layout.alignment: Qt.AlignTop
            SFText {
                id:                 headerTitle
                Layout.fillWidth:   headerTextLabel.text.length == 0
                color:              Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
        
                font {
                    styleName:      "Bold"
                    weight:         Font.Bold
                    pixelSize:      14
                    letterSpacing:  3.11
                    capitalization: Font.AllUppercase
                }
        
                visible:              text.length > 0

                MouseArea {
                    anchors.fill:       parent
                    acceptedButtons:    Qt.LeftButton
                    cursorShape:        Qt.PointingHandCursor
                    onClicked: {
                        control.folded = !control.folded;
                    }
                }
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
        
                visible:              text.length > 0

                MouseArea {
                    anchors.fill:       parent
                    acceptedButtons:    Qt.LeftButton
                    cursorShape:        Qt.PointingHandCursor
                    onClicked: {
                        control.folded = !control.folded;
                    }
                }
            }
            SvgImage {
                Layout.alignment:       Qt.AlignCenter
                Layout.maximumHeight:   8
                Layout.maximumWidth:    13
                source:                 control.folded ? "qrc:/assets/icon-grey-arrow-down.svg" : "qrc:/assets/icon-grey-arrow-up.svg"
                MouseArea {
                    anchors.fill:       parent
                    acceptedButtons:    Qt.LeftButton
                    cursorShape:        Qt.PointingHandCursor
                    onClicked: {
                        control.folded = !control.folded;
                    }
                }
            } 
        }

        Control {
            id:                     placeholder
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            Layout.topMargin:       20
            Layout.alignment:       Qt.AlignTop
            visible:                !control.folded
        }
    }

    background: Rectangle {
        radius:  10
        color:   Style.background_second
    }
}
