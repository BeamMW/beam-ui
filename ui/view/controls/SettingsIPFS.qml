import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    property var viewModel
    property bool runNode: nodeStart.state != "never"

    //% "IPFS Integrated Node"
    title: qsTrId("settings-ipfsnode-title")

    content: ColumnLayout {
        spacing: 30

        GridLayout{
            columns: 2
            columnSpacing: 25
            rowSpacing: 30

            SFText {
                Layout.fillWidth: true
                //% "Run node on"
                text: qsTrId("settings-ipfs-start")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            TristateSwitch {
                id: nodeStart
                Layout.alignment: Qt.AlignHCenter
                height:  22
                width:   280
                choices: ["clientstart", "dapps", "never"]
                labels:  [
                    //% "Start"
                    qsTrId("settings-start-switch"),
                    //% "DApps"
                    qsTrId("settings-dapps-switch"),
                    //% "Never"
                    qsTrId("settings-never-switch"),
                    ]
                state:   viewModel.ipfsNodeStart

                Binding {
                    target: viewModel
                    property: "ipfsNodeStart"
                    value: nodeStart.state
                }
            }

            SFText {
                //% "Swarm Port"
                text: qsTrId("settings-ipfs-swarm-port")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: 0

                SFTextInput {
                    id: swarmPort
                    Layout.preferredWidth: 100
                    activeFocusOnTab: true
                    font.pixelSize: 14
                    color: swarmPort.acceptableInput ? Style.content_main : Style.validator_error
                    backgroundColor: swarmPort.acceptableInput ? Style.content_main : Style.validator_error

                    text: (viewModel.ipfsSwarmPort || "").toString()
                    validator: RegularExpressionValidator {regularExpression: /^([1-9][0-9]{0,3}|[1-5][0-9]{2,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/g}

                    Binding {
                        target: viewModel
                        property: "ipfsSwarmPort"
                        value: swarmPort.text
                    }
                }

                Item {
                    Layout.preferredWidth: 100
                    Layout.alignment: Qt.AlignRight
                    SFText {
                        color:          Style.validator_error
                        font.pixelSize: 12
                        font.italic:    true
                        text:           qsTrId("general-invalid-port")
                        visible:        !swarmPort.acceptableInput
                    }
                }
            }
        }

        //
        // Buttons
        //
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 25

            CustomButton {
                leftPadding:  25
                rightPadding: 25
                spacing: 12
                //% "Cancel"
                text: qsTrId("general-cancel")
                icon.source: enabled ? "qrc:/assets/icon-cancel-white.svg" : "qrc:/assets/icon-cancel.svg"
                enabled: viewModel.ipfsChanged
                onClicked: function () {
                    viewModel.undoChanges()
                }
            }

            Item {
                width: 20
            }

            PrimaryButton {
                leftPadding:  25
                rightPadding: 25
                spacing: 12
                //: settings tab, node section, apply button
                //% "Apply changes"
                text: qsTrId("settings-apply")
                icon.source: "qrc:/assets/icon-done.svg"
                enabled: viewModel.ipfsChanged

                onClicked: function () {
                    viewModel.applyIPFSChanges()
                }
            }
        }
    }
}
