import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: dialog
    modal: true
    property alias address:                 addressField.text
    property string addressLabelText:       ""
    property alias amount:                  qrCode.amount
    signal addressCopied;

    QR {
        id:         qrCode
        address:    dialog.address
    }

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    parent:         Overlay.overlay
    padding:        30

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onOpened: {
        forceActiveFocus();
    }

    onClosed : {
        dialog.destroy();
    }

    contentItem: ColumnLayout {
        spacing:        0
        RowLayout {
            SFText {
                Layout.fillWidth:       true
                Layout.leftMargin:      30
                horizontalAlignment:    Text.AlignHCenter
                font.pixelSize:         24
                color:                  Style.content_main
                //: show QR dialog title
                //% "QR code"
                text:                   qsTrId("show-qr-title")
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
            Layout.preferredWidth:  200
            Layout.preferredHeight: 200
            fillMode:               Image.PreserveAspectFit
            source:                 qrCode.data
            visible:                qrCode.data.length > 0
        }

        SFText {
            Layout.alignment:       Qt.AlignHCenter
            text:                   addressLabelText + ":"
            color:                  Style.content_main
            font.pixelSize:         14
            font.styleName:         "Bold"
            font.weight:            Font.Bold
            visible:                addressLabelText.length
        }
        
        SFLabel {
            id:                     addressField
            Layout.preferredWidth:  450
            Layout.topMargin:       10
            Layout.bottomMargin:    30
            horizontalAlignment:    Text.AlignHCenter
            color:                  Style.content_secondary
            font.pixelSize:         14
            wrapMode:               Text.WrapAnywhere 
            copyMenuEnabled:        true
            onCopyText:             {
                BeamGlobals.copyToClipboard(text)
                addressCopied();
            }
        }

        CustomButton {
            //% "Close"
            text:               qsTrId("general-close")
            Layout.alignment:   Qt.AlignHCenter
            icon.source:        "qrc:/assets/icon-cancel-16.svg"
            onClicked:          dialog.close()
        }
    }
}
