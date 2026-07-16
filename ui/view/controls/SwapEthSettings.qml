import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

SettingsFoldable {
    id:            control

    property string generalTitle:             ""
    property alias  showSeedDialogTitle:      seedPhraseDialog.showSeedDialogTitle
    property alias  showAddressesDialogTitle: showAddressesDialog.showAddressesDialogTitle
    property string color:                    Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
    property string disabledColor:            Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.2)

    property bool   isConnected:           false
    property var    mainSettingsViewModel: undefined
    property bool   canChangeConnection:             true
    property bool   canEdit:  !control.isConnected

    property alias  infuraProjectID:      infuraProjectIDInput.text
    property alias  accountIndex:         accountIndexInput.text
    property alias  useCustomRpc:         useCustomRpcSwitch.checked
    property alias  customRpcUrl:         customRpcUrlInput.text

    property alias seedPhrases:        seedPhraseDialog.seedPhrasesElectrum
    property alias phrasesSeparator:   seedPhraseDialog.phrasesSeparatorElectrum
    property bool  isCurrentSeedValid: false

    property string endpointCheckResult: ""
    property bool   endpointCheckOk:     false

    // backing SwapEthSettingsItem, used directly for the custom-token flow
    // rather than mirroring every field through property aliases + Connections
    property var    ethSettings: undefined
    property string newTokenSymbol:   ""
    property int    newTokenDecimals: 0
    property string newTokenError:    ""

    function resetTokenLookup() {
        newTokenSymbol   = ""
        newTokenDecimals = 0
        newTokenError    = ""
    }
    onUseCustomRpcChanged:    resetTokenLookup()
    onCustomRpcUrlChanged:    resetTokenLookup()
    onInfuraProjectIDChanged: resetTokenLookup()
    // collapsing the section should not leave the token input focused/open underneath
    onFoldedChanged: if (control.folded) addTokenPane.hide()

    Connections {
        target: ethSettings
        function onTokenInfoReady(contract, symbol, decimals, error) {
            if (contract !== newTokenAddress.text.trim()) return
            newTokenSymbol   = symbol
            newTokenDecimals = decimals
            newTokenError    = error
        }
    }

    // function to get ethereum addresses
    property var   getEthereumAddresses:       undefined

    ConfirmPasswordDialog {
        id: confirmPasswordDialog
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
    signal validateEndpoint

    QtObject {
        id: internalValues
        property string initialInfuraProjectID
        property string initialSeed
        property string initialAccountIndex
        property bool   initialUseCustomRpc
        property string initialCustomRpcUrl

        property bool   isSeedChanged: false

        function restore() {
            isSeedChanged = false
            infuraProjectID = initialInfuraProjectID
            control.restoreSeedPhrases()

            accountIndex = initialAccountIndex
            useCustomRpc = initialUseCustomRpc
            customRpcUrl = initialCustomRpcUrl
        }

        function save() {
            initialInfuraProjectID = infuraProjectID
            isSeedChanged = false
            initialAccountIndex = accountIndex
            initialUseCustomRpc = useCustomRpc
            initialCustomRpcUrl = customRpcUrl
        }

        function isChanged() {
            return initialInfuraProjectID !== infuraProjectID || isSeedChanged || initialAccountIndex != accountIndex ||
                   initialUseCustomRpc !== useCustomRpc || initialCustomRpcUrl !== customRpcUrl
        }
    }

    function isSettingsChanged() {
        return internalValues.isChanged();
    }

    function canApplySettings() {
        return isCurrentSeedValid;
    }

    function applyChanges() {
        internalValues.save();
        control.applySettings();
    }

    function restoreSettings() {
        internalValues.restore();
    }

    function haveSettings() {
        return isCurrentSeedValid;
    }

    function clear() {
        control.clearSettings();
        internalValues.save();
    }

    function canClear() {
        return !isConnected && (internalValues.initialInfuraProjectID.length || isCurrentSeedValid);
    }

    function canConnect() {
        return !isSettingsChanged() && haveSettings() && !isConnected;
    }

    function canDisconnect() {
        return isConnected && control.canChangeConnection;
    }

    Component.onCompleted: {
        internalValues.save();
    }

    content: ColumnLayout {
        spacing: 0

        // Infura & Custom RPC switch
        RowLayout {
            height:   20
            spacing:  10
            Layout.fillWidth: true
            SFText {
                //% "Infura"
                text:  qsTrId("settings-eth-infura")
                color: useCustomRpcSwitch.checked ? control.color : Style.active
                font.pixelSize: 14
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: useCustomRpcSwitch.checked = !useCustomRpcSwitch.checked
                }
            }
            CustomSwitch {
                id:          useCustomRpcSwitch
                alwaysGreen: true
                spacing:     0
                enabled:     canEdit
            }
            SFText {
                //% "Custom RPC"
                text: qsTrId("settings-eth-custom-rpc")
                color: useCustomRpcSwitch.checked ? Style.active : control.color
                font.pixelSize: 14
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: useCustomRpcSwitch.checked = !useCustomRpcSwitch.checked
                }
            }
        }

        GridLayout {
            Layout.topMargin: 20
            columns:          2
            columnSpacing:    50
            rowSpacing:       13

            SFText {
                visible:        !useCustomRpcSwitch.checked
                font.pixelSize: 14
                color:          control.color
                //% "Infura project ID"
                text:           qsTrId("settings-infura-project-id")
            }

            SFTextInput {
                id:               infuraProjectIDInput
                visible:          !useCustomRpcSwitch.checked
                Layout.fillWidth: true
                color:            Style.content_main
                font.pixelSize:     14
                activeFocusOnTab:   true
                underlineVisible: canEdit
                readOnly:         !canEdit
            }

            SFText {
                font.pixelSize: 14
                color:          control.color
                //% "Account index"
                text:           qsTrId("settings-account-index")
            }

            SFTextInput {
                id:                 accountIndexInput
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
        }

        SFText {
            visible:        useCustomRpcSwitch.checked
            Layout.topMargin: 20
            font.pixelSize: 14
            color:          control.color
            //% "Ethereum RPC endpoint"
            text:           qsTrId("settings-eth-rpc-endpoint")
        }

        SFTextInput {
            id:               customRpcUrlInput
            visible:          useCustomRpcSwitch.checked
            Layout.fillWidth: true
            color:            Style.content_main
            font.pixelSize:   14
            activeFocusOnTab: true
            underlineVisible: canEdit
            readOnly:         !canEdit
            placeholderText:  "https://mainnet.infura.io/v3/..."
        }

        SFText {
            visible:        useCustomRpcSwitch.checked
            Layout.fillWidth: true
            Layout.topMargin: 7
            font.pixelSize: 12
            color:          Style.content_secondary
            wrapMode:       Text.Wrap
            //% "Works with any Ethereum JSON-RPC endpoint. Keyless public options: ethereum-rpc.publicnode.com, eth.drpc.org, rpc.mevblocker.io - or run your own node."
            text:           qsTrId("settings-eth-rpc-note")
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

        // check connection / cancel / apply / connect / disconnect - all on one centered row
        RowLayout {
            visible:                control.canChangeConnection
            Layout.preferredHeight: 52
            Layout.fillWidth:       true
            Layout.topMargin:       30
            spacing:                10

            Item {
                Layout.fillWidth: true
            }

            CustomButton {
                Layout.preferredHeight: 38
                Layout.preferredWidth:  160
                leftPadding:  20
                rightPadding: 20
                //% "Check connection"
                text:         qsTrId("settings-eth-check-connection")
                // don't allow checking the last-applied endpoint while there are unapplied edits on screen
                enabled:      (isConnected || canApplySettings()) && !isSettingsChanged()
                onClicked:    validateEndpoint()
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
                Layout.preferredWidth:  160
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
                Layout.preferredWidth:  160
            }

            Item {
                Layout.fillWidth: true
            }
        }

        SFText {
            visible:          endpointCheckResult !== ""
            Layout.fillWidth: true
            Layout.topMargin: 10
            font.pixelSize:   14
            wrapMode:         Text.Wrap
            color:            endpointCheckOk ? Style.active : Style.validator_error
            text:             (endpointCheckOk ? "✓ " : "✗ ") + endpointCheckResult
        }

        //
        // Custom ERC-20 tokens (used as swap-offer receive currencies)
        //
        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 30
            spacing: 10
            visible: control.ethSettings !== undefined && control.ethSettings !== null

            SFText {
                font.pixelSize: 14
                color: control.color
                //% "Custom ERC-20 tokens"
                text: qsTrId("settings-swap-token-section-title")
            }

            // built in - always supported natively, can't be removed
            Repeater {
                model: control.ethSettings ? control.ethSettings.builtinTokens : []
                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    SvgImage {
                        visible:    !!modelData.icon
                        source:     modelData.icon ? modelData.icon : ""
                        sourceSize: Qt.size(26, 26)
                    }
                    TokenIcon {
                        visible:    !modelData.icon
                        tokenColor: modelData.color
                        symbol:     modelData.symbol
                    }
                    SFText {
                        Layout.fillWidth: true
                        font.pixelSize: 14
                        color: Style.content_main
                        elide: Text.ElideMiddle
                        text: modelData.symbol + "  " + modelData.contract
                    }
                    SFText {
                        font.pixelSize: 12
                        color: control.color
                        //% "built-in"
                        text: qsTrId("settings-swap-token-builtin")
                    }
                }
            }

            // user-added
            Repeater {
                model: control.ethSettings ? control.ethSettings.customTokens : []
                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    TokenIcon {
                        tokenColor: modelData.color
                        symbol:     modelData.symbol
                    }
                    SFText {
                        Layout.fillWidth: true
                        font.pixelSize: 14
                        color: Style.content_main
                        elide: Text.ElideMiddle
                        text: modelData.symbol + "  " + modelData.contract
                    }
                    LinkButton {
                        //% "remove"
                        text: qsTrId("settings-swap-token-remove")
                        onClicked: control.ethSettings.removeCustomToken(modelData.contract)
                    }
                }
            }

            // "+ Add token" link; clicking it reveals the input row below
            LinkButton {
                visible:   !addTokenPane.visible
                //% "+ Add token"
                text:      qsTrId("settings-swap-token-add-open")
                onClicked: {
                    addTokenPane.visible = true
                    newTokenAddress.forceActiveFocus()
                }
            }

            ColumnLayout {
                id:      addTokenPane
                visible: false
                Layout.fillWidth: true
                spacing: 10

                function hide() {
                    addTokenPane.visible = false
                    newTokenAddress.focus = false
                    newTokenAddress.text = ""
                    control.resetTokenLookup()
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    SFTextInput {
                        id: newTokenAddress
                        Layout.fillWidth: true
                        font.pixelSize: 14
                        color: Style.content_main
                        underlineVisible: true
                        //% "0x contract address"
                        placeholderText: qsTrId("settings-swap-token-address-placeholder")
                        onTextChanged: control.resetTokenLookup()
                    }

                    LinkButton {
                        //% "cancel"
                        text: qsTrId("settings-swap-token-add-cancel")
                        onClicked: addTokenPane.hide()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 15

                    CustomButton {
                        Layout.preferredHeight: 38
                        leftPadding:  25
                        rightPadding: 25
                        //% "Look up"
                        text: qsTrId("settings-swap-token-lookup")
                        // the lookup queries the last-applied endpoint
                        enabled: newTokenAddress.text.trim().length > 0 && !isSettingsChanged()
                        onClicked: control.ethSettings.lookupToken(newTokenAddress.text.trim())
                    }

                    CustomButton {
                        Layout.preferredHeight: 38
                        leftPadding:  25
                        rightPadding: 25
                        //% "Add"
                        text: qsTrId("settings-swap-token-add")
                        enabled: newTokenSymbol.length > 0
                        onClicked: {
                            control.ethSettings.addCustomToken(newTokenAddress.text.trim(), newTokenSymbol, newTokenDecimals)
                            // the rejection error is emitted synchronously
                            if (newTokenError.length === 0)
                                addTokenPane.hide()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                SFText {
                    visible: newTokenSymbol.length > 0 || newTokenError.length > 0
                    Layout.fillWidth: true
                    font.pixelSize: 12
                    wrapMode: Text.Wrap
                    color: newTokenError.length > 0 ? Style.validator_error : Style.content_secondary
                    text: newTokenError.length > 0 ? newTokenError :
                        //% "%1, %2 decimals"
                        qsTrId("settings-swap-token-info").arg(newTokenSymbol).arg(newTokenDecimals)
                }
            }
        }
    }
}