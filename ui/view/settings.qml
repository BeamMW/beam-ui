import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "controls"
import "utils.js" as Utils
import Beam.Wallet 1.0

ColumnLayout {
    id: settingsView
    Layout.fillWidth:   true
    spacing:            0
    state:              "general"

    property string  linkStyle: "<style>a:link {color: '#00f6d2'; text-decoration: none;}</style>"
    property string  unfoldSection:   ""
    property bool    creating: true

    property bool settingsPrivacyFolded: true

    Component.onCompleted: {
        settingsView.creating = false
    }

    SettingsViewModel {
        id: viewModel
    }

    Title {
        //% "Settings"
        text: qsTrId("settings-title")

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter | Qt.AlignRight
            SFText {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                //% "Blockchain height"
                text: qsTrId("settings-blockchain-height")
                color: Style.content_secondary
                font.pixelSize: 14
            }

            SFLabel {
                horizontalAlignment: Text.AlignRight
                color: Style.content_main
                text: viewModel.currentHeight
                font.pixelSize: 14
                font.styleName:      "Bold"
                font.weight:         Font.Bold
            }

            SFText {
                horizontalAlignment: Text.AlignRight
                Layout.leftMargin: 20
                font.pixelSize: 14
                color: Style.content_secondary
                //: settings tab, version label
                //% "v"
                text: qsTrId("settings-version") + " " + viewModel.version
            }
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
    //
    // Subtitle row
    //
    SubtitleRow {
    }
    ScrollView {
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
                    folded: settingsPrivacyFolded
                }

                SettingsApps {
                    id: appsBlock
                    viewModel: viewModel
                }

                SettingsCA {
                    id: caBlock
                    viewModel: viewModel
                    folded: unfoldSection != "CA"
                }

                SettingsTitle {
                    topPadding: 30
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

                SettingsBeamRemoteNode {
                    id: remoteNodeBlock
                    viewModel: viewModel

                    showStatus: true
                    connectionStatus: getStatus()
                    connectionError:  main.statusBar.walletError
                    folded: unfoldSection != "BEAM_NODE"

                    function getStatus() {
                        var sbar = main.statusBar.model
                        if (sbar.isFailedStatus) return "error"
                        else if (sbar.isSyncInProgress || sbar.isOnline) return "connected"
                        else return "disconnected";
                    }
                }

                SettingsBeamNode {
                    id: nodeBlock
                    viewModel: viewModel

                    showStatus: true
                    connectionStatus: getStatus()
                    connectionError:  main.statusBar.localNodeError
                    folded: unfoldSection != "BEAM_NODE"
                    syncProgress: main.statusBar.nodeSyncProgress
                    function getStatus() {
                        var sbar = main.statusBar.model
                        if (sbar.isFailedLocalNode) return "error"
                        else if (sbar.nodeSyncProgress > 0) return "connected"
                        else return "disconnected";
                    }
                }

                SettingsIPFS {
                    id: ipfsBlock
                    viewModel: viewModel
                    visible: viewModel.ipfsSupported
                    folded: unfoldSection != "IPFS_NODE"

                    showStatus: true
                    connectionStatus: main.statusBar.model.ipfsStatus
                    connectionError: main.statusBar.model.ipfsError
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
                        canChangeConnection:      modelData.canChangeConnection
                        isSupportedElectrum:      modelData.isSupportedElectrum
                        isConnected:              modelData.isConnected
                        isNodeConnection:         modelData.isNodeConnection
                        isElectrumConnection:     modelData.isElectrumConnection
                        connectionStatus:         modelData.connectionStatus
                        connectionError:          modelData.connectionError
                        getAddressesElectrum:     modelData.getAddressesElectrum
                        folded:                   creating ? modelData.folded :
                                                             (unfoldSection == modelData.coinID ? false : (unfoldSection == "ALL_COINS" ? modelData.isConnected : true))


                        mainSettingsViewModel:    viewModel
                        showStatus:               true

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
                            function onCanChangeConnectionChanged () {
                                settingsControl.canChangeConnection = modelData.canChangeConnection
                            }
                            function onConnectionTypeChanged () {
                                settingsControl.isConnected          = modelData.isConnected
                                settingsControl.isNodeConnection     = modelData.isNodeConnection
                                settingsControl.isElectrumConnection = modelData.isElectrumConnection
                                settingsControl.title                = modelData.title
                            }
                            function  onConnectionStatusChanged () {
                                settingsControl.connectionStatus = modelData.connectionStatus
                            }
                            function onConnectionErrorChanged () {
                                settingsControl.connectionError = modelData.connectionError
                            }

                            //
                            // Node
                            //
                            function onNodeAddressChanged () {
                                settingsControl.address = modelData.nodeAddress
                            }

                            function onNodePortChanged () {
                                settingsControl.port = modelData.nodePort
                            }

                            function onNodeUserChanged () {
                                settingsControl.username = modelData.nodeUser
                            }

                            function onNodePassChanged () {
                                settingsControl.password = modelData.nodePass
                            }

                            //
                            // Electrum
                            //
                            function onNodeAddressElectrumChanged () {
                                settingsControl.addressElectrum = modelData.nodeAddressElectrum
                            }
                            function onNodePortElectrumChanged () {
                                settingsControl.portElectrum = modelData.nodePortElectrum
                            }
                            function onSelectServerAutomaticallyChanged () {
                                settingsControl.useRandomElectrumNode = modelData.selectServerAutomatically
                            }
                            function onElectrumSeedPhrasesChanged () {
                                settingsControl.seedPhrasesElectrum = modelData.electrumSeedPhrases
                            }
                            function onIsCurrentSeedValidChanged () {
                                settingsControl.isCurrentElectrumSeedValid = modelData.isCurrentSeedValid
                            }
                            function onIsCurrentSeedSegwitChanged () {
                                settingsControl.isCurrentElectrumSeedSegwitAndValid = modelData.isCurrentSeedSegwit
                            }
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
                    id:                       swapEthSettings
                    title:                    viewModel.ethSettings.title
                    generalTitle:             viewModel.ethSettings.generalTitle
                    showSeedDialogTitle:      viewModel.ethSettings.showSeedDialogTitle
                    showAddressesDialogTitle: viewModel.ethSettings.showAddressesDialogTitle
                    seedPhrases:              viewModel.ethSettings.seedPhrases
                    phrasesSeparator:         viewModel.ethSettings.phrasesSeparator
                    isCurrentSeedValid:       viewModel.ethSettings.isCurrentSeedValid
                    mainSettingsViewModel:    viewModel
                    showStatus:               true
                    getEthereumAddresses:     viewModel.ethSettings.getEthereumAddresses
                    folded:                   creating ? (unfoldSection == viewModel.ethSettings.coinID ? false : (unfoldSection == "ALL_COINS" ? viewModel.ethSettings.isConnected : true)) : viewModel.ethSettings.folded
                    canChangeConnection:      viewModel.ethSettings.canChangeConnection
                    isConnected:              viewModel.ethSettings.isConnected
                    connectionStatus:         viewModel.ethSettings.connectionStatus
                    connectionError:          viewModel.ethSettings.connectionError
                    infuraProjectID:          viewModel.ethSettings.infuraProjectID
                    accountIndex:             viewModel.ethSettings.accountIndex

                    Connections {
                        target: viewModel.ethSettings

                        function onCanChangeConnectionChanged () {
                            swapEthSettings.canChangeConnection = viewModel.ethSettings.canChangeConnection
                        }

                        function onConnectionChanged () {
                            swapEthSettings.isConnected = viewModel.ethSettings.isConnected
                            swapEthSettings.title = viewModel.ethSettings.title
                        }

                        function onConnectionStatusChanged () {
                            swapEthSettings.connectionStatus = viewModel.ethSettings.connectionStatus
                        }

                        function onConnectionErrorChanged () {
                            swapEthSettings.connectionError = viewModel.ethSettings.connectionError
                        }

                        function onInfuraProjectIDChanged () {
                            swapEthSettings.infuraProjectID = viewModel.ethSettings.infuraProjectID
                        }

                        function onAccountIndexChanged () {
                            swapEthSettings.accountIndex = viewModel.ethSettings.accountIndex
                        }

                        function onSeedPhrasesChanged () {
                            swapEthSettings.seedPhrases = viewModel.ethSettings.seedPhrases
                        }

                        function onIsCurrentSeedValidChanged () {
                            swapEthSettings.isCurrentSeedValid   = viewModel.ethSettings.isCurrentSeedValid
                        }
                    }
                    
                    onDisconnect:                viewModel.ethSettings.disconnect()
                    onApplySettings:             viewModel.ethSettings.applySettings()
                    onClearSettings:             viewModel.ethSettings.clearSettings()
                    onConnectToNode:             viewModel.ethSettings.connectToNode()
                    onNewSeedPhrases:            viewModel.ethSettings.newSeedPhrases()
                    onRestoreSeedPhrases:        viewModel.ethSettings.restoreSeedPhrases()
                    onCopySeedPhrases:           viewModel.ethSettings.copySeedPhrases()
                    onValidateCurrentSeedPhrase: viewModel.ethSettings.validateCurrentSeedPhrase()

                    Binding {
                        target:   viewModel.ethSettings
                        property: "folded"
                        value:    swapEthSettings.folded
                    }

                    Binding {
                        target:   viewModel.ethSettings
                        property: "infuraProjectID"
                        value:    swapEthSettings.infuraProjectID
                    }

                    Binding {
                        target:   viewModel.ethSettings
                        property: "accountIndex"
                        value:    swapEthSettings.accountIndex
                    }
                }
            }
        }
    }
}
