import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

Rectangle {
    color: Style.background_main
    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        anchors.topMargin: 50

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

        Item {
            Layout.fillHeight: true
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 30

            CustomButton {
                //% "Back"
                text: qsTrId("general-back")
                icon.source: "qrc:/assets/icon-back.svg"
                onClicked: startWizzardView.pop();
            }

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
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 67
            Layout.maximumHeight: 143
        }

        VersionFooter {}
    }
}