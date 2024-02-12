import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

WizzardPage {
    property Item defaultFocusItem: null

    Column {
        spacing: 30
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.preferredWidth: 730
        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            //% "Restore wallet"
            text: qsTrId("general-restore-wallet")
            color: Style.content_main
            font.pixelSize: 36
        }
        SFText {
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Qt.AlignHCenter
            //% "Type in or paste your seed phrase"
            text: qsTrId("start-restore-message")
            color: Style.content_main
            wrapMode: Text.WordWrap
            font.pixelSize: 14
        }
    }

    Grid{
        Layout.alignment: Qt.AlignHCenter

        topPadding: 50
        columnSpacing: 30
        rowSpacing:  20

        Repeater {
            model:viewModel.recoveryPhrases

            Row {
                width: 160
                height: 38
                spacing: 20
                Item {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 9
                    width: 20
                    height: 20
                    Rectangle {
                        color: "transparent"
                        border.color: Style.background_second
                        width: 20
                        height: 20
                        radius: 10
                        SFText {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData.index + 1
                            font.pixelSize: 10
                            color: Style.background_second
                        }
                        visible: modelData.value.length == 0
                    }

                    Rectangle {
                        id: correctPhraseRect
                        color: modelData.isAllowed ? Style.background_second : Style.validator_error
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
                        visible: modelData.value.length > 0
                    }
                }

                SFTextInput {
                    id: phraseValue
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 6
                    width: 121
                    font.pixelSize: 14
                    color: (modelData.isAllowed || modelData.value.length == 0) ? Style.content_main : Style.validator_error
                    backgroundColor: (modelData.isAllowed || modelData.value.length == 0) ? Style.content_main : Style.validator_error
                    text: modelData.value
                    onTextEdited: {
                        var phrases = text.trim().split(viewModel.phrasesSeparator);
                        if (phrases.length > viewModel.recoveryPhrases.length) {
                            for(var i = 0; i < viewModel.recoveryPhrases.length; ++i)
                            {
                                viewModel.recoveryPhrases[i].value = phrases[i].trim();
                            }
                        }
                    }
                    Component.onCompleted: {
                        if (modelData.index == 0) {
                            defaultFocusItem = phraseValue;
                        }
                    }
                }
                Binding {
                    target: modelData
                    property: "value"
                    value: phraseValue.text
                }
            }
        }
    }

    buttons: [

        CustomButton {
            //% "Back"
            text: qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            onClicked: {
                startWizzardView.pop();
                viewModel.resetPhrases();
            }
        },

        PrimaryButton {
            id: checkRecoveryNextButton
            //% "Next"
            text: qsTrId("general-next")
            enabled: {
                var enable = true;
                if (viewModel.validateDictionary) {
                    for(var i = 0; i < viewModel.recoveryPhrases.length; ++i) {
                        enable &= viewModel.recoveryPhrases[i].isAllowed;
                    }
                }
                return enable;
            }
            icon.source: "qrc:/assets/icon-next-blue.svg"
            onClicked: {
                viewModel.validateDictionary = true;
                viewModel.useHWWallet = false;
                viewModel.isRecoveryMode = true;
                startWizzardView.push(accountLabelPage);
            }
        }
    ]

    Keys.onPressed: {
        if (event.key == Qt.Key_Shift) {
            viewModel.validateDictionary = false;
        }
    }

    Keys.onReleased: {
        if (event.key == Qt.Key_Shift) {
            viewModel.validateDictionary = true;
        }
    }
}