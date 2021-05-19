import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Panel {
    id: control

    property string addressLabel:       ""
    property bool showQrCode:           true
    property alias token:               tokenLabel.text
    property string amount:             ""
    property bool isValidToken:         false
    property alias ignoreStoredVouchers:infoDialog.ignoreStoredVouchers
    property alias headerItem:          headerPlaceholder.contentItem
    property alias headerVisible:       headerPlaceholder.visible
    property alias footerItem:          footerPlaceholder.contentItem
    property alias footerVisible:       footerPlaceholder.visible

    signal tokenCopied
    signal closed

    backgroundColor:  Style.background_button

    TokenInfoDialog {
        id:                     infoDialog
        token:                  control.token
        onAddressCopied: {
            control.tokenCopied()
        }
    }

    content: ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Control {
            id:                          headerPlaceholder
            Layout.fillWidth:            true
            visible:                     false
            bottomPadding:               20
        }

        RowLayout {
            id: contentRow
            Layout.fillWidth:         true
            Layout.topMargin:         -8
            Layout.bottomMargin:      -8
            spacing:                  0
            SFLabel {
                id:                     tokenLabel
                Layout.preferredWidth:  180
                font.pixelSize:         14
                color:                  Style.content_main
                elide:                  Text.ElideMiddle
            }
            CustomToolButton {
                Layout.alignment:       Qt.AlignVCenter
                Layout.leftMargin:      4
                icon.source:            "qrc:/assets/icon-copy-blue.svg"
                //% "Copy"
                ToolTip.text:           qsTrId("general-copy")
                onClicked: {
                    BeamGlobals.copyToClipboard(control.token);
                    control.tokenCopied();
                }
            }
            CustomToolButton {
                Layout.alignment:       Qt.AlignVCenter
                icon.source:            "qrc:/assets/icon-qr.svg"
                //% "QR Code"
                ToolTip.text:           qsTrId("qr-code")
                visible:                control.showQrCode
                onClicked: {
                    var popup = Qt.createComponent("AddressQRDialog.qml").createObject(main)
                    popup.address = control.token;
                    popup.addressLabelText = control.addressLabel;
                    popup.amount = control.amount;
                    popup.addressCopied.connect(function(){
                        control.tokenCopied();
                    })
                    popup.open();
                }
            }
            Item {
                Layout.fillWidth:       true
            }
            LinkButton {
                //% "More details"
                text:                   qsTrId("more-details")
                linkColor:              Style.accent_incoming
                onClicked: {
                    infoDialog.open();
                }
            }
        }
        Control {
            id:                          footerPlaceholder
            Layout.fillWidth:            true
            visible:                     false
            topPadding:                  6
        }
        CustomButton {
            id: copyButton
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
                control.closed();
            }
            enabled:                control.isValidToken
        }
    }
}
