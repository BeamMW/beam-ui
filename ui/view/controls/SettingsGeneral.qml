import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: generalBlock
    property var viewModel
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
                CustomComboBox {
                    id: language
                    Layout.preferredWidth: secondCurrencySwitch.width
                    fontPixelSize: 14
                    enableScroll: true

                    model: viewModel.supportedLanguages
                    currentIndex: viewModel.currentLanguageIndex
                    onActivated: {
                        viewModel.currentLanguage = currentText;
                    }
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
                Layout.preferredWidth: secondCurrencySwitch.width
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
                //: settings tab, general section, amounts unit label
                //% "Show amounts in"
                text: qsTrId("settings-general-amounts-unit")
                color: Style.content_secondary
                font.pixelSize: 14
            }
            Item {}
            CustomSecondCurrencySwitch {
                id: secondCurrencySwitch
                width: 210 // generalBlock.width * 0.33
                height: 20
                state: viewModel.secondCurrency
                onStateChanged: {
                    viewModel.secondCurrency = secondCurrencySwitch.state;
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