import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: control
    property var viewModel

    readonly property bool isChanged: viewModel.appsServerPort != parseInt(appsServerPort.text)
    readonly property bool isValid: appsServerPort.acceptableInput

    function undoChanges () {
        appsServerPort.text = viewModel.appsServerPort
    }

    title: "DApps"
    content: ColumnLayout {
        spacing: 0

        //
        // Local apps server port
        //
        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            SFText {
                //: settings tab, daps section, local apps server port label
                //% "Local server port"
                text: qsTrId("settings-dapps-port")
                color: Style.content_secondary
                font.pixelSize: 14
                wrapMode: Text.NoWrap
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                spacing: 0

                SFTextInput {
                    id: appsServerPort
                    Layout.preferredWidth: 100
                    topPadding: 0
                    activeFocusOnTab: true
                    font.pixelSize: 14
                    color: !appsServerPort.acceptableInput ? Style.validator_error : Style.content_main
                    text:  viewModel.appsServerPort
                    validator: RegExpValidator {regExp: /^([1-9][0-9]{0,3}|[1-5][0-9]{2,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/g}
                    backgroundColor: !appsServerPort.acceptableInput ? Style.validator_error : Style.content_main
                }

                Item {
                    Layout.preferredWidth: 100
                    Layout.alignment: Qt.AlignRight
                    SFText {
                        color:          Style.validator_error
                        font.pixelSize: 12
                        font.italic:    true
                        text:           qsTrId("general-invalid-port")
                        visible:        !appsServerPort.acceptableInput
                    }
                }
            }
        }

        //
        // Buttons
        //
        RowLayout {
            Layout.fillWidth:    true
            Layout.alignment:    Qt.AlignHCenter
            Layout.topMargin:    35

            CustomButton {
                leftPadding: 25
                rightPadding: 25
                spacing: 12
                //% "Cancel"
                text: qsTrId("general-cancel")
                icon.source: enabled ? "qrc:/assets/icon-cancel-white.svg" : "qrc:/assets/icon-cancel.svg"

                enabled: control.isChanged
                onClicked: function () {
                    undoChanges()
                }
            }

            Item {
                width: 20
            }

            PrimaryButton {
                leftPadding:  25
                rightPadding: 25
                spacing: 12
                //% "Apply changes"
                text: qsTrId("settings-apply")
                icon.source: "qrc:/assets/icon-done.svg"
                enabled: control.isChanged && control.isValid
                //enabled: {
                //    if (!viewModel.isNodeChanged) return false;
                //    if (!localNodeRun.checked) return viewModel.localNodePeers.length > 0 && localNodePort.acceptableInput
                //    return viewModel.isValidNodeAddress && nodeAddress.acceptableInput && remoteNodePort.acceptableInput
                //}
                //onClicked: viewModel.applyChanges()
            }
        }
    }
}
