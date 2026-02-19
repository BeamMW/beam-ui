import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

WizzardPage {
    Column {
        spacing: 30
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            //% "Init wallet with Trezor"
            text: qsTrId("start-init-wallet-with-trezor")
            color: Style.content_main
            font.pixelSize: 36
        }

        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            text: viewModel.isTrezorConnected
                //% "Found device:"
                ? qsTrId("start-found-trezor-device") + " " + viewModel.trezorDeviceName
                //% "There is no device connected, please, connect your hardware wallet."
                : qsTrId("start-no-trezor-device-connected")
            color: Style.content_main
            wrapMode: Text.WordWrap
            font.pixelSize: 14
        }
    }

    buttons: [

        CustomButton {
            //% "Back"
            text: qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            onClicked: startWizzardView.pop();
        },

        PrimaryButton {
            id: nextButton
            enabled: viewModel.isTrezorConnected
            //% "Next"
            text: qsTrId("general-next")
            icon.source: "qrc:/assets/icon-next-blue.svg"
            onClicked: {
                //viewModel.startOwnerKeyImporting();
                //startWizzardView.push(importTrezorOwnerKeyPage);
                startWizzardView.push(accountLabelPage);
            }
        }
    ]
}
