import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

SettingsFoldable {
    id: privacyBlock
    property var viewModel
    //% "Privacy"
    title: qsTrId("settings-privacy-title")

    ConfirmPasswordDialog {
        id: confirmPasswordDialog
        settingsViewModel: viewModel
    }

    ChangePasswordDialog {
        id: changePasswordDialog
        settingsViewModel: viewModel
        parent: main
    }

    OwnerKeyDialog {
        id: showOwnerKeyDialog
        settingsViewModel: viewModel
        parent: main
    }

    content: ColumnLayout {
        spacing: 30

        RowLayout {
            Layout.preferredHeight: 32
            Layout.fillWidth:       true
            SFText {
                property string beamUrl: "<a href='https://www.beam.mw/'>beam.mw</a>"
                //% "blockchain explorer"
                property string explorerUrl: "<a href='%1'>%2</a>".arg(Style.explorerUrl).arg(qsTrId("explorer"))
                //: general settings, label for alow open external links
/*% "Allow access to %1 and %2
(to fetch exchanges and transaction data)"
*/
                text: Style.linkStyle + qsTrId("settings-general-allow-beammw-label").arg(beamUrl).arg(explorerUrl)
                textFormat: Text.RichText
                font.pixelSize: 14
                color: allowBeamMWLinks.palette.text
                wrapMode: Text.WordWrap
                Layout.fillWidth:       true
                Layout.rightMargin:     20
                Layout.preferredHeight: 32
                linkEnabled: true
                onLinkActivated:  {
                    Utils.openExternalWithConfirmation(link)
                }
            }

            CustomSwitch {
                id: allowBeamMWLinks
                checked: viewModel.isAllowedBeamMWLinks
                Binding {
                    target: viewModel
                    property: "isAllowedBeamMWLinks"
                    value: allowBeamMWLinks.checked
                }
            }
        }

        CustomSwitch {
            id: isPasswordReqiredToSpendMoney
            //: settings tab, general section, ask password to send label
            //% "Ask password on every Send"
            text: qsTrId("settings-general-require-pwd-to-spend")
            checked: viewModel.isPasswordReqiredToSpendMoney
            Layout.fillWidth: true
            font.styleName:   "Regular"
            font.weight:      Font.Normal
            function onDialogAccepted() {
                viewModel.isPasswordReqiredToSpendMoney = checked;
            }

            function onDialogRejected() {
                checked = !checked;
            }

            onClicked: {
                confirmPasswordDialog.dialogTitle = viewModel.isPasswordReqiredToSpendMoney
                    //: settings tab, general section, ask password to send, confirm password dialog, title if checked
                    //% "Don't ask password on every Send"
                    ? qsTrId("settings-general-require-pwd-to-spend-confirm-pwd-title")
                    //: settings tab, general section, ask password to send, confirm password dialog, title if unchecked
                    //% "Ask password on every Send"
                    : qsTrId("settings-general-no-require-pwd-to-spend-confirm-pwd-title")
                //: settings tab, general section, ask password to send, confirm password dialog, message
                //% "Password verification is required to change that setting"
                confirmPasswordDialog.dialogMessage = qsTrId("settings-general-require-pwd-to-spend-confirm-pwd-message")
                confirmPasswordDialog.okButtonIcon = "qrc:/assets/icon-done.svg"
                //% "Proceed"
                confirmPasswordDialog.okButtonText = qsTrId("general-proceed")
                confirmPasswordDialog.onDialogAccepted = onDialogAccepted
                confirmPasswordDialog.onDialogRejected = onDialogRejected
                confirmPasswordDialog.open()
            }
        }

        SFText {
            //% "Anonymity set for Max privacy transactions"
            text: qsTrId("settings-privacy-mp-anonymity-set")
            wrapMode:   Text.WordWrap
            color: Style.content_main
            font.pixelSize: 14
        }

        SFText {
            Layout.topMargin: -40
            //% "Received coins can be spent immediately."
            text: "(" + qsTrId("settings-privacy-mp-notice") + ")"
            wrapMode:   Text.WordWrap
            color: Style.content_secondary
            font.pixelSize: 10
        }

        CustomComboBox {
            id: mpAnonymitySet
            fontPixelSize: 14
            Layout.preferredWidth: 150
            currentIndex: 0
            model: [
                "64k",
                "48k",
                "32k",
                "16k",
                "8k",
                "2k",
            ]
        }

        SFText {
            //% "Max privacy lock time limit"
            text: qsTrId("settings-privacy-mp-time-limit")
            wrapMode:   Text.WordWrap
            color: Style.content_main
            font.pixelSize: 14
        }

        CustomComboBox {
            id: mpLockTimeLimit
            fontPixelSize: 14
            Layout.preferredWidth: 150
            currentIndex: 0
            model: [
                //% "72h"
                qsTrId("settings-privacy-mp-time-limit-72"),
                //% "60h"
                qsTrId("settings-privacy-mp-time-limit-60"),
                //% "48h"
                qsTrId("settings-privacy-mp-time-limit-48"),
                //% "36h"
                qsTrId("settings-privacy-mp-time-limit-36"),
                //% "24h"
                qsTrId("settings-privacy-mp-time-limit-24"),
            ]
        }

        LinkButton {
            //: settings tab, general section, Show owner key button and dialog title
            //% "Show owner key"
            text: qsTrId("settings-general-require-pwd-to-show-owner-key")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                //: settings tab, general section, Show owner key button and dialog title
                //% "Show owner key"
                confirmPasswordDialog.dialogTitle = qsTrId("settings-general-require-pwd-to-show-owner-key")
                //: settings tab, general section, ask password to Show owner key, message
                //% "Password verification is required to see the owner key"
                confirmPasswordDialog.dialogMessage = qsTrId("settings-general-require-pwd-to-show-owner-key-message")
                //: settings tab, general section, Show owner key button and dialog title
                //% "Show owner key"
                confirmPasswordDialog.okButtonText = qsTrId("settings-general-require-pwd-to-show-owner-key")
                confirmPasswordDialog.okButtonIcon = "qrc:/assets/icon-show-key.svg"
                confirmPasswordDialog.onDialogAccepted = function () {
                    showOwnerKeyDialog.pwd = confirmPasswordDialog.pwd
                    showOwnerKeyDialog.open()
                }
                confirmPasswordDialog.onDialogRejected = function() {}
                confirmPasswordDialog.open();
            }
        }

        LinkButton {
            //% "Change wallet password"
            text: qsTrId("general-change-pwd")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                changePasswordDialog.open()
            }
        }

        /*LinkButton {
            //% "Clear local wallet data"
            text: qsTrId("settings-clear-local-data")
            linkColor: "#ffffff"
            bold: true
            onClicked: {
                // TODO:SETTINGS
            }
        }*/
    }
}
