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
    readonly property bool noSearchResults: searchActive && !hasAnySearchResult()

    property bool settingsPrivacyFolded: true

    function searchTerms(query) {
        var cleaned = query.trim().toLowerCase();
        if (cleaned.length == 0) {
            return [];
        }

        var terms = [cleaned];
        var words = cleaned.split(/\s+/);
        for (var i = 0; i < words.length; ++i) {
            if (words[i].length > 0 && terms.indexOf(words[i]) == -1) {
                terms.push(words[i]);
            }
        }
        return terms;
    }

    function valueMatches(value, terms) {
        if (value === undefined || value === null) {
            return false;
        }

        var text = value.toString().toLowerCase();
        for (var i = 0; i < terms.length; ++i) {
            if (text.indexOf(terms[i]) != -1) {
                return true;
            }
        }
        return false;
    }

    function isButtonItem(item) {
        if (!item) {
            return false;
        }

        var typeName = item.toString();
        return typeName.indexOf("Button") != -1 ||
               typeName.indexOf("MenuItem") != -1;
    }

    function hasMouseAreaChild(item) {
        try {
            for (var childIndex = 0; childIndex < item.children.length; ++childIndex) {
                var child = item.children[childIndex];
                if (child.toString().indexOf("MouseArea") != -1 &&
                    child.enabled &&
                    child.acceptedButtons != Qt.NoButton) {
                    return true;
                }
            }
        } catch (e) {
        }
        return false;
    }

    function matchesObject(item, terms, depth) {
        if (!item || depth > 16) {
            return false;
        }

        var properties = isButtonItem(item) || hasMouseAreaChild(item) ? [] : [
            "title",
            "generalTitle",
            "text",
            "placeholderText",
            "currentText",
            "displayText",
            "value",
            "address",
            "port",
            "username",
            "infuraProjectID",
            "accountIndex",
            "connectionStatus",
            "connectionError"
        ];

        for (var i = 0; i < properties.length; ++i) {
            try {
                if (valueMatches(item[properties[i]], terms)) {
                    return true;
                }
            } catch (e) {
            }
        }

        try {
            for (var childIndex = 0; childIndex < item.children.length; ++childIndex) {
                if (isButtonItem(item.children[childIndex])) {
                    continue;
                }
                if (matchesObject(item.children[childIndex], terms, depth + 1)) {
                    return true;
                }
            }
        } catch (e) {
        }

        try {
            if (item.contentItem && !isButtonItem(item.contentItem) && matchesObject(item.contentItem, terms, depth + 1)) {
                return true;
            }
        } catch (e) {
        }

        return false;
    }

    function matchesSearch(item) {
        return matchesObject(item, searchTerms(searchText), 0);
    }

    function showForSearch(item) {
        return !searchActive || matchesSearch(item);
    }

    function markSearchMatch(item) {
        return searchActive && matchesSearch(item);
    }

    function matchesSearchIfReady(item) {
        return item && matchesSearch(item);
    }

    function hasAnySearchResult() {
        try {
            if (matchesSearchIfReady(generalBlock) ||
                matchesSearchIfReady(notificationsBlock) ||
                matchesSearchIfReady(utilitiesBlock) ||
                matchesSearchIfReady(privacyBlock) ||
                matchesSearchIfReady(appsBlock) ||
                matchesSearchIfReady(caBlock) ||
                matchesSearchIfReady(resourcesBlock) ||
                matchesSearchIfReady(reportBlock) ||
                matchesSearchIfReady(remoteNodeBlock) ||
                matchesSearchIfReady(nodeBlock) ||
                matchesSearchIfReady(ipfsBlock) ||
                matchesSearchIfReady(swapEthSettings)) {
                return true;
            }

            for (var i = 0; i < swapSettingsList.count; ++i) {
                if (matchesSearchIfReady(swapSettingsList.itemAt(i))) {
                    return true;
                }
            }
        } catch (e) {
        }
        return false;
    }

    Component.onCompleted: {
        settingsView.creating = false
    }

    SettingsViewModel {
        id: viewModel
    }

    //% "Settings"
    property string title:       qsTrId("settings-title")
    property var titleContent: Item {
        Layout.fillWidth: true
        Layout.preferredHeight: 56

        RowLayout {
            id: settingsHeaderStats
            anchors.top: parent.top
            anchors.right: parent.right
            spacing: 6

            SFText {
                visible: !settingsSearch.searchInput.visible && settingsSearch.text.length == 0
                //% "Blockchain height"
                text: qsTrId("settings-blockchain-height")
                color: Style.content_secondary
                font.pixelSize: 14
            }

            SFLabel {
                visible: !settingsSearch.searchInput.visible && settingsSearch.text.length == 0
                color: Style.content_main
                text: viewModel.currentHeight
                font.pixelSize: 14
                font.styleName:      "Bold"
                font.weight:         Font.Bold
            }

            SFText {
                id: versionText
                Layout.leftMargin: !settingsSearch.searchInput.visible && settingsSearch.text.length == 0 ? 14 : 0
                font.pixelSize: 14
                color: Style.content_secondary
                //: settings tab, version label
                //% "v"
                text: qsTrId("settings-version") + viewModel.version

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: function () {
                        BeamGlobals.copyToClipboard(versionText.text)
                    }
                }
            }
        }

        PrimaryButton {
            id: updateWalletButton
            anchors.right: settingsSearch.left
            anchors.rightMargin: 20
            anchors.verticalCenter: settingsSearch.verticalCenter
            height: 38
            //: settings update wallet button
            //% "update wallet"
            text: qsTrId("settings-update-wallet")
            icon.source: "qrc:/assets/icon-repeat.svg"
            visible: main.hasNewerVersion && !settingsSearch.searchInput.visible && settingsSearch.text.length == 0
            onClicked: Utils.navigateToDownloads()
        }

        SearchBox {
            id: settingsSearch
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: settingsSearch.searchInput.visible || settingsSearch.text.length > 0 ? Math.min(480, parent.width) : 32
            height: 32
            //% "Search by section name or parameter"
            placeholderText: qsTrId("settings-search-placeholder")
            onTextChanged: settingsView.searchText = text
        }
    }

    ScrollView {
        id: settingsScroll
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.bottomMargin: 10
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical:          ScrollBar.AsNeeded
        clip: true

            Item {
                id: settingsContent
                width: settingsView.width
                height: implicitHeight
                implicitWidth: settingsView.width
                implicitHeight: settingsView.noSearchResults
                                ? Math.max(settingsScroll.height, noSearchResultsColumn.implicitHeight)
                                : settingsColumns.implicitHeight

            ColumnLayout {
                id: noSearchResultsColumn
                anchors.centerIn: parent
                visible: settingsView.noSearchResults
                spacing: 18

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 72
                    Layout.preferredHeight: 72

                    Rectangle {
                        x: 5
                        y: 5
                        width: 44
                        height: 44
                        radius: 22
                        color: "transparent"
                        border.color: Style.content_secondary
                        border.width: 5
                        opacity: 0.45
                    }

                    Rectangle {
                        x: 45
                        y: 43
                        width: 26
                        height: 6
                        radius: 3
                        color: Style.content_secondary
                        opacity: 0.45
                        rotation: 45
                        transformOrigin: Item.Left
                    }

                    Rectangle {
                        x: 22
                        y: 25
                        width: 16
                        height: 5
                        radius: 2
                        color: Style.content_secondary
                        opacity: 0.45
                        rotation: 45
                    }

                    Rectangle {
                        x: 22
                        y: 25
                        width: 16
                        height: 5
                        radius: 2
                        color: Style.content_secondary
                        opacity: 0.45
                        rotation: -45
                    }
                }

                SFText {
                    Layout.alignment: Qt.AlignHCenter
                    //% "No results found"
                    text: qsTrId("settings-search-no-results")
                    color: Style.content_secondary
                    font.pixelSize: 16
                }
            }

            RowLayout {
                id: settingsColumns
                width: parent.width
                visible: !settingsView.noSearchResults
                spacing: 10

                ColumnLayout {
                    Layout.preferredWidth: parent.width / 2 - parent.spacing / 2
                    Layout.alignment: Qt.AlignTop
                    spacing: 10

                SettingsTitle {
                    id: walletTitle
                    visible: !settingsView.searchActive ||
                             settingsView.showForSearch(generalBlock) ||
                             settingsView.showForSearch(notificationsBlock) ||
                             settingsView.showForSearch(utilitiesBlock) ||
                             settingsView.showForSearch(privacyBlock) ||
                             settingsView.showForSearch(appsBlock) ||
                             settingsView.showForSearch(caBlock)
                    //% "Wallet"
                    text:  qsTrId("settings-wallet-title")
                }

                SettingsGeneral {
                    id: generalBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(generalBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(generalBlock)
                }

                SettingsNotifications {
                    id: notificationsBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(notificationsBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(notificationsBlock)
                }

                SettingsUtilities {
                    id: utilitiesBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(utilitiesBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(utilitiesBlock)
                }

                SettingsPrivacy {
                    id: privacyBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(privacyBlock)
                    folded: settingsPrivacyFolded
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(privacyBlock)
                }

                SettingsApps {
                    id: appsBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(appsBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(appsBlock)
                }

                SettingsCA {
                    id: caBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(caBlock)
                    folded: unfoldSection != "CA"
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(caBlock)
                }

                SettingsTitle {
                    id: troubleshootingTitle
                    visible: !settingsView.searchActive ||
                             settingsView.showForSearch(resourcesBlock) ||
                             settingsView.showForSearch(reportBlock)
                    topPadding: 30
                    //% "Troubleshooting"
                    text:  qsTrId("settings-troubleshooting-title")
                }

                SettingsResources {
                    id: resourcesBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(resourcesBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(resourcesBlock)
                }

                SettingsReport {
                    id: reportBlock
                    viewModel: viewModel
                    visible: settingsView.showForSearch(reportBlock)
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(reportBlock)
                }
                }

                ColumnLayout {
                    Layout.preferredWidth: parent.width / 2 - parent.spacing / 2
                    Layout.alignment: Qt.AlignTop
                    spacing: 10

                SettingsTitle {
                    id: connectivityTitle
                    visible: !settingsView.searchActive ||
                             settingsView.showForSearch(remoteNodeBlock) ||
                             settingsView.showForSearch(nodeBlock) ||
                             settingsView.showForSearch(ipfsBlock) ||
                             settingsView.showForSearch(swapEthSettings)
                    //% "Connectivity"
                    text:  qsTrId("settings-connectivity-title")
                }

                SettingsBeamRemoteNode {
                    id: remoteNodeBlock
                    viewModel: viewModel

                    showStatus: true
                    connectionStatus: getStatus()
                    connectionError:  main.statusBar.walletError
                    visible: settingsView.showForSearch(remoteNodeBlock)
                    folded: unfoldSection != "BEAM_NODE"
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(remoteNodeBlock)

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
                    visible: settingsView.showForSearch(nodeBlock)
                    folded: unfoldSection != "BEAM_NODE"
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(nodeBlock)
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
                    visible: viewModel.ipfsSupported && settingsView.showForSearch(ipfsBlock)
                    folded: unfoldSection != "IPFS_NODE"
                    searchActive: settingsView.searchActive
                    searchMatched: settingsView.markSearchMatch(ipfsBlock)

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
                        visible:                  settingsView.showForSearch(settingsControl)
                        folded:                   creating ? modelData.folded :
                                                             (unfoldSection == modelData.coinID ? false : (unfoldSection == "ALL_COINS" ? modelData.isConnected : true))
                        searchActive:             settingsView.searchActive
                        searchMatched:            settingsView.markSearchMatch(settingsControl)


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
                    visible:                  settingsView.showForSearch(swapEthSettings)
                    searchActive:             settingsView.searchActive
                    searchMatched:            settingsView.markSearchMatch(swapEthSettings)

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
}
