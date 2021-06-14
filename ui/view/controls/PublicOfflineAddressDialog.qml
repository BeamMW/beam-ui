import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

CustomDialog {
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
    padding: 30

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onOpened: {
        forceActiveFocus();
    }

    contentItem: ColumnLayout {
        spacing:                0
        RowLayout {
            SFText {
                Layout.fillWidth:       true
                Layout.leftMargin:      30
                horizontalAlignment:    Text.AlignHCenter
                font.pixelSize:         24
                color:                  Style.content_main
                //% "Public offline address"
                text:                   qsTrId("public-address-title")
            }

            CustomToolButton {
                icon.source:            "qrc:/assets/icon-cancel-16.svg"
                //% "Close"
                ToolTip.text:           qsTrId("general-close")
                onClicked: {
                    dialog.close();
                }
            }
        }

        Image {
            Layout.alignment:       Qt.AlignHCenter
            Layout.topMargin:       50
            Layout.bottomMargin:    50
            Layout.preferredWidth:  160
            Layout.preferredHeight: 160
            fillMode:               Image.PreserveAspectFit
            source:                 qrCode.data
            visible:                qrCode.data.length > 0
        }

        // Address
        SFText {
            id:                     addressField
            Layout.fillWidth:       true
            Layout.bottomMargin:    20
            Layout.maximumWidth:    400
            wrapMode:               Text.Wrap
            horizontalAlignment:    Text.AlignHCenter
            font.pixelSize:         14
            color:                  Style.content_main
        }

        // Note
        SFText {
            id:                     note
            Layout.fillWidth:       true
            Layout.maximumWidth:    400
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
                text:               qsTrId("general-copy-and-close")
                onClicked: {
                    BeamGlobals.copyToClipboard(dialog.address);
                    dialog.close()
                }
            }
        }
    }
}