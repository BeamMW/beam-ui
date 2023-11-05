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
    property Item defaultFocusItem: generateRecoveryPhraseButton

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
                //% "Create new wallet"
                text: qsTrId("general-create-wallet")
                color: Style.content_main
                font.pixelSize: 36
            }
            SFText {
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
                //% "Create new wallet with generating seed phrase."
                text: qsTrId("start-create-new-message-line-1")
                color: Style.content_main
                wrapMode: Text.WordWrap
                font.pixelSize: 14
            }
            SFText {
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
                //% "If you ever lose your device, you will need this phrase to recover your wallet!"
                text: qsTrId("start-create-new-message-line-2")
                color: Style.content_main
                wrapMode: Text.WordWrap
                font.pixelSize: 14
            }
        }

        Row {
            topPadding: 100
            spacing: 65
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumHeight : 300
            Layout.maximumHeight: 500
            SecurityNote{
                iconSource: "qrc:/assets/eye.svg"
                //% "Do not let anyone see your seed phrase"
                text: qsTrId("start-create-new-securiry-note-1")
            }
            SecurityNote{
                iconSource: "qrc:/assets/password.svg"
                //% "Never type your seed phrase into password managers or elsewhere"
                text: qsTrId("start-create-new-securiry-note-2")
            }
            SecurityNote{
                iconSource: "qrc:/assets/copy-two-paper-sheets-interface-symbol.svg"
                //% "Keep the copies of your seed phrase in a safe place"
                text: qsTrId("start-create-new-securiry-note-3")
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
                id: generateRecoveryPhraseButton
                //% "Generate seed phrase"
                text: qsTrId("start-generate-seed-phrase-button")
                icon.source: "qrc:/assets/icon-recovery.svg"
                onClicked: startWizzardView.push(generateRecoveryPhrasePage);
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