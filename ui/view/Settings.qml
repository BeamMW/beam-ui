import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
    property string  searchText: ""
    readonly property bool searchActive: searchText.trim().length > 0

    property bool settingsPrivacyFolded: true

    Component.onCompleted: {
        settingsView.creating = false
    }

    SettingsViewModel {
        id: viewModel
    }

    function searchTerms() {
        var query = searchText.trim().toLowerCase()
        if (!query.length) return []
        var words = query.split(/\s+/)
        return words.indexOf(query) === -1 ? [query].concat(words) : words
    }

    function itemMatches(item, terms) {
        if (!item || !terms.length) return false

        var values = []
        if (item.title !== undefined) values.push(item.title)
        if (item.text !== undefined) values.push(item.text)
        if (item.displayText !== undefined) values.push(item.displayText)
        if (item.currentText !== undefined) values.push(item.currentText)

        for (var i = 0; i < values.length; ++i) {
            var value = String(values[i]).toLowerCase()
            for (var j = 0; j < terms.length; ++j) {
                if (value.indexOf(terms[j]) !== -1) return true
            }
        }

        if (!item.children) return false
        for (var k = 0; k < item.children.length; ++k) {
            if (itemMatches(item.children[k], terms)) return true
        }
        return false
    }

    function blockMatches(block) {
        return !searchActive || itemMatches(block, searchTerms())
    }

    function hasSearchResults() {
        if (!searchActive) return true

        var blocks = [
            generalBlock, notificationsBlock, utilitiesBlock, privacyBlock,
            appsBlock, caBlock, resourcesBlock, reportBlock, remoteNodeBlock,
            nodeBlock, ipfsBlock, swapEthSettings
        ]

        for (var i = 0; i < blocks.length; ++i) {
            if (blockMatches(blocks[i])) return true
        }

        for (var j = 0; j < swapSettingsList.count; ++j) {
            if (blockMatches(swapSettingsList.itemAt(j))) return true
        }

        return false
    }

    function applySearchToBlock(block, defaultFolded) {
        if (searchActive) {
            block.folded = !blockMatches(block)
        } else if (defaultFolded !== undefined) {
            block.folded = defaultFolded
        }
    }

    onSearchTextChanged: {
        applySearchToBlock(generalBlock, true)
        applySearchToBlock(notificationsBlock, true)
        applySearchToBlock(utilitiesBlock, true)
        applySearchToBlock(privacyBlock, settingsPrivacyFolded)
        applySearchToBlock(appsBlock, true)
        applySearchToBlock(caBlock, unfoldSection != "CA")
        applySearchToBlock(resourcesBlock, true)
        applySearchToBlock(reportBlock, true)
        applySearchToBlock(remoteNodeBlock, unfoldSection != "BEAM_NODE")
        applySearchToBlock(nodeBlock, unfoldSection != "BEAM_NODE")
        applySearchToBlock(ipfsBlock, unfoldSection != "IPFS_NODE")
        applySearchToBlock(swapEthSettings, creating ? (unfoldSection == viewModel.ethSettings.coinID ? false : (unfoldSection == "ALL_COINS" ? viewModel.ethSettings.isConnected : true)) : viewModel.ethSettings.folded)
    }

    //% "Settings"
    property string title:       qsTrId("settings-title")
    property var titleContent: RowLayout {
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
                id: versionText
                horizontalAlignment: Text.AlignRight
                Layout.leftMargin: 20
                font.pixelSize: 14
                color: Style.content_secondary
                //: settings tab, version label
                //% "v"
                text: qsTrId("settings-version") + viewModel.version
                
                MouseArea {
                anchors.fill:parent
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onClicked: function () {
		                    BeamGlobals.copyToClipboard(versionText.text)
                }
           } 
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

    SearchBox {
        Layout.fillWidth: true
        Layout.leftMargin: 20
        Layout.rightMargin: 20
        Layout.bottomMargin: 10
        alwaysVisibleInput: true
        //% "Search settings"
        placeholderText: qsTrId("settings-search-placeholder")
        onTextChanged: settingsView.searchText = text
    }

    SFText {
        Layout.leftMargin: 20
        Layout.rightMargin: 20
        Layout.bottomMargin: 10
        visible: settingsView.searchActive && !settingsView.hasSearchResults()
        color: Style.content_secondary
        font.pixelSize: 14
        //% "No matching settings"
        text: qsTrId("settings-search-no-results")
    }

    ScrollView {
        Layout.fillHeight: true
        Layout.bottomMargin: 10
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical:          ScrollBar.AsNeeded
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
                    visible: settingsView.blockMatches(generalBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(generalBlock)
                }

                SettingsNotifications {
                    id: notificationsBlock
                    viewModel: viewModel
                    visible: settingsView.blockMatches(notificationsBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(notificationsBlock)
                }

                SettingsUtilities {
                    id: utilitiesBlock
                    viewModel: viewModel
                    visible: settingsView.blockMatches(utilitiesBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(utilitiesBlock)
                }

                SettingsPrivacy {
                    id: privacyBlock
                    viewModel: viewModel
                    folded: settingsPrivacyFolded
                    visible: settingsView.blockMatches(privacyBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(privacyBlock)
                }

                SettingsApps {
                    id: appsBlock
                    viewModel: viewModel
                    visible: settingsView.blockMatches(appsBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(appsBlock)
                }

                SettingsCA {
                    id: caBlock
                    viewModel: viewModel
                    folded: unfoldSection != "CA"
                    visible: settingsView.blockMatches(caBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(caBlock)
                }

                SettingsTitle {
                    topPadding: 30
                    //% "Troubleshooting"
                    text:  qsTrId("settings-troubleshooting-title")
                }

                SettingsResources {
                    id: resourcesBlock
                    viewModel: viewModel
                    visible: settingsView.blockMatches(resourcesBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(resourcesBlock)
                }

                SettingsReport {
                    id: reportBlock
                    viewModel: viewModel
                    visible: settingsView.blockMatches(reportBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(reportBlock)
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
                    visible: settingsView.blockMatches(remoteNodeBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(remoteNodeBlock)

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
                    visible: settingsView.blockMatches(nodeBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(nodeBlock)

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
                    visible: viewModel.ipfsSupported && settingsView.blockMatches(ipfsBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.blockMatches(ipfsBlock)
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
                        visible:                  settingsView.blockMatches(settingsControl)
                        searchActive:             settingsView.searchActive
                        searchMatched:            settingsView.blockMatches(settingsControl)

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
                    visible:                  settingsView.blockMatches(swapEthSettings)
                    searchActive:             settingsView.searchActive
                    searchMatched:            settingsView.blockMatches(swapEthSettings)
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
