import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import Beam.Wallet 1.0
import "."

Dialog {
    id:         dialog
    parent:     Overlay.overlay
    modal:      true

    x:          (parent.width - width) / 2
    y:          (parent.height - height) / 2

    width:      460
    padding:    30

    property alias dialogTitle:  titleText.text
    property alias text:         labelInput.text
    property alias placeholderText: labelInput.placeholderText

    background: Rectangle {
        radius: 10
        color:          Style.background_popup
        anchors.fill:   parent
    }


    contentItem: ColumnLayout {
        spacing:      30

        RowLayout {
            Layout.fillWidth:   true
            SFText {
                id:                     titleText
                Layout.fillWidth:       true
                horizontalAlignment:    Text.AlignHCenter
                leftPadding:            30
                font.pixelSize:         18
                font.styleName:         "Bold"
                font.weight:            Font.Bold
                color:                  Style.content_main
            }
    
            CustomToolButton {
                Layout.alignment: Qt.AlignTop
                icon.source: "qrc:/assets/icon-cancel-16.svg"
                icon.width: 16
                icon.height: 16
                //% "Close"
                ToolTip.text: qsTrId("general-close")
                onClicked: {
                    dialog.close();
                }
            }
        }

        SFTextInput {
            id:                 labelInput
            Layout.fillWidth:   true
            Layout.alignment:   Qt.AlignTop
            font.pixelSize:     14
            color:              Style.content_main
            backgroundColor:    Style.content_main

            Keys.onEnterPressed: {
                dialog.accept()
            }
            Keys.onReturnPressed: {
                dialog.acceptt()
            }
        }

        RowLayout {
            Layout.alignment:   Qt.AlignHCenter
            spacing:            20
            CustomButton {
                id:         cancelButton
                focus:      true
                icon.source: "qrc:/assets/icon-cancel-16.svg"
                //% "don't save"
                text:       qsTrId("address-dont-save")
                onClicked: { 
                    reject();
                    close();
                }
            }
 
            CustomButton {
                id:                 okButton
                icon.source:        "qrc:/assets/icon-done.svg"
                palette.button:     Style.active
                //% "save"
                text:               qsTrId("address-save")
                palette.buttonText: Style.content_opposite
                onClicked: {
                    dialog.accept()
                }
            }
        }
    }
}