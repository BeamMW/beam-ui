import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

SettingsFoldable {
    id:            control
    height:        362

    property string generalTitle:             ""
    property alias  showSeedDialogTitle:      seedPhraseDialog.showSeedDialogTitle
    property alias  showAddressesDialogTitle: showAddressesDialog.showAddressesDialogTitle
    property string color:                    Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
    property string disabledColor:            Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.2)

    property bool   isConnected:           false
    property var    mainSettingsViewModel: undefined
    property bool   canChangeConnection:             true
    property bool   canEdit:  !control.isConnected

    // TODO roman.strilets it's for test
    property alias  address:              addressInput.text
    property alias  port:                 portInput.text
    property alias  accountIndex:         accountIndexInput.text
    property alias  contractAddress:      contractAddressInput.text
    property alias  erc20ContractAddress: erc20ContractAddressInput.text
    property alias  daiContractAddress:   daiContractAddressInput.text
    property alias  usdtContractAddress:  usdtContractAddressInput.text
    property alias  wbtcContractAddress:  wbtcContractAddressInput.text

    property alias activateDai:  activateDaiSwitch.checked
    property alias activateUsdt: activateUsdtSwitch.checked
    property alias activateWBTC: activateWBTCSwitch.checked

    property alias seedPhrases:        seedPhraseDialog.seedPhrasesElectrum
    property alias phrasesSeparator:   seedPhraseDialog.phrasesSeparatorElectrum
    property bool  isCurrentSeedValid: false

    // function to get ethereum addresses
    property var   getEthereumAddresses:       undefined

    ConfirmPasswordDialog {
        id: confirmPasswordDialog
        parent: control.parent
        settingsViewModel: mainSettingsViewModel 
    }

    SeedPhraseDialog {
        id: seedPhraseDialog;

        onNewSeedElectrum:        control.newSeedPhrases()
        onCopySeedElectrum:       control.copySeedPhrases()
        onValidateFullSeedPhrase: control.validateCurrentSeedPhrase()
        onClosed: {
            internalValues.isSeedChanged = seedPhraseDialog.isSeedChanged
        }
    }

    ShowAddressesDialog {
        id: showAddressesDialog
    }

    //
    // signals
    //
    signal disconnect
    signal applySettings
    signal clearSettings
    signal connectToNode
    signal newSeedPhrases
    signal restoreSeedPhrases
    signal copySeedPhrases
    signal validateCurrentSeedPhrase

    QtObject {
        id: internalValues
        property string initialAddress
        property string initialPort
        property string initialSeed
        property string initialContractAddress
        property string initialERC20ContractAddress
        property string initialDaiContractAddress
        property string initialUsdtContractAddress
        property string initialWbtcContractAddress
        property string initialAccountIndex
        property bool initialActivateDai
        property bool initialActivateUsdt
        property bool initialActivateWBTC

        property bool   isSeedChanged: false

        function restore() {
            isSeedChanged = false
            address = initialAddress
            port = initialPort
            control.restoreSeedPhrases()

            contractAddress = initialContractAddress
            erc20ContractAddress = initialERC20ContractAddress
            daiContractAddress = initialDaiContractAddress
            usdtContractAddress = initialUsdtContractAddress
            wbtcContractAddress = initialWbtcContractAddress
            accountIndex = initialAccountIndex

            activateDai = initialActivateDai
            activateUsdt = initialActivateUsdt
            activateWBTC = initialActivateWBTC
        }

        function save() {
            initialAddress = address
            initialPort    = port
            isSeedChanged = false
            initialContractAddress = contractAddress
            initialERC20ContractAddress = erc20ContractAddress
            initialDaiContractAddress = daiContractAddress
            initialUsdtContractAddress = usdtContractAddress
            initialWbtcContractAddress = wbtcContractAddress
            initialAccountIndex = accountIndex
            initialActivateDai = activateDai
            initialActivateUsdt = activateUsdt
            initialActivateWBTC = activateWBTC
        }

        function isChanged() {
            return initialAddress !== address || initialPort !== port || isSeedChanged || contractAddress != initialContractAddress
            || daiContractAddress != initialDaiContractAddress || usdtContractAddress != initialUsdtContractAddress || wbtcContractAddress != initialWbtcContractAddress
            || initialAccountIndex != accountIndex || erc20ContractAddress != initialERC20ContractAddress
            || initialActivateDai != activateDai || initialActivateUsdt != activateUsdt || initialActivateWBTC != activateWBTC
        }
    }

    function isSettingsChanged() {
        return internalValues.isChanged();
    }

    function canApplySettings() {
        // TODO roman.strilets for test
        return isCurrentSeedValid /*&& addressInput.isValid && portInput.acceptableInput*/;
    }

    function applyChanges() {
        internalValues.save();
        control.applySettings();
    }

    function restoreSettings() {
        internalValues.restore();
    }

    function haveSettings() {
        // TODO roman.strilets for test
        return isCurrentSeedValid /*&& addressInput.isValid && portInput.acceptableInput*/;
    }

    function clear() {
        control.clearSettings();
        internalValues.save();
    }

    function canClear() {
        return !isConnected && (internalValues.initialAddress.length || isCurrentSeedValid);
    }

    function canConnect() {
        return !isSettingsChanged() && haveSettings() && !isConnected;
    }

    function canDisconnect() {
        return isConnected && control.canChangeConnection;
    }

    Component.onCompleted: {
        //control.editElectrum = control.isElectrumConnection || (!control.canChangeConnection && !control.isNodeConnection);
        internalValues.save();
    }

    content: ColumnLayout {
        spacing: 0

        GridLayout {
            //visible: !(editElectrum && useRandomNode.checked && !isElectrumConnection)
            Layout.topMargin: 20
            columns:          2
            columnSpacing:    50
            rowSpacing:       13

            SFText {
                //visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Node address"
                text:           qsTrId("settings-node-address")
            }

            // TODO roman.strilets it's for test
            /*IPAddrInput {
                id:               addressInput
                //visible:          !editElectrum
                Layout.fillWidth: true
                color:            Style.content_main
                //underlineVisible: canEdit
                //readOnly:         !canEdit
                ipOnly:           false
            }*/

            SFTextInput {
                id:               addressInput
                //visible:          !editElectrum
                Layout.fillWidth: true
                color:            Style.content_main
                font.pixelSize:     14
                activeFocusOnTab:   true
                underlineVisible: canEdit
                readOnly:         !canEdit
            }

            SFText {
                //visible:        !editElectrum
                visible: false
                font.pixelSize: 14
                color:          control.color
                text:           qsTrId("settings-local-node-port")
            }

            SFTextInput {
                id:                 portInput
                //visible:            !editElectrum
                visible: false
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
                validator: IntValidator {
                    bottom: 1
                    top: 65535
                }
            }

            SFText {
                //visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Account index"
                text:           qsTrId("settings-account-index")
            }

            SFTextInput {
                id:                 accountIndexInput
                //visible:            !editElectrum
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
                validator: IntValidator {
                    bottom: 0
                    top: 20
                }
            }

            SFText {
                //visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Contract address"
                text:           qsTrId("settings-contract-address")
            }

            SFTextInput {
                id:                 contractAddressInput
                //visible:            !editElectrum
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "ERC20 contract address"
                text:           qsTrId("settings-erc20-contract-address")
            }

            SFTextInput {
                id:                 erc20ContractAddressInput
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "DAI contract address"
                text:           qsTrId("settings-dai-contract-address")
            }

            SFTextInput {
                id:                 daiContractAddressInput
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "USDT contract address"
                text:           qsTrId("settings-usdt-contract-address")
            }

            SFTextInput {
                id:                 usdtContractAddressInput
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "WBTC contract address"
                text:           qsTrId("settings-wbtc-contract-address")
            }

            SFTextInput {
                id:                 wbtcContractAddressInput
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEdit
                readOnly:           !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "Activate DAI"
                text:           qsTrId("settings-activate-dai")
            }

            CustomSwitch {
                id:          activateDaiSwitch
                //alwaysGreen: true
                spacing:     0
                enabled:     canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "Activate USDT"
                text:           qsTrId("settings-activate-usdt")
            }

            CustomSwitch {
                id:          activateUsdtSwitch
                //alwaysGreen: true
                spacing:     0
                enabled:     canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "Activate WBTC"
                text:           qsTrId("settings-activate-wbtc")
            }

            CustomSwitch {
                id:          activateWBTCSwitch
                //alwaysGreen: true
                spacing:     0
                enabled:     canEdit
            }
        }

        // seed: new || edit
        RowLayout {
            visible:             canEdit
            spacing:             20
            Layout.fillWidth:    true
            Layout.topMargin:    30
            Layout.bottomMargin: 7

            LinkButton {
                text:      isCurrentSeedValid ?   
                            //% "Edit your seed phrase"
                            qsTrId("settings-swap-edit-seed") :
                            //% "Enter your seed phrase"
                            qsTrId("settings-swap-enter-seed")
                onClicked: {
                    function editSeedPhrase() {
                        seedPhraseDialog.setModeEdit();
                        seedPhraseDialog.isCurrentElectrumSeedValid = Qt.binding(function(){return isCurrentSeedValid;});
                        seedPhraseDialog.open();
                    }
                    if (isCurrentSeedValid) {
                        //: electrum settings, ask password to edit seed phrase, dialog title
                        //% "Edit seed phrase"
                        confirmPasswordDialog.dialogTitle = qsTrId("settings-swap-confirm-edit-seed-title");
                        //: electrum settings, ask password to edit seed phrase, dialog message
                        //% "Enter your wallet password to edit the phrase"
                        confirmPasswordDialog.dialogMessage = qsTrId("settings-swap-confirm-edit-seed-message");
                        confirmPasswordDialog.onDialogAccepted = function() {
                            editSeedPhrase();
                        };
                        confirmPasswordDialog.open();
                    } else {
                        editSeedPhrase();
                    }
                }
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "or"
                text:           qsTrId("settings-swap-label-or")
            }

            LinkButton {
                //% "Generate new seed phrase"
                text:             qsTrId("settings-swap-new-seed")
                onClicked: {
                    function generateSeedPhrase() {
                        newSeedPhrases();
                        seedPhraseDialog.setModeNew();
                        seedPhraseDialog.isCurrentElectrumSeedValid = Qt.binding(function(){return isCurrentSeedValid;});
                        seedPhraseDialog.open();
                    }

                    if (isCurrentSeedValid) {
                        //: electrum settings, ask password to generate new seed phrase, dialog title
                        //% "Generate new seed phrase"
                        confirmPasswordDialog.dialogTitle = qsTrId("settings-swap-confirm-generate-seed-title");
                        //: electrum settings, ask password to generate new seed phrase, dialog message
                        //% "Enter your wallet password to generate new seed phrase"
                        confirmPasswordDialog.dialogMessage = qsTrId("settings-swap-confirm-generate-seed-message");
                        confirmPasswordDialog.onDialogAccepted = function() {
                            generateSeedPhrase();
                        };
                        confirmPasswordDialog.open();
                    } else {
                        generateSeedPhrase();
                    }
                }
            }
        }

        // alert text if we have active transactions
        SFText {
            visible:               !control.canChangeConnection
            Layout.topMargin:      30
            Layout.preferredWidth: 390
            Layout.alignment:      Qt.AlignVCenter | Qt.AlignHCenter
            horizontalAlignment:   Text.AlignHCenter
            verticalAlignment:     Text.AlignVCenter
            font.pixelSize:        14
            wrapMode:              Text.WordWrap
            color:                 control.color
            lineHeight:            1.1
            text:                  qsTrId("settings-doge-node-progress")
        }

        // show seed
        RowLayout {
            visible:             !canEdit
            spacing:             20
            Layout.fillWidth:    true
            Layout.topMargin:    30
            Layout.bottomMargin: 7

            LinkButton {
                //% "Show seed phrase"
                text:      qsTrId("settings-swap-show-seed")
                onClicked: {
                    //: electrum settings, ask password to show seed phrase, dialog title
                    //% "Show seed phrase"
                    confirmPasswordDialog.dialogTitle = qsTrId("settings-swap-confirm-show-seed-title");
                    //: electrum settings, ask password to show seed phrase, dialog message
                    //% "Enter your wallet password to see the phrase"
                    confirmPasswordDialog.dialogMessage = qsTrId("settings-swap-confirm-show-seed-message");
                    confirmPasswordDialog.onDialogAccepted = function() {
                        seedPhraseDialog.setModeView();
                        seedPhraseDialog.open();
                    };
                    confirmPasswordDialog.open();
                }
            }

            LinkButton {
                //% "Show wallet addresses"
                text:      qsTrId("settings-swap-show-addresses")
                onClicked: {                        
                    showAddressesDialog.addresses = getEthereumAddresses();
                    showAddressesDialog.open();
                }
            }
        }

        // buttons
        // "cancel" "apply"
        // "connect to node" or "connect to electrum"
        RowLayout {
            visible:                control.canChangeConnection
            Layout.preferredHeight: 52
            Layout.fillWidth:       true
            Layout.topMargin:       30
            spacing:                15

            Item {
                Layout.fillWidth: true
            }

            CustomButton {
                visible:                applySettingsButtonId.visible
                Layout.preferredHeight: 38
                Layout.preferredWidth:  130
                leftPadding:  25
                rightPadding: 25
                text:         qsTrId("general-cancel")
                icon.source:  enabled ? "qrc:/assets/icon-cancel-white.svg" : "qrc:/assets/icon-cancel.svg"
                enabled:      isSettingsChanged()
                onClicked:    restoreSettings()
            }

            CustomButton {
                id:                     disconnectButtonId
                visible:                canDisconnect()
                Layout.preferredHeight: 38
                Layout.preferredWidth:  164
                palette.button:         Style.swapDisconnectNode
                palette.buttonText:     Style.content_opposite
                //% "disconnect"
                text:                   qsTrId("settings-swap-disconnect")
                icon.source:            "qrc:/assets/icon-delete-blue.svg"
                onClicked:              disconnect()
            }

            CustomButton {
                visible:                connectButtonId.visible
                Layout.preferredHeight: 38
                Layout.preferredWidth:  124
                leftPadding:            25
                rightPadding:           25
                //% "clear"
                text:                   qsTrId("settings-swap-clear")
                icon.source:            "qrc:/assets/icon-delete.svg"
                onClicked:  {
                    //: electrum settings, ask password to clear seed phrase, dialog title
                    //% "Clear seed phrase"
                    confirmPasswordDialog.dialogTitle = qsTrId("settings-swap-confirm-clear-seed-title");
                    //: electrum settings, ask password to clear seed phrase, dialog message
                    //% "Enter your wallet password to clear seed phrase"
                    confirmPasswordDialog.dialogMessage = qsTrId("settings-swap-confirm-clear-seed-message");
                    confirmPasswordDialog.onDialogAccepted = function() {
                        clear();
                    };
                    confirmPasswordDialog.open();
                }
            }

            PrimaryButton {
                id:                     applySettingsButtonId
                visible:                !connectButtonId.visible && !disconnectButtonId.visible
                leftPadding:            25
                rightPadding:           25
                text:                   qsTrId("settings-apply")
                icon.source:            "qrc:/assets/icon-done.svg"
                enabled:                isSettingsChanged() && canApplySettings()
                onClicked:              applyChanges()
                Layout.preferredHeight: 38
            }

            PrimaryButton {
                id:                     connectButtonId
                visible:                canConnect()
                leftPadding:            25
                rightPadding:           25
                //% "connect to node"
                text:                   qsTrId("settings-swap-connect-to-node")
                icon.source:            "qrc:/assets/icon-done.svg"
                onClicked:              connectToNode();
                Layout.preferredHeight: 38
                Layout.preferredWidth:  /*editElectrum ? 253 : */193
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}