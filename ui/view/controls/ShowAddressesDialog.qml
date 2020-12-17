import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Dialog {
    id: control

    width:   460
    height:  400
    parent:  Overlay.overlay
    x:       Math.round((parent.width - width) / 2)
    y:       Math.round((parent.height - height) / 2)
    modal:   true

    property alias showAddressesDialogTitle: showAddressesDialogTitleId.text
    property var   addresses: undefined

    background: Rectangle {
        radius:       10
        color:        Style.background_popup
        anchors.fill: parent
    }

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
            Layout.topMargin:          50
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
                            Layout.preferredWidth: 55
                        }

                        CustomToolButton {
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            icon.source: "qrc:/assets/icon-copy.svg"
                            //% "Copy address"
                            ToolTip.text: qsTrId("settings-swap-copy-address")
                            onClicked: BeamGlobals.copyToClipboard(addressId.text)
                        }
                    }
                }
            }
        }

        // buttons
        CustomButton {
            Layout.topMargin:       24
            Layout.alignment:       Qt.AlignHCenter
            text:             qsTrId("general-close")
            icon.source:      "qrc:/assets/icon-cancel-white.svg"
            onClicked:        control.close()
        }
    }
}