import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "."

Dialog {
    id: dialog
    modal: true

    property alias address:                 addressField.text

    QR {
        id:         qrCode
        address:    dialog.address
    }

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    
    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onClosed: {
        
    }

    onOpened: {
        forceActiveFocus();
    }

    background: Rectangle {
        radius: 10
        color: Style.background_popup
    }

    contentItem: ColumnLayout {
        spacing:                0

        SFText {
            Layout.fillWidth:       true
            Layout.topMargin:       30
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    14
            horizontalAlignment:    Text.AlignHCenter
            font.pixelSize:         18
            font.styleName:         "Bold"
            font.weight:            Font.Bold
            color:                  Style.content_main
            //% "Public offline address"
            text:                   qsTrId("public-address-title")
        }

        // Address
        SFText {
            id:                     addressField
            Layout.fillWidth:       true
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    30
            Layout.maximumWidth:    400
            wrapMode:               Text.Wrap
            font.pixelSize:         14
            color:                  Style.content_main
        }

        Image {
            id:                     qrImage
            Layout.alignment:       Qt.AlignHCenter
            Layout.preferredWidth:  120
            Layout.preferredHeight: 120
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    20
            fillMode:               Image.PreserveAspectFit
            source:                 qrCode.data
            visible:                qrCode.data.length > 0
        }

        // Note
        SFText {
            id:                     note
            Layout.fillWidth:       true
            Layout.maximumWidth:    400
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    30
            horizontalAlignment:    Text.AlignHCenter
            wrapMode:               Text.Wrap
            font.pixelSize:         14
            font.italic:            true
            color:                  Style.content_secondary
            //% "Publishing this address will allow you to be identified"
            text:                   qsTrId("public-offline-address-note")
        }

        Row {
            id:                     buttonsLayout
            Layout.fillHeight:      true
            Layout.alignment:       Qt.AlignHCenter
            Layout.leftMargin:      30
            Layout.rightMargin:     30
            Layout.bottomMargin:    30
            spacing:                20
        
            CustomButton {
                icon.source:    "qrc:/assets/icon-cancel-16.svg"
                //% "Close"
                text:           qsTrId("general-close")
                onClicked: {
                    dialog.close()
                }
            }
            
            PrimaryButton {
                icon.source:        "qrc:/assets/icon-copy-blue.svg"
                palette.buttonText: Style.content_opposite
                icon.color:         Style.content_opposite
                palette.button:     Style.accent_incoming
                text:               qsTrId("wallet-receive-copy-and-close")
                onClicked: {
                    BeamGlobals.copyToClipboard(dialog.address);
                    dialog.close()
                }
            }
        }
    }
}