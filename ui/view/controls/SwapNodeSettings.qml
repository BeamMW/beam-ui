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
    //
    // Common props
    //
    property string generalTitle:             ""
    property alias  showSeedDialogTitle:      seedPhraseDialog.showSeedDialogTitle
    property alias  showAddressesDialogTitle: showAddressesDialog.showAddressesDialogTitle
    property string color:               Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
    property string disabledColor:       Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.2)
    property alias  editElectrum:        useElectrumSwitch.checked
    property bool   canChangeConnection: true
    property bool   isSupportedElectrum: true

    property bool   isConnected:            false
    property bool   isNodeConnection:       false
    property bool   isElectrumConnection:   false
    property var    mainSettingsViewModel:  undefined

    //
    // Node props
    //
    property alias  address:      addressInput.address
    property alias  port:         portInput.text
    property alias  username:     usernameInput.text
    property alias  password:     passwordInput.text
    property bool   canEditNode:  !control.isNodeConnection

    //
    // Electrum props
    //
    property alias addressElectrum:                     addressInputElectrum.address
    property alias portElectrum:                        portInputElectrum.text
    property alias useRandomElectrumNode:               useRandomNode.checked
    property alias seedPhrasesElectrum:                 seedPhraseDialog.seedPhrasesElectrum
    property alias phrasesSeparatorElectrum:            seedPhraseDialog.phrasesSeparatorElectrum
    property bool  isCurrentElectrumSeedValid:          false
    property bool  isCurrentElectrumSeedSegwitAndValid: false
    property bool  canEditElectrum:                     !control.isElectrumConnection

    // function to get "receiving" addresses
    property var   getAddressesElectrum:       undefined

    ConfirmPasswordDialog {
        id: confirmPasswordDialog
        settingsViewModel: mainSettingsViewModel 
    }

    //
    // signals
    //
    signal disconnect
    // node
    signal applyNode
    signal clearNode
    signal connectToNode
    // electrum
    signal newSeedElectrum
    signal restoreSeedElectrum
    signal applyElectrum
    signal clearElectrum
    signal connectToElectrum
    signal copySeedElectrum
    signal validateCurrentSeedPhrase

    QtObject {
        id: internalNode
        property string initialAddress
        property string initialPort
        property string initialUsername
        property string initialPassword

        function restore() {
            address  = initialAddress
            port     = initialPort
            username = initialUsername
            password = initialPassword
        }

        function save() {
            initialAddress  = address
            initialPort     = port
            initialUsername = username
            initialPassword = password
        }

        function isChanged() {
            return initialAddress  !== address
                || initialPort     !== port
                || initialUsername !== username
                || initialPassword !== password
        }
    }

    function isSettingsChanged() {
        return editElectrum ? internalElectrum.isChanged() : internalNode.isChanged();
    }

    function canApplySettings() {
        return editElectrum ? canApplyElectrum() : canApplyNode();
    }

    function applyChanges() {
        return editElectrum ? applyChangesElectrum() : applyChangesNode();
    }

    function restoreSettings() {
        return editElectrum ? internalElectrum.restore() : internalNode.restore();
    }

    function haveSettings() {
        return editElectrum ? haveElectrumSettings() : haveNodeSettings();
    }

    function clear() {
        if (editElectrum) {
            control.clearElectrum();
            internalElectrum.save();
        }
        else {
            control.clearNode();
            internalNode.save();
        }
    }

    function canClear() {
        return control.canChangeConnection && (editElectrum ? canClearElectrum() : canClearNode());
    }

    function canConnect() {
        return !isSettingsChanged() && haveSettings() && (editElectrum ? !isElectrumConnection : !isNodeConnection);
    }

    function canDisconnect() {
        return isConnected && (editElectrum ? canDisconnectElectrum() : canDisconnectNode() && control.canChangeConnection);
    }

    function canApplyNode() {
        return password.length && username.length && addressInput.isValid && portInput.acceptableInput
    }

    function applyChangesNode() {
        internalNode.save()
        control.applyNode()
    }

    function canClearNode() {
        return !isNodeConnection && (internalNode.initialPassword.length || internalNode.initialUsername.length || internalNode.initialAddress.length);
    }

    function canDisconnectNode() {
        return isNodeConnection;
    }

    function haveNodeSettings() {
        return password.length && username.length && addressInput.isValid && portInput.acceptableInput;
    }

    //
    // Electrum props
    //

    QtObject {
        id: internalElectrum
        property string initialAddress
        property string initialPort
        property bool   initialRandomNode
        property string initialSeed
        property bool   isSeedChanged: false

        function restore() {
            isSeedChanged = false
            addressElectrum = initialAddress
            portElectrum = initialPort
            useRandomElectrumNode = initialRandomNode
            control.restoreSeedElectrum()
        }

        function save() {
            if (!useRandomElectrumNode) {
                initialAddress = addressElectrum
                initialPort    = portElectrum
            }
            initialRandomNode = useRandomElectrumNode
            isSeedChanged = false
        }

        function isChanged() {
            return (!useRandomElectrumNode && (initialAddress !== addressElectrum || initialPort !== portElectrum )) || isSeedChanged || 
                    useRandomElectrumNode !== initialRandomNode
        }
    }

    function canApplyElectrum() {
        return isCurrentElectrumSeedValid && ((addressInputElectrum.isValid && portInputElectrum.acceptableInput) || useRandomElectrumNode)
    }

    function canClearElectrum() {
        return !isElectrumConnection && (internalElectrum.initialAddress.length || isCurrentElectrumSeedValid);
    }

    function canDisconnectElectrum() {
        return isElectrumConnection;
    }

    function applyChangesElectrum() {
        internalElectrum.save();
        control.applyElectrum();
    }

    function haveElectrumSettings() {
        return isCurrentElectrumSeedValid && ((addressInputElectrum.isValid && portInputElectrum.acceptableInput) || useRandomElectrumNode);
    }

    Component.onCompleted: {
        control.editElectrum = control.isElectrumConnection || (!control.canChangeConnection && !control.isNodeConnection);
        internalNode.save();
        internalElectrum.save();
    }

    content: ColumnLayout {
        spacing: 0

        // Node & Electrum switch
        RowLayout {
            height:   20
            spacing:  10
            visible:  isSupportedElectrum
            Layout.fillWidth:   true
            SFText {
                //% "Node"
                text:  qsTrId("settings-swap-node")
                color: useElectrumSwitch.checked ? useElectrumSwitch.enabled ? control.color : control.disabledColor : Style.active
                font.pixelSize: 14
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        if (useElectrumSwitch.enabled)
                            useElectrumSwitch.checked = !useElectrumSwitch.checked;
                    }
                }
            }

            CustomSwitch {
                id:          useElectrumSwitch
                alwaysGreen: true
                spacing:     0
                enabled:     !(isConnected || !isConnected && !control.canChangeConnection)
            }

            SFText {
                //% "Electrum"
                text: qsTrId("general-electrum")
                color: useElectrumSwitch.checked ? Style.active : useElectrumSwitch.enabled ? control.color : control.disabledColor
                font.pixelSize: 14
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        if (useElectrumSwitch.enabled)
                            useElectrumSwitch.checked = !useElectrumSwitch.checked;
                    }
                }
            }

            SFText {
                //% "Specific node"
                text:  qsTrId("specific_node")
                color: useRandomNode.checked ? useRandomNode.enabled ? control.color : control.disabledColor : Style.active
                font.pixelSize: 14
                visible: useElectrumSwitch.checked
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        if (useRandomNode.enabled)
                            useRandomNode.checked = !useRandomNode.checked;
                    }
                }
            }

            CustomSwitch {
                id:          useRandomNode
                alwaysGreen: true
                spacing:     0
                visible:     useElectrumSwitch.checked
                enabled:     !isConnected
            }

            SFText {
                //% "Random node"
                text: qsTrId("random_node")
                color: useRandomNode.checked ? Style.active : useRandomNode.enabled ? control.color : control.disabledColor
                font.pixelSize: 14
                visible: useElectrumSwitch.checked
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        if (useRandomNode.enabled)
                            useRandomNode.checked = !useRandomNode.checked;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        GridLayout {
            visible: !(editElectrum && useRandomNode.checked && !isElectrumConnection)
            Layout.topMargin: isSupportedElectrum ? 20 : 0
            columns:          2
            columnSpacing:    50
            rowSpacing:       13

            SFText {
                visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Node address"
                text:           qsTrId("settings-node-address")
            }

            IPAddrInput {
                id:               addressInput
                visible:          !editElectrum
                Layout.fillWidth: true
                color:            Style.content_main
                underlineVisible: canEditNode
                readOnly:         !canEditNode
                ipOnly:           false
            }

            SFText {
                visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                text:           qsTrId("settings-local-node-port")
            }

            SFTextInput {
                id:                 portInput
                visible:            !editElectrum
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                underlineVisible:   canEditNode
                readOnly:           !canEditNode
                validator: IntValidator {
                    bottom: 1
                    top: 65535
                }
            }

            SFText {
                visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Username"
                text:           qsTrId("settings-username")
            }

            SFTextInput {
                id:               usernameInput
                visible:          !editElectrum
                Layout.fillWidth: true
                font.pixelSize:   14
                color:            Style.content_main
                activeFocusOnTab: true
                underlineVisible: canEditNode
                readOnly:         !canEditNode
            }

            SFText {
                visible:        !editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Password"
                text:           qsTrId("settings-password")
            }

            SFTextInput {
                id:               passwordInput
                visible:          !editElectrum
                Layout.fillWidth: true
                font.pixelSize:   14
                color:            Style.content_main
                activeFocusOnTab: true
                echoMode:         TextInput.Password
                underlineVisible: canEditNode
                readOnly:         !canEditNode
            }

            // electrum settings
            SFText {
                visible:        editElectrum
                font.pixelSize: 14
                color:          control.color
                //% "Node address"
                text:           qsTrId("settings-node-address")
            }

            IPAddrInput {
                visible:          editElectrum
                id:               addressInputElectrum
                Layout.fillWidth: true
                color:            Style.content_main
                ipOnly:           false
                readOnly:         useRandomNode.checked
                underlineVisible: !useRandomNode.checked
            }

            SFText {
                visible:        editElectrum
                font.pixelSize: 14
                color:          control.color
                text:           qsTrId("settings-local-node-port")
            }

            SFTextInput {
                id:                 portInputElectrum
                visible:            editElectrum
                Layout.fillWidth:   true
                font.pixelSize:     14
                activeFocusOnTab:   true
                color:              Style.content_main
                readOnly:           useRandomNode.checked
                underlineVisible:   !useRandomNode.checked
                validator:          RegExpValidator {regExp: /^([1-9][0-9]{0,3}|[1-5][0-9]{2,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/g}
            }
        }

        SFText {
            visible:               editElectrum && useRandomNode.checked && !disconnectButtonId.visible
            Layout.topMargin:      30
            Layout.preferredWidth: 390
            Layout.alignment:      Qt.AlignVCenter | Qt.AlignHCenter
            horizontalAlignment:   Text.AlignHCenter
            verticalAlignment:     Text.AlignVCenter
            font.pixelSize:        14
            wrapMode:              Text.WordWrap
            color:                 control.color
            lineHeight:            1.1 
/*% "Random node address and port will be displayed 
when connection is established"
*/
            text:                  qsTrId("settings-random-node-text")
        }

        // electrum settings - seed: new || edit
        RowLayout {
            visible:             editElectrum && canEditElectrum
            spacing:             20
            Layout.fillWidth:    true
            Layout.topMargin:    30
            Layout.bottomMargin: 7

            LinkButton {
                Layout.fillWidth:   true
                text:      isCurrentElectrumSeedValid ?   
                            //% "Edit your seed phrase"
                            qsTrId("settings-swap-edit-seed") :
                            //% "Enter your seed phrase"
                            qsTrId("settings-swap-enter-seed")
                onClicked: {
                    function editSeedPhrase() {
                        seedPhraseDialog.setModeEdit();
                        seedPhraseDialog.isCurrentElectrumSeedValid = Qt.binding(function(){return isCurrentElectrumSeedValid;});
                        seedPhraseDialog.isCurrentElectrumSeedSegwitAndValid = Qt.binding(function(){return isCurrentElectrumSeedSegwitAndValid;});
                        seedPhraseDialog.open();
                    }
                    if (isCurrentElectrumSeedValid) {
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
                Layout.fillWidth:   true
                //% "Generate new seed phrase"
                text:             qsTrId("settings-swap-new-seed")
                onClicked: {
                    function generateSeedPhrase() {
                        newSeedElectrum();
                        seedPhraseDialog.setModeNew();
                        seedPhraseDialog.open();
                    }

                    if (isCurrentElectrumSeedValid) {
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

        // electrum settings: show seed && show addresses
        RowLayout {
            visible:             editElectrum && !canEditElectrum
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
                    showAddressesDialog.addresses = getAddressesElectrum();
                    showAddressesDialog.open();
                }
            }
        }

        // alert text if we have active transactions
        SFText {
            visible:               !control.canChangeConnection && !editElectrum
            Layout.topMargin:      30
            Layout.preferredWidth: 390
            Layout.alignment:      Qt.AlignVCenter | Qt.AlignHCenter
            horizontalAlignment:   Text.AlignHCenter
            verticalAlignment:     Text.AlignVCenter
            font.pixelSize:        14
            wrapMode:              Text.WordWrap
            color:                 control.color
            lineHeight:            1.1
            text:                  isSupportedElectrum ?
                                       //% "Swap in progress, cannot disconnect or switch connection type."
                                       qsTrId("settings-node-progress")
                                       :
                                       //% "Swap in progress, cannot disconnect."
                                       qsTrId("settings-doge-node-progress")
        }

        SFText {
            visible:               !control.canChangeConnection && editElectrum && !disconnectButtonId.visible
            //visible: false
            Layout.topMargin:      30
            Layout.preferredWidth: 390
            Layout.alignment:      Qt.AlignVCenter | Qt.AlignHCenter
            horizontalAlignment:   Text.AlignHCenter
            verticalAlignment:     Text.AlignVCenter
            font.pixelSize:        14
            wrapMode:              Text.WordWrap
            color:                 control.color
            lineHeight:            1.1 
            text:                  qsTrId("settings-node-progress")
        }

        // buttons
        // "cancel" "apply"
        // "connect to node" or "connect to electrum"
        RowLayout {
            visible:                control.canChangeConnection || editElectrum
            Layout.preferredHeight: 52
            Layout.alignment:       Qt.AlignHCenter
            Layout.topMargin:       30
            spacing:                20

            CustomButton {
                leftPadding:            25
                rightPadding:           25
                visible:                !canConnect() && !canDisconnect()
                text:                   qsTrId("general-cancel")
                icon.source:            enabled ? "qrc:/assets/icon-cancel-white.svg" : "qrc:/assets/icon-cancel.svg"
                enabled:                isSettingsChanged()
                onClicked:              restoreSettings()
            }

            CustomButton {
                id:                     disconnectButtonId
                leftPadding:            25
                rightPadding:           25
                visible:                canDisconnect()
                palette.button:         Style.swapDisconnectNode
                palette.buttonText:     Style.content_opposite
                //% "disconnect"
                text:                   qsTrId("settings-swap-disconnect")
                icon.source:            "qrc:/assets/icon-delete-blue.svg"
                onClicked:              disconnect()
            }

            CustomButton {
                leftPadding:            25
                rightPadding:           25
                visible:                canConnect()
                //% "clear"
                text:                   qsTrId("settings-swap-clear")
                icon.source:            "qrc:/assets/icon-delete.svg"
                onClicked:  {
                    if (editElectrum) {
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
                    } else {
                        clear();
                    }
                }
            }

            PrimaryButton {
                id:                     applySettingsButtonId
                leftPadding:            25
                rightPadding:           25
                // TODO roman.strilets need to refactor this
                visible:                !canConnect() && !canDisconnect()
                text:                   qsTrId("settings-apply")
                icon.source:            "qrc:/assets/icon-done.svg"
                enabled:                isSettingsChanged() && canApplySettings()
                onClicked:              applyChanges()
            }

            PrimaryButton {
                id:                     connectButtonId
                leftPadding:            25
                rightPadding:           25
                visible:                canConnect()
                text:                   editElectrum
                                            //% "connect to electrum node"
                                            ? qsTrId("settings-swap-connect-to-electrum")
                                            //% "connect to node"
                                            : qsTrId("settings-swap-connect-to-node")
                icon.source:            "qrc:/assets/icon-done.svg"
                onClicked:              editElectrum ? connectToElectrum() : connectToNode();
            }
        }
    }

    SeedPhraseDialog {
        id: seedPhraseDialog;

        onNewSeedElectrum: control.newSeedElectrum()
        onCopySeedElectrum: control.copySeedElectrum()
        onValidateFullSeedPhrase: control.validateCurrentSeedPhrase()
        onClosed: {
            internalElectrum.isSeedChanged = seedPhraseDialog.isSeedChanged
        }
    }

    ShowAddressesDialog {
        id: showAddressesDialog
    }
}