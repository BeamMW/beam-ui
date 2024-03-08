import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

CustomDialog {
    id:      control
    modal:   true
    x:       (parent.width - width) / 2
    y:       (parent.height - height) / 2
    padding: 30

    property var  viewModel
    property var  addressItem
    property bool isShieldedSupported: true

    property var     token:         addressItem.token
    property var     walletID:      addressItem.walletID
    property var     isOldAddr:     addressItem.token == addressItem.walletID

    contentItem: Item { ColumnLayout {
        spacing: 0

        SFText {
            Layout.alignment: Qt.AlignHCenter
                                                //TODO: add //% "Delete address"
            text: "Delete address?"              //TODO: add qsTrId("delete-addr-title") with "Delete address"
            color: Style.content_main
            font.pixelSize: 18
            font.weight:    Font.Bold
        }

        SFText {
            Layout.topMargin: 20
            //% "Address"
            text: qsTrId("edit-addr-addr")
            color: Style.content_main
            font.pixelSize: 14
            font.weight: Font.Bold
        }

        ScrollView {
            Layout.maximumHeight:         200
            Layout.topMargin:             10
            Layout.preferredWidth:        control.isOldAddr ? 510: 582
            clip:                         true
            ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
            ScrollBar.vertical.policy:    ScrollBar.AsNeeded

            SFLabel {
                id:                       addressID
                width:                    control.isOldAddr ? 510: 582
                copyMenuEnabled:          true
                wrapMode:                 Text.Wrap
                font.pixelSize:           14
                color:                    Style.content_main
                text:                     isShieldedSupported ? control.token : control.walletID

                onCopyText: function () {
                    BeamGlobals.copyToClipboard(text)
                }
            }
        }

        RowLayout {
            Layout.topMargin: 35
            Layout.alignment: Qt.AlignHCenter
            spacing: 15

            CustomButton {
                Layout.preferredHeight: 40

                //% "Cancel"
                text:        qsTrId("general-cancel")
                icon.source: "qrc:/assets/icon-cancel-white.svg"
                icon.color:  Style.content_main

                onClicked: {
                    control.destroy()
                }
            }

            PrimaryButton {
                id: deleteButton
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignHCenter

                //% "Delete"
                text: "Delete" //TODO: add qsTrId("delete-addr-delete-button")
                icon.source: "qrc:/assets/icon-done.svg"
                onClicked: {
                viewModel.deleteAddress(contextMenu.addressItem.token)
                control.destroy()
                }
            }
        }
    }}
}