import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import "controls"
import "utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
    id: settingsView
    Layout.fillWidth: true
    state: "general"

    property string  linkStyle:  "<style>a:link {color: '#00f6d2'; text-decoration: none;}</style>"
    property string  swapMode:   ""
    property bool    creating:   true

    Component.onCompleted: {
        settingsView.creating = false
    }

    SettingsViewModel {
        id: viewModel
    }

    RowLayout {
        id: mainColumn
        Layout.fillWidth:     true
        Layout.minimumHeight: 40
        Layout.alignment:     Qt.AlignTop

        Title {
            //% "Settings"
            text: qsTrId("settings-title")
        }

        SFText {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter | Qt.AlignRight
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 14
            color: Style.content_secondary
            //: settings tab, version label
            //% "Version"
            text: qsTrId("settings-version") + ": " + viewModel.version
        }
        PrimaryButton {
            Layout.alignment: Qt.AlignCenter
            Layout.leftMargin: 20
            Layout.preferredHeight: 38
            //: settings update wallet button
            //% "update wallet"
            text: qsTrId("settings-update-wallet")
            icon.source: "qrc:/assets/icon-repeat.svg"
            visible: main.hasNewerVersion
            onClicked: Utils.navigateToDownloads()
        }

    }

    StatusBar {
        id: status_bar
        model: statusbarModel
    }

    ScrollView {
        Layout.topMargin:  25
        Layout.fillHeight: true
        Layout.bottomMargin: 10
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy:   ScrollBar.AsNeeded
        clip: true

        RowLayout {
            width: settingsView.width
            spacing: 10

            ColumnLayout {
                Layout.preferredWidth: parent.width / 2 - parent.spacing / 2
                Layout.alignment: Qt.AlignTop
                spacing: 10

                SettingsTitle {
                    //% "Wallet"
                    text:  qsTrId("settings-wallet-title")
                }

                SettingsGeneral {
                    id: generalBlock
                    viewModel: viewModel
                }

                SettingsNotifications {
                    id: notificationsBlock
                    viewModel: viewModel
                }

                SettingsUtilities {
                    id: utilitiesBlock
                    viewModel: viewModel
                }

                SettingsPrivacy {
                    id: privacyBlock
                    viewModel: viewModel
                }

                SettingsTitle {
                    //% "Troubleshooting"
                    text:  qsTrId("settings-troubleshooting-title")
                }

                SettingsResources {
                    id: resourcesBlock
                    viewModel: viewModel
                }

                SettingsReport {
                    id: reportBlock
                    viewModel: viewModel
                }
            }

            ColumnLayout {
                Layout.preferredWidth: parent.width / 2 - parent.spacing / 2
                Layout.alignment: Qt.AlignTop
                spacing: 10

                SettingsTitle {
                    //% "Connectivity"
                    text:  qsTrId("settings-connectivity-title")
                }

                SettingsBeamNode {
                    id: nodeBlock
                    viewModel: viewModel
                    hasStatusIndicatior: true
                    connectionStatus: getStatus()
                    connectionErrorMsg: status_bar.error_msg

                    function getStatus() {
                        var status = status_bar.status;
                        if (status == "error") return "error"
                        if (status == "updating") return "connected";
                        if (status == "online") return "connected";
                        return "disconnected"
                    }
                }

                Repeater {
                    id: swapSettingsList
                    model: viewModel.swapCoinSettingsList
                    SwapNodeSettings {
                        id:                       settingsControl
                        title:                    modelData.title
                        generalTitle:             modelData.generalTitle
                        showSeedDialogTitle:      modelData.showSeedDialogTitle
                        showAddressesDialogTitle: modelData.showAddressesDialogTitle
                        canEdit:                  modelData.canEdit
                        isSupportedElectrum:      modelData.isSupportedElectrum
                        isConnected:              modelData.isConnected
                        isNodeConnection:         modelData.isNodeConnection
                        isElectrumConnection:     modelData.isElectrumConnection
                        connectionStatus:         modelData.connectionStatus
                        connectionErrorMsg:       modelData.connectionErrorMsg
                        getAddressesElectrum:     modelData.getAddressesElectrum
                        folded:                   creating ? (swapMode == modelData.coinID ? false : (swapMode == "ALL" ? modelData.isConnected : true)) : modelData.folded
                        mainSettingsViewModel:    viewModel
                        hasStatusIndicatior:      true

                        //
                        // Node
                        //
                        address:             modelData.nodeAddress
                        port:                modelData.nodePort
                        username:            modelData.nodeUser
                        password:            modelData.nodePass

                        //
                        // Electrum
                        //
                        addressElectrum:                     modelData.nodeAddressElectrum
                        portElectrum:                        modelData.nodePortElectrum
                        useRandomElectrumNode:               modelData.selectServerAutomatically
                        seedPhrasesElectrum:                 modelData.electrumSeedPhrases
                        phrasesSeparatorElectrum:            modelData.phrasesSeparatorElectrum
                        isCurrentElectrumSeedValid:          modelData.isCurrentSeedValid
                        isCurrentElectrumSeedSegwitAndValid: modelData.isCurrentSeedSegwit

                        Connections {
                            target: modelData
                            onCanEditChanged:        settingsControl.canEdit = modelData.canEdit
                            onConnectionTypeChanged: {
                                settingsControl.isConnected          = modelData.isConnected;
                                settingsControl.isNodeConnection     = modelData.isNodeConnection;
                                settingsControl.isElectrumConnection = modelData.isElectrumConnection;
                                settingsControl.title                = modelData.title;
                            }
                            onConnectionStatusChanged: {
                                settingsControl.connectionStatus     = modelData.connectionStatus;
                            }

                            onConnectionErrorMsgChanged: {
                                settingsControl.connectionErrorMsg   = modelData.connectionErrorMsg;
                            }

                            //
                            // Node
                            //
                            onNodeAddressChanged: settingsControl.address  = modelData.nodeAddress
                            onNodePortChanged:    settingsControl.port     = modelData.nodePort
                            onNodeUserChanged:    settingsControl.username = modelData.nodeUser
                            onNodePassChanged:    settingsControl.password = modelData.nodePass
                            //
                            // Electrum
                            //
                            onNodeAddressElectrumChanged: settingsControl.addressElectrum = modelData.nodeAddressElectrum
                            onNodePortElectrumChanged: settingsControl.portElectrum = modelData.nodePortElectrum
                            onSelectServerAutomaticallyChanged: settingsControl.useRandomElectrumNode = modelData.selectServerAutomatically
                            onElectrumSeedPhrasesChanged: settingsControl.seedPhrasesElectrum = modelData.electrumSeedPhrases
                            onIsCurrentSeedValidChanged:  settingsControl.isCurrentElectrumSeedValid = modelData.isCurrentSeedValid
                            onIsCurrentSeedSegwitChanged: settingsControl.isCurrentElectrumSeedSegwitAndValid = modelData.isCurrentSeedSegwit
                        }

                        onApplyNode:                 modelData.applyNodeSettings()
                        onClearNode:                 modelData.resetNodeSettings()
                        onApplyElectrum:             modelData.applyElectrumSettings()
                        onClearElectrum:             modelData.resetElectrumSettings()
                        onNewSeedElectrum:           modelData.newElectrumSeed()
                        onRestoreSeedElectrum:       modelData.restoreSeedElectrum()
                        onDisconnect:                modelData.disconnect()
                        onConnectToNode:             modelData.connectToNode()
                        onConnectToElectrum:         modelData.connectToElectrum()
                        onCopySeedElectrum:          modelData.copySeedElectrum()
                        onValidateCurrentSeedPhrase: modelData.validateCurrentElectrumSeedPhrase()

                        Binding {
                            target:   modelData
                            property: "folded"
                            value:    settingsControl.folded
                        }

                        Binding {
                            target:   modelData
                            property: "nodeAddress"
                            value:    settingsControl.address
                        }

                        Binding {
                            target:   modelData
                            property: "nodePort"
                            value:    settingsControl.port
                        }

                        Binding {
                            target:   modelData
                            property: "nodeUser"
                            value:    settingsControl.username
                        }

                        Binding {
                            target:   modelData
                            property: "nodePass"
                            value:    settingsControl.password
                        }

                        Binding {
                            target:   modelData
                            property: "nodeAddressElectrum"
                            value:    settingsControl.addressElectrum
                        }

                        Binding {
                            target:   modelData
                            property: "nodePortElectrum"
                            value:    settingsControl.portElectrum
                        }

                        Binding {
                            target:   modelData
                            property: "selectServerAutomatically"
                            value:    settingsControl.useRandomElectrumNode
                        }
                    }
                }

                SwapEthSettings {
                    id: swapEthSettings
                    title:                    viewModel.ethSettings.title
                    generalTitle:             viewModel.ethSettings.generalTitle
                    showSeedDialogTitle:      viewModel.ethSettings.showSeedDialogTitle
                    seedPhrases:              viewModel.ethSettings.seedPhrases
                    phrasesSeparator:         viewModel.ethSettings.phrasesSeparator
                    isCurrentSeedValid:       viewModel.ethSettings.isCurrentSeedValid
                    mainSettingsViewModel:    viewModel

                    canEdit:                  viewModel.ethSettings.canEdit
                    isConnected:              viewModel.ethSettings.isConnected

                    address:             viewModel.ethSettings.nodeAddress
                    port:                viewModel.ethSettings.nodePort
                    accountIndex:        viewModel.ethSettings.accountIndex
                    contractAddress:     viewModel.ethSettings.contractAddress

                    Connections {
                        target: viewModel.ethSettings
                        onCanEditChanged:        swapEthSettings.canEdit = viewModel.ethSettings.canEdit
                        onConnectionChanged: {
                            swapEthSettings.isConnected          = viewModel.ethSettings.isConnected;
                            swapEthSettings.title                = viewModel.ethSettings.title;
                        }
                        /*onConnectionStatusChanged: {
                            swapEthSettings.connectionStatus     = modelData.connectionStatus;
                        }

                        onConnectionErrorMsgChanged: {
                            swapEthSettings.connectionErrorMsg   = modelData.connectionErrorMsg;
                        }*/

                        onNodeAddressChanged: swapEthSettings.address  = viewModel.ethSettings.nodeAddress
                        onNodePortChanged:    swapEthSettings.port     = viewModel.ethSettings.nodePort
                        onAccountIndexChanged: swapEthSettings.accountIndex = viewModel.ethSettings.accountIndex
                        onContractAddressChanged: swapEthSettings.contractAddress = viewModel.ethSettings.contractAddress
                        onSeedPhrasesChanged: swapEthSettings.seedPhrasesElectrum = viewModel.ethSettings.electrumSeedPhrases
                        onIsCurrentSeedValidChanged:  swapEthSettings.isCurrentSeedValid = viewModel.ethSettings.isCurrentSeedValid
                    }
                    
                    onDisconnect: viewModel.ethSettings.disconnect()
                    onApplySettings: viewModel.ethSettings.applySettings()
                    onClearSettings: viewModel.ethSettings.clearSettings()
                    onConnectToNode: viewModel.ethSettings.connectToNode()
                    onNewSeedPhrases: viewModel.ethSettings.newSeedPhrases()
                    onRestoreSeedPhrases: viewModel.ethSettings.restoreSeedPhrases()
                    onCopySeedPhrases: viewModel.ethSettings.copySeedPhrases()
                    onValidateCurrentSeedPhrase: viewModel.ethSettings.validateCurrentSeedPhrase()
                }
            }
        }
    }
}
