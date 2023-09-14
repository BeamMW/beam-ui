import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: control

    width:   460
    parent:  Overlay.overlay
    x:       Math.round((parent.width - width) / 2)
    y:       Math.round((parent.height - height) / 2)
    modal:   true

    property alias showAddressesDialogTitle: showAddressesDialogTitleId.text
    property var   addresses: undefined

    // hack
    implicitHeight: (addresses != undefined && addresses.length > 1) ? 400 : 220

    contentItem: ColumnLayout {
        spacing: 0
        anchors.fill:    parent
        anchors.margins: 30

        // title
        SFText {
            id: showAddressesDialogTitleId
            Layout.fillWidth:     true
            color:                Style.white
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       18
            font.weight:          Font.Bold
        }

        // body
        ScrollView {
            Layout.fillWidth:          true
            Layout.fillHeight:         true
            Layout.topMargin:          35
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            clip:                      true

            ColumnLayout {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                spacing:           30

                Repeater {
                    model: control.addresses
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        SFLabel {
                            id: addressId
                            Layout.fillWidth:    true
                            horizontalAlignment: Text.AlignLeft
                            text:                modelData
                            font.pixelSize:      14
                            color:               Style.content_main
                            copyMenuEnabled:     true
                            onCopyText:          BeamGlobals.copyToClipboard(text)
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 50
                        }

                        CustomToolButton {
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            icon.source: "qrc:/assets/icon-copy.svg"
                            //% "Copy address"
                            ToolTip.text: qsTrId("settings-swap-copy-address")
                            ToolTip.visible: hovered
                            ToolTip.delay: 500
                            ToolTip.timeout: 2000
                            hoverEnabled: true
                            onClicked: BeamGlobals.copyToClipboard(addressId.text)
                        }
                    }
                }
            }
        }

        // buttons
        CustomButton {
            Layout.topMargin:     24
            Layout.alignment:     Qt.AlignHCenter
            text:             qsTrId("general-close")
            icon.source:      "qrc:/assets/icon-cancel-white.svg"
            onClicked:        control.close()
        }
    }
}