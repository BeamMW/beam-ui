import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: generalBlock
    property var viewModel
    readonly property int settingsControlWidth: 210
    //: settings tab, general section, title
    //% "General"
    title: qsTrId("settings-general-title")

    content: ColumnLayout {
        spacing: 30

        RowLayout {
            Layout.preferredHeight: 16
            ColumnLayout {
                SFText {
                    Layout.fillWidth: true
                    //: settings tab, general section, language label
                    //% "Language"
                    text: qsTrId("settings-general-language")
                    color: Style.content_secondary
                    font.pixelSize: 14
                }
            }

            Item {
            }

            ColumnLayout {
                LanguageComboBox {
                    id: language
                    Layout.preferredWidth: settingsControlWidth
                    languages:     viewModel.supportedLanguages
                    languageIndex: viewModel.currentLanguageIndex
                    onLanguageActivated: (language) => { viewModel.currentLanguage = language }
                }
            }
            //visible: false  // Remove to enable language dropdown
        }

        RowLayout {
            SFText {
                Layout.fillWidth: true
                //: settings tab, general section, lock screen label
                //% "Lock screen"
                text: qsTrId("settings-general-lock-screen")
                color: Style.content_secondary
                font.pixelSize: 14
            }
            Item {}
            CustomComboBox {
                id: lockTimeoutControl
                fontPixelSize: 14
                Layout.preferredWidth: settingsControlWidth
                currentIndex: viewModel.lockTimeout
                model: [
                    //% "Never"
                    qsTrId("settings-general-lock-screen-never"),
                    //% "1 minute"
                    qsTrId("settings-general-lock-screen-1m"),
                    //% "5 minutes"
                    qsTrId("settings-general-lock-screen-5m"),
                    //% "15 minutes"
                    qsTrId("settings-general-lock-screen-15m"),
                    //% "30 minutes"
                    qsTrId("settings-general-lock-screen-30m"),
                    //% "1 hour"
                    qsTrId("settings-general-lock-screen-1h"),
                ]
                onActivated: {
                    viewModel.lockTimeout = lockTimeoutControl.currentIndex
                }
            }
        }

        RowLayout {
            SFText {
                Layout.fillWidth: true
                //: settings tab, general section, minimum confirmations
                //% "Minimum confirmations"
                text: qsTrId("settings-general-confirmations")
                color: Style.content_secondary
                font.pixelSize: 14
            }
            Item {}
            CustomComboBox {
                id: minConfirmationsControl
                fontPixelSize: 14
                Layout.preferredWidth: settingsControlWidth
                currentIndex: viewModel.minConfirmations
                model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
                onActivated: {
                    viewModel.minConfirmations = minConfirmationsControl.currentIndex
                }
            }
        }

        /*
        LinkButton {
            //% "Remove current wallet"
            text: qsTrId("settings-remove-wallet")
            linkColor: "#ff625c"
            bold: true
            onClicked: {
                // TODO:SETTINGS
            }
        }
        */
    }
}