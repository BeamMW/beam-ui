import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Panel {
    id: control
    property alias title:       control.title
    property var qrCode:        null
    property alias token:       tokenLabel.text
    property bool isValidToken: false
    signal tokenCopied;

    TokenInfoDialog {
        id:     infoDialog;
        token:  control.token
    }

    content: RowLayout {
        anchors.fill:   parent
        spacing:        20

        Image {
            Layout.preferredWidth:  120
            Layout.preferredHeight: 120
            fillMode:               Image.PreserveAspectFit
            source:                 control.qrCode
            visible:                control.qrCode.length > 0
        }
        ColumnLayout {
            Layout.fillWidth:            true
            Layout.alignment:            Qt.AlignTop
            RowLayout {
                Layout.topMargin:        20
                Layout.fillWidth:        true
                SFLabel {
                    id:                  tokenLabel
                    Layout.fillWidth:    true
                    font.pixelSize:      14
                    color:               Style.content_main
                    elide:               Text.ElideMiddle
                }
        
                LinkButton {
                    //% "Show token"
                    text: qsTrId("show-token")
                    linkColor: Style.accent_incoming
                    onClicked: {
                        infoDialog.open();
                    }
                }
            }
            CustomButton {
                Layout.topMargin:       20
                //% "Copy and close"
                text:                   qsTrId("wallet-receive-copy-and-close")
                Layout.preferredHeight: 38
                palette.buttonText:     Style.content_opposite
                icon.color:             Style.content_opposite
                palette.button:         Style.accent_incoming
                icon.source:            "qrc:/assets/icon-copy.svg"
                onClicked: {
                    BeamGlobals.copyToClipboard(control.token);
                    control.tokenCopied();
                }
                enabled:                control.isValidToken
            }
        }
    }
}
