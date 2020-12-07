import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import "."

Dialog {
    id: control

    width:       800
    height:      430
    parent:      Overlay.overlay
    x:           Math.round((parent.width - width) / 2)
    y:           Math.round((parent.height - height) / 2)
    closePolicy: Popup.NoAutoClose
    modal:       true
    visible:     false
        
    property string showSeedDialogTitle:                 ""
    property string phrasesSeparatorElectrum:            ""
    property var    seedPhrasesElectrum:                 undefined
    property bool   isCurrentElectrumSeedValid:          false
    property bool   isCurrentElectrumSeedSegwitAndValid: false
    property bool   isSeedChanged:                       false
    property bool   isAllWordsAllowed:                   true

    signal newSeedElectrum
    signal copySeedElectrum
    signal validateFullSeedPhrase

    Component.onCompleted: {
        updateIsAllWordsAllowed();
    }

    function setModeEdit() {
        seedDialogContent.state = "editPhrase";
    }

    function setModeView() {
        seedDialogContent.state = "viewPhrase";
    }

    function setModeNew() {
        seedDialogContent.state = "newPhrase";
    }

    function applySeedPhrase() {
        for(var i = 0; i < control.seedPhrasesElectrum.length; ++i)
        {
            control.seedPhrasesElectrum[i].applyChanges();
        }
        control.close();
        control.isSeedChanged = false;
    }

    function undoChanges() {
        for(var i = 0; i < control.seedPhrasesElectrum.length; ++i)
        {
            control.seedPhrasesElectrum[i].revertChanges();
        }
        validateFullSeedPhrase();
        control.close();
        control.isSeedChanged = false;
    }

    function updateIsSeedChanged() {
        var isChanged = false;
        for(var i = 0; i < control.seedPhrasesElectrum.length; ++i) {
            if (control.seedPhrasesElectrum[i].isModified) {
                isChanged = true;
                break;
            }
        }
        isSeedChanged = isChanged;
    }

    function updateIsAllWordsAllowed() {
        for (var i = 0; i < control.seedPhrasesElectrum.length; ++i) {
            if (!control.seedPhrasesElectrum[i].isAllowed) {
                isAllWordsAllowed = false;
                return;
            }
        }
        isAllWordsAllowed = true;
    }

    background: Rectangle {
        radius:       10
        color:        Style.background_popup
        anchors.fill: parent
    }

    contentItem: 
    ColumnLayout {
        id: seedDialogContent
        anchors.fill:          parent
        anchors.margins:       30
        spacing:               0
        property bool canEdit: false
        state:                 "newPhrase"

        // Title: New seed phrase / Enter your seed phrase / "coin" seed phrase
        SFText {
            id: seedDialogTitle
            Layout.fillWidth:    true
            color:               Style.white
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize:      18
            font.weight:         Font.Bold
        }

        // additional comment (only on "New seed phrase"):
        SFText {
            id: additionalInfo
            Layout.topMargin:    14
            Layout.fillWidth:    true
            visible:             false
            color:               Style.white
            wrapMode:            Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize:      14
            font.weight:         Font.Normal
            //% "Your seed phrase is the access key to all the funds! Print or write down the phrase and keep it in a safe or in a locked vault. Without the phrase you will not be able to recover your money."
            text: qsTrId("swap-seed-info-message")
        }
            
        // body: seed phrase
        GridLayout {
            Layout.topMargin:    50
            Layout.bottomMargin: invalidSeedWarning.visible ? 6 : 50
            columns:             4
            columnSpacing:       30
            rowSpacing:          20

            Repeater {
                model: seedPhrasesElectrum
                Rectangle {
                    id:           phraseItem
                    border.color: seedDialogContent.canEdit ? "transparent" : Style.background_second
                    color:        "transparent"
                    width:        160
                    height:       38
                    radius:       30
            
                    RowLayout {
                        spacing:      0
                        anchors.fill: parent
            
                        // index
                        Rectangle {
                            Layout.leftMargin: 9

                            color:  Style.background_second
                            width:  20
                            height: 20
                            radius: 10
            
                            SFText {
                                anchors.centerIn: parent
                                text:             modelData.index + 1
                                font.pixelSize:   10
                                color:            Style.content_main
                            }
                        }

                        // inpunt
                        SFTextInput {
                            id: phraseValue
                            visible:               seedDialogContent.canEdit
                            Layout.leftMargin:     10
                            Layout.preferredWidth: 110

                            font.pixelSize:   14
                            color:            (modelData.isAllowed || modelData.value.length == 0) ? Style.content_main : Style.validator_error
                            backgroundColor:  (modelData.isAllowed || modelData.value.length == 0) ? Style.content_main : Style.validator_error
                            text:             modelData.value

                            onTextEdited: {
                                var phrases = text.split(phrasesSeparatorElectrum);
                                if (phrases.length >= control.seedPhrasesElectrum.length) {
                                    for(var i = 0; i < control.seedPhrasesElectrum.length; ++i)
                                    {
                                        control.seedPhrasesElectrum[i].value = phrases[i];
                                    }
                                }
                            }

                            Binding {
                                target:   modelData
                                property: "value"
                                value:    phraseValue.text
                            }

                            Connections {
                                target: modelData
                                onValueChanged: {
                                    control.updateIsSeedChanged();
                                    control.validateFullSeedPhrase()
                                    control.updateIsAllWordsAllowed()
                                }
                            }
                        }

                        SFText {
                            visible:               !seedDialogContent.canEdit
                            Layout.leftMargin:     10
                            Layout.preferredWidth: 110
                            horizontalAlignment:   Text.AlignLeft
                            text:                  modelData.phrase
                            font.pixelSize:        14
                            color:                 Style.content_main
                        }
                    }
                }
            }
        }

        SFText {
            id: invalidSeedWarning
            Layout.fillWidth:     true
            Layout.bottomMargin:  28
            text:                 isCurrentElectrumSeedSegwitAndValid ? 
                                    //% "Segwit seed phrase is not supported yet."
                                    qsTrId("settings-swap-seed-segwit-warning") :
                                    //% "Invalid seed phrase. Please check again and resubmit."
                                    qsTrId("settings-swap-seed-invali-warning")
            color:                Style.validator_error
            font.pixelSize:       12
            font.italic:          true
            width:                parent.width
            horizontalAlignment:  Text.AlignHCenter
            visible:              isCurrentElectrumSeedSegwitAndValid || 
                                    (!isCurrentElectrumSeedValid && control.isAllWordsAllowed && control.isSeedChanged)
        }

        // buttons
        RowLayout {
            spacing:                20
            Layout.fillWidth:       true
            Layout.preferredHeight: 38

            Item {
                Layout.fillWidth: true
            }

            // editPhrase: "cancel" "apply"
            CustomButton {
                id: cancelButtonId
                visible:                false
                Layout.minimumWidth:    133
                text:                   qsTrId("general-cancel")
                icon.source:            "qrc:/assets/icon-cancel-white.svg"
                enabled:                true
                onClicked:              control.undoChanges()
            }

            PrimaryButton {
                id: applyButtonId
                visible:                false
                Layout.minimumWidth:    126
                text:                   qsTrId("settings-apply")
                icon.source:            "qrc:/assets/icon-done.svg"
                enabled:                control.isCurrentElectrumSeedValid && 
                                        control.isSeedChanged && control.isAllWordsAllowed;
                onClicked:              control.applySeedPhrase()
            }

            // viewPhrase: "close" "copy"
            // newPhrase:  "close" "generate another seed phrase" "copy"
            CustomButton {
                id: closeButtonId
                visible:                false
                Layout.minimumWidth:    125
                text:                   qsTrId("general-close")
                icon.source:            "qrc:/assets/icon-cancel-white.svg"
            }

            CustomButton {
                id: generateButtonId
                visible:                false
                Layout.minimumWidth:    271
                rightPadding:           20
                //% "generate another seed phrase"
                text:                   qsTrId("settings-swap-seed-generate")
                icon.source:            "qrc:/assets/icon-repeat-white.svg"
                onClicked:              control.newSeedElectrum();
            }

            PrimaryButton {
                id: copyButtonId
                visible:                false
                Layout.minimumWidth:    124
                text:                   qsTrId("general-copy")
                icon.source:            "qrc:/assets/icon-copy-blue.svg"
                onClicked:              control.copySeedElectrum();
            }

            Item {
                Layout.fillWidth: true
            }
        }

        states: [
            State {
                name: "newPhrase"
                PropertyChanges {
                    target: seedDialogTitle
                    //% "New seed phrase"
                    text: qsTrId("swap-seed-new")
                }
                PropertyChanges {
                    target: additionalInfo
                    visible: true
                }
                PropertyChanges {
                    target: closeButtonId
                    visible: true
                    onClicked: {
                        control.close()
                    }
                }
                PropertyChanges {
                    target: generateButtonId
                    visible: true
                }
                PropertyChanges {
                    target: copyButtonId
                    visible: true
                }
                PropertyChanges {
                    target: control
                    isSeedChanged: true
                }
            },
            State {
                name: "editPhrase"
                PropertyChanges {
                    target: seedDialogTitle
                    //% "Enter your seed phrase"
                    text: qsTrId("swap-seed-edit")
                }
                PropertyChanges {
                    target: control
                    height: 380
                }
                PropertyChanges {
                    target: seedDialogContent
                    canEdit: true
                }
                PropertyChanges {
                    target: cancelButtonId
                    visible: true
                }
                PropertyChanges {
                    target: applyButtonId
                    visible: true
                }
            },
            State {
                name: "viewPhrase"
                PropertyChanges {
                    target: seedDialogTitle
                    text: showSeedDialogTitle
                }
                PropertyChanges {
                    target: control
                    height: 380
                }
                PropertyChanges {
                    target: closeButtonId
                    visible: true
                    onClicked: control.close()
                }
                PropertyChanges {
                    target: copyButtonId
                    visible: true
                }
            }
        ]
    }
}