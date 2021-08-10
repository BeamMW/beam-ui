import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import "controls"
import Beam.Wallet 1.0
import QtQuick.Layouts 1.12

Component {
    id: generateRecoveryPhrase

    Rectangle {
        color: Style.background_main
        property Item defaultFocusItem: nextButton

        ColumnLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            anchors.topMargin: 50
            Column {
                spacing: 30
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.preferredWidth: 730
                SFText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Qt.AlignHCenter
                    //% "Seed phrase"
                    text: qsTrId("general-seed-phrase")
                    color: Style.content_main
                    font.pixelSize: 36
                }
                SFText {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    horizontalAlignment: Qt.AlignHCenter
                    //% "Your seed phrase is the access key to all the cryptocurrencies in your wallet. Write down the phrase to keep it in a safe or in a locked vault. Without the phrase you will not be able to recover your money."
                    text: qsTrId("start-generate-seed-phrase-message")
                    color: Style.content_main
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                }
            }
            ConfirmationDialog {
                id: confirRecoveryPhrasesDialog
                //% "I understand"
                okButtonText: qsTrId("start-confirm-seed-phrase-button")
                okButtonIconSource: "qrc:/assets/icon-done.svg"
                cancelButtonVisible: false
                width: 460
                //% "It is strictly recommended to write down the seed phrase on a paper. Storing it in a file makes it prone to cyber attacks and, therefore, less secure."
                text: qsTrId("start-confirm-seed-phrase-message")
                onAccepted: {
                    onClicked: startWizzardView.push(checkRecoveryPhrase);
                }
            }
            SeedValidationHelper {
                id: seedValidationHelper
                Component.onCompleted: {
                    if (seedValidationHelper.isSeedValidatiomMode) {
                        viewModel.loadRecoveryPhraseForValidation();
                    }
                }
            }
            Grid{
                id: phrasesView
                Layout.alignment: Qt.AlignHCenter

                topPadding: 50
                columnSpacing: 30
                rowSpacing:  20

                Repeater {
                    model:viewModel.recoveryPhrases //TODO zavarza
                    Rectangle{
                        border.color: Style.background_second
                        color: "transparent"
                        width: 160
                        height: 38
                        radius: 30
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: 9
                            anchors.left: parent.left
                            color: Style.background_second
                            width: 20
                            height: 20
                            radius: 10
                            SFText {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.index + 1
                                font.pixelSize: 10
                                color: Style.content_main
                                opacity: 0.5
                            }
                        }
                        SFText {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData.phrase
                            font.pixelSize: 14
                            color: Style.content_main
                        }
                    }
                }
            }
            
            Item {
                Layout.fillHeight: true
                Layout.minimumHeight: 50
            }

            Row {
                Layout.alignment: Qt.AlignHCenter

                spacing: 30

                CustomButton {
                    //% "Back"
                    text: qsTrId("general-back")
                    icon.source: "qrc:/assets/icon-back.svg"
                    onClicked: {
                        if (seedValidationHelper.isSeedValidatiomMode) {
                            rootLoader.setSource("qrc:/main.qml");
                        } else {
                            startWizzardView.pop();
                            viewModel.resetPhrases();
                        }
                    }
                }

                CustomButton {
                    //% "I will do it later"
                    text: qsTrId("general-do-later")
                    icon.source: "qrc:/assets/icon-next-white.svg"
                    visible: !seedValidationHelper.isSeedValidatiomMode
                    onClicked: {
                        viewModel.saveSeed = true;
                        startWizzardView.push(create);
                    }
                }

                PrimaryButton {
                    id: nextButton
                    //% "Complete verification"
                    text: qsTrId("general-complete-verification-button")
                    icon.source: "qrc:/assets/icon-recovery.svg"
                    onClicked: {confirRecoveryPhrasesDialog.open();}
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
}
