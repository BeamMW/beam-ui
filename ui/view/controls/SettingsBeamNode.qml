import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    property var viewModel
    property real syncProgress
    //% "Beam Integrated Node"
    title: qsTrId("settings-integrated-node-title") + ((syncProgress > 0.0) ? " (" + syncProgress.toFixed(2) + "%)" : "")

    content: ColumnLayout {
        spacing:    0
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            SFText {
                Layout.fillWidth: true
                //: settings tab, node section, run node label
                //% "Run integrated node"
                text: qsTrId("settings-local-node-run-checkbox")
                color: viewModel.localNodeRun ? Style.active : Style.content_secondary
                font.pixelSize: 14
            }

            CustomSwitch {
                id: localNodeRun
                checked: viewModel.localNodeRun
                alwaysGreen: true
                spacing: 0
                Binding {
                    target: viewModel
                    property: "localNodeRun"
                    value: localNodeRun.checked
                }
            }

        }

        //
        // Integrated node settings
        //
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 20
            visible: viewModel.localNodeRun
            spacing: 20

            SFText {
                //: settings tab, node section, port label
                //% "Port"
                text: qsTrId("settings-local-node-port")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: 0

                SFTextInput {
                    id: localNodePort
                    Layout.preferredWidth: 100
                    activeFocusOnTab: true
                    font.pixelSize: 14

                    color: localNodePort.acceptableInput ? Style.content_main : Style.validator_error
                    text: (viewModel.localNodePort || "").toString()

                    validator: RegExpValidator {regExp: /^([1-9][0-9]{0,3}|[1-5][0-9]{2,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/g}
                    backgroundColor: localNodePort.acceptableInput ? Style.content_main : Style.validator_error

                    Binding {
                        target: viewModel
                        property: "localNodePort"
                        value: parseInt(localNodePort.text || "0")
                    }
                }
                Item {
                    id: localNodePortError
                    Layout.preferredWidth: 100
                    Layout.alignment: Qt.AlignRight
                    SFText {
                        color:          Style.validator_error
                        font.pixelSize: 12
                        font.italic:    true
                        text:           qsTrId("general-invalid-port")
                        visible:        !localNodePort.acceptableInput
                    }
                }
            }
        }

        SFText {
            //% "Peers"
            text: qsTrId("settings-peers-title")
            color: Style.content_main
            font.pixelSize: 16
            font.styleName: "Bold"; font.weight: Font.Bold
            visible: viewModel.localNodeRun
            Layout.topMargin: 25
        }

        RowLayout {
            Layout.topMargin:       15
            Layout.bottomMargin:    5
            Layout.rightMargin:     0
            spacing: 0
            visible: viewModel.localNodeRun

            ColumnLayout {
                SFTextInput {
                    Layout.preferredWidth: nodeBlock.width * 0.7
                    id: newLocalNodePeer
                    activeFocusOnTab: true
                    font.pixelSize: 14
                    color: getColor()
                    backgroundColor: getColor()
                    validator: RegExpValidator { regExp: /^(\s|\x180E)*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|([\w.-]+(?:\.[\w\.-]+)+))(:([1-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))?(\s|\x180E)*$/ }

                    function getColor () {
                        return text.length && (!acceptableInput || viewModel.hasPeer(text)) ? Style.validator_error : Style.content_main
                    }

                    function canAddPeer() {
                        return acceptableInput && !viewModel.hasPeer(newLocalNodePeer.text)
                    }
                }

                SFText {
                    font.pixelSize: 12
                    font.italic:    true
                    color:          Style.validator_error
                    //% "Peer already exists"
                    text:            viewModel.hasPeer(newLocalNodePeer.text) ? qsTrId("error-peer-exists") : ""
                }
            }

            Item {
                Layout.fillWidth: true
            }

            CustomToolButton {
                Layout.alignment: Qt.AlignRight
                Layout.bottomMargin: 12
                icon.source: "qrc:/assets/icon-add-green.svg"
                enabled:    newLocalNodePeer.canAddPeer()
                onClicked: {
                    if (newLocalNodePeer.canAddPeer()) {
                        viewModel.addLocalNodePeer(newLocalNodePeer.text.trim());
                        newLocalNodePeer.clear();
                    }
                }
            }
        }

        ListView {
            visible: viewModel.localNodeRun
            Layout.fillWidth: true
            Layout.preferredHeight: (viewModel.localNodePeers.length || 1 ) * 36
            model: viewModel.localNodePeers
            id: peersList
            
            delegate: RowLayout {
                width: parent.width
                height: 36
                SFText {
                    Layout.alignment: Qt.AlignVCenter
                    text: modelData
                    font.pixelSize: 14
                    color: Style.content_main
                    height: 16
                    elide: Text.ElideRight
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    padding: 0
                    spacing: 0
                    icon.source: "qrc:/assets/icon-delete.svg"
                    enabled: localNodeRun.checked
                    onClicked: viewModel.deleteLocalNodePeer(index)
                }
            }
        }

        //
        // Buttons
        //
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
            spacing:          20

            CustomButton {
                leftPadding:  25
                rightPadding: 25
                spacing: 12
                //% "Cancel"
                text: qsTrId("general-cancel")
                icon.source: enabled ? "qrc:/assets/icon-cancel-white.svg" : "qrc:/assets/icon-cancel.svg"
                enabled: viewModel.isLocalNodeChanged
                onClicked: viewModel.undoChanges()
            }

            PrimaryButton {
                leftPadding:  25
                rightPadding: 25
                spacing: 12
                //: settings tab, node section, apply button
                //% "Apply changes"
                text: qsTrId("settings-apply")
                icon.source: "qrc:/assets/icon-done.svg"
                enabled: {
                    if (!viewModel.isLocalNodeChanged) return false;
                    if (localNodeRun.checked) return viewModel.localNodePeers.length > 0 && localNodePort.acceptableInput
                    return true;
                }
                onClicked: viewModel.applyLocalNodeChanges()
            }
        }
    }
}
