import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    property var viewModel

    //% "Beam Node Connection"
    title: qsTrId("settings-remote-node-title")

    content: ColumnLayout {
        spacing:    0
        RowLayout {
            Layout.fillWidth:   true
            spacing:            10
            SFText {
                //: settings tab, node section, connect to intergrated node label
                //% "Integrated node"
                text: qsTrId("settings-connect-local-node")
                color: viewModel.connectLocalNode ? Style.active : Style.content_secondary
                font.pixelSize: 14
            }
            CustomSwitch {
                id: connectLocalNodeSwitch
                checked: !viewModel.connectLocalNode
                alwaysGreen: true
                spacing: 0
                Binding {
                    target: viewModel
                    property: "connectLocalNode"
                    value: !connectLocalNodeSwitch.checked
                }
            }
            SFText {
                //% "Remote node"
                text: qsTrId("settings-connect-remote-node")
                color: viewModel.connectLOcalNode ? Style.content_secondary : Style.active
                font.pixelSize: 14
            }
            Item {
                Layout.fillWidth: true
            }
        }

        //
        // Remote node settings
        //
        GridLayout {
            Layout.fillWidth: true
            Layout.topMargin: 30
            columnSpacing: 20
            rowSpacing: 30
            columns: 2

            SFText {
                //: settings tab, node section, address label
                //% "Node address"
                text: qsTrId("settings-remote-node-address")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                spacing: 0

                SFTextInput {
                    id: nodeAddress
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    focus: true
                    activeFocusOnTab: true
                    font.pixelSize: 14
                    color:  (nodeAddress.text.length && (!viewModel.isValidNodeAddress || !nodeAddress.acceptableInput)) ? Style.validator_error : Style.content_main
                    backgroundColor:  (nodeAddress.text.length && (!viewModel.isValidNodeAddress || !nodeAddress.acceptableInput)) ? Style.validator_error : Style.content_main
                    validator: RegularExpressionValidator { regularExpression: /^(\s|\x180E)*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|([\w.-]+(?:\.[\w\.-]+)+))(\s|\x180E)*$/ }
                    text: viewModel.nodeAddress
                    //% "Please enter the address"
                    placeholderText:  qsTrId("settings-remote-node-address-placeholder")
                    enabled: !viewModel.connectLocalNode
                    Binding {
                        target: viewModel
                        property: "nodeAddress"
                        value: nodeAddress.text.trim()
                    }
                }
                Item {
                    id: nodeAddressError
                    Layout.preferredWidth: 170
                    Layout.alignment: Qt.AlignRight
                    SFText {
                        anchors.right:  parent.right
                        color:          Style.validator_error
                        font.pixelSize: 12
                        font.italic:    true
                        text:           qsTrId("general-invalid-address")
                        visible:        (nodeAddress.text.length && (!viewModel.isValidNodeAddress || !nodeAddress.acceptableInput))
                    }
                }
            }

            // remote port
            SFText {
                text: qsTrId("settings-local-node-port")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                spacing: 0

                SFTextInput {
                    id: remoteNodePort
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                    activeFocusOnTab: true
                    font.pixelSize: 14
                    color: (text.length && !remoteNodePort.acceptableInput) ? Style.validator_error : Style.content_main
                    backgroundColor: (text.length && !remoteNodePort.acceptableInput) ? Style.validator_error : Style.content_main
                    text: viewModel.remoteNodePort
                    //% "Please enter the port"
                    placeholderText:  qsTrId("settings-local-node-port-placeholder")
                    validator: RegularExpressionValidator {regularExpression: /^([1-9][0-9]{0,3}|[1-5][0-9]{2,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/g}
                    enabled: !viewModel.connectLocalNode
                    Binding {
                        target: viewModel
                        property: "remoteNodePort"
                        value: remoteNodePort.text
                    }
                }
                Item {
                    id: nodePortError
                    Layout.preferredWidth: 170
                    Layout.alignment: Qt.AlignRight
                    SFText {
                        anchors.right:  parent.right
                        color:          Style.validator_error
                        font.pixelSize: 12
                        font.italic:    true
                        //: settings tab, node section, port error label
                        //% "Port is mandatory"
                        text:           qsTrId("general-invalid-port")
                        visible:        !remoteNodePort.acceptableInput
                    }
                }
            }
        }
        SFText {
            Layout.fillWidth:   true
            Layout.topMargin:   20
            Layout.leftMargin:  20
            Layout.rightMargin: 20
            color:              Style.content_main
            opacity:            0.5
            font.pixelSize:     14
            horizontalAlignment:Text.AlignHCenter
            wrapMode:           Text.Wrap
            //% "To support maximum anonymity set and offline transactions please connect to integrated node or to own node configured with your owner key."
            text:               qsTrId("remote-node-lelantus-warning")
            visible:            !viewModel.connectLocalNode && !statusbarModel.isConnectionTrusted
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
                enabled: viewModel.isNodeChanged
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
                    if (!viewModel.isNodeChanged) return false;
                    return viewModel.isValidNodeAddress && nodeAddress.acceptableInput && remoteNodePort.acceptableInput
                }
                onClicked: viewModel.applyNodeConnectionChanges()
            }
        }
    }
}
