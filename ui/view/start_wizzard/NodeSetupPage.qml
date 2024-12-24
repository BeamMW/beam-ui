import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

WizzardPage {
    id: nodeSetupRectangle
    property Item defaultFocusItem: localNodeButton

    function onRestoreCancelled(useRandomNode) {
        if (useRandomNode) {
            nodeSetupRectangle.defaultFocusItem = randomNodeButton;
            randomNodeButton.checked = true;
        } else if (viewModel.getIsRunLocalNode()) {
            nodeSetupRectangle.defaultFocusItem = localNodeButton;
            localNodeButton.checked = true;

            portInput.text = viewModel.localPort;
            localNodePeer.text = viewModel.localNodePeer;
        } else {
            nodeSetupRectangle.defaultFocusItem = remoteNodeButton;
            remoteNodeButton.checked = true;

            remoteNodeAddrInput.text = viewModel.remoteNodeAddress;
        }
        nodeSetupRectangle.defaultFocusItem.focus = true;
    }

    Column {
        spacing: 30
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.preferredWidth: 730
        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            //% "Setup node connectivity"
            text: qsTrId("start-node-title")
            color: Style.content_main
            font.pixelSize: 36
        }
    }

    Column {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.preferredWidth: 440
        topPadding: 50

        clip: true

        spacing: 15
        ButtonGroup {
            id: nodePreferencesGroup
        }

        CustomRadioButton {
            id: localNodeButton
            //% "Run integrated node (recommended)"
            text: qsTrId("start-node-integrated-radio")
            ButtonGroup.group: nodePreferencesGroup
            font.pixelSize: 14
            checked: true
        }
        Column {
            id: localNodePanel
            visible: localNodeButton.checked
            width: parent.width
            leftPadding: 34

            spacing: 10

            SFText {
                //% "Enter port to listen"
                text: qsTrId("start-node-port-label")
                color: Style.content_main
                font.pixelSize: 14
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id:portInput
                width: parent.width - 40

                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.defaultPortToListen()
                validator: RegExpValidator { regExp: /^\d{1,5}$/ }
                onTextChanged: if (portInput.text.length > 0) portError.text = ""
            }

            SFText {
                id: portError
                color: Style.validator_error
                font.pixelSize: 14
            }

            SFText {
                //% "Peer"
                text: qsTrId("start-node-peer-label")
                color: Style.content_main
                font.pixelSize: 14
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id: localNodePeer
                width: parent.width - 40
                activeFocusOnTab: true
                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.chooseRandomNode()
                validator: RegExpValidator { regExp: /^(\s|\x180E)*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|([\w.-]+(?:\.[\w\.-]+)+))(:([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))?(\s|\x180E)*$/ }
                onTextChanged: if (peerError.text.length > 0) peerError.text = ""
            }

            SFText {
                id: peerError
                color: Style.validator_error
                font.pixelSize: 14
            }
        }

        CustomRadioButton {
            id: randomNodeButton
            //% "Connect to random remote node"
            text: qsTrId("start-node-random-radio")
            ButtonGroup.group: nodePreferencesGroup
            font.pixelSize: 14
            enabled: viewModel.isRecoveryMode == false
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            CustomRadioButton {
                id: remoteNodeButton
                //% "Connect to specific remote node"
                text: qsTrId("start-node-remote-radio")
                ButtonGroup.group: nodePreferencesGroup
                font.pixelSize: 14
                enabled: viewModel.isRecoveryMode == false
            }
            SFTextInput {
                Layout.alignment: Qt.AlignVCenter
                id:remoteNodeAddrInput
                visible: remoteNodeButton.checked
                width: parent.width - parent.spacing - remoteNodeButton.width
                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.defaultRemoteNodeAddr()
                validator: RegExpValidator { regExp: /^(\s|\x180E)*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|([\w.-]+(?:\.[\w\.-]+)+))(:([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))?(\s|\x180E)*$/ }
                onTextChanged: if (remoteNodeAddrInput.text.length > 0) remoteNodeAddrError.text = ""
            }
        }
        Column {
            id: remoteNodePanel
            visible: remoteNodeButton.checked
            width: parent.width
            leftPadding: 40

            spacing: 10

            SFText {
                id: remoteNodeAddrError
                color: Style.validator_error
                font.pixelSize: 14
            }
        }
    }

    buttons: [

        CustomButton {
            //% "Back"
            text: qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            visible: !isBadPortMode
            onClicked: startWizzardView.pop();
        },

        PrimaryButton {
            text: viewModel.isRecoveryMode ?
                //% "Restore wallet"
                qsTrId("general-restore-wallet") :
                //% "Start using your wallet"
                qsTrId("general-start-using");
            icon.source: viewModel.isRecoveryMode ? "qrc:/assets/icon-restore-blue.svg" : "qrc:/assets/icon-next-blue.svg"
            enabled: nodePreferencesGroup.checkState != Qt.Unchecked
            onClicked:{
                if (localNodeButton.checked) {
                    if (portInput.text.trim().length === 0) {
                        //% "Please specify port"
                        portError.text = qsTrId("start-node-port-empty-error");
                        return;
                    }
                    var effectivePort = parseInt(portInput.text.trim());
                    if (effectivePort > 65535 || effectivePort < 1) {
                        //% "Port must be a number between 1 and 65535"
                        portError.text = qsTrId("start-node-port-value-error");
                        return;
                    }
                    if (localNodePeer.text.trim().length === 0) {
                        //% "Please specify peer"
                        peerError.text = qsTrId("start-node-peer-empty-error");
                        return;
                    }
                    if (!localNodePeer.acceptableInput) {
                        //% "Incorrect address"
                        peerError.text = qsTrId("start-node-peer-error");
                        return;
                    }

                    viewModel.setupLocalNode(parseInt(portInput.text), localNodePeer.text);
                }
                else if (remoteNodeButton.checked) {
                    if (remoteNodeAddrInput.text.trim().length === 0) {
                        //% "Please specify address of the remote node"
                        remoteNodeAddrError.text = qsTrId("start-node-empty-error");
                        return;
                    }
                    viewModel.setupRemoteNode(remoteNodeAddrInput.text.trim());
                }
                else if (randomNodeButton.checked) {
                    viewModel.setupRandomNode();
                }

                if (isBadPortMode) {
                    viewModel.onNodeSettingsChanged();
                    navigateToMain();
                    //startWizzardView.push("qrc:/loading.qml", {"isRecoveryMode" : viewModel.isRecoveryMode, "isCreating" : true, "cancelCallback": startWizzardView.pop});
                } else {
                    createWallet();
                }
            }
        }
    ]
}