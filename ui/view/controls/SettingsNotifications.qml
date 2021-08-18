import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: notificationsBlock
    property var viewModel
    //% "Notifications"
    title: qsTrId("settings-notifications-title")

    content: ColumnLayout {
        spacing: 20

        CustomSwitch {
            id: walletVersionNotificationsSwitch
            //% "Wallet updates"
            text: qsTrId("settings-notifications-version")
            font.pixelSize: 14
            font.styleName:  "Regular"
            font.weight:     Font.Normal
            Layout.fillWidth: true
            checked: viewModel.notificationsSettings.isNewVersionActive
            palette.text: Style.content_secondary
            Binding {
                target: viewModel.notificationsSettings
                property: "isNewVersionActive"
                value: walletVersionNotificationsSwitch.checked
            }
        }

        CustomSwitch {
            id: txStatusNotificationsSwitch
            //% "Transaction status"
            text: qsTrId("settings-notifications-tx-status")
            font.pixelSize: 14
            font.styleName:  "Regular"
            font.weight:     Font.Normal
            Layout.fillWidth: true
            checked: viewModel.notificationsSettings.isTxStatusActive
            palette.text: Style.content_secondary
            Binding {
                target: viewModel.notificationsSettings
                property: "isTxStatusActive"
                value: txStatusNotificationsSwitch.checked
            }
        }

        /* May be will be restored in the future
        CustomSwitch {
            id: addressNotificationwSwitch
            //% "Address expiration"
            text: qsTrId("settings-notifications-address-expiration")
            font.pixelSize: 14
            Layout.fillWidth: true
            checked: viewModel.notificationsSettings.isAddressExpirationActive
            Binding {
                target:   viewModel.notificationsSettings
                property: "isAddressExpirationActive"
                value:    addressNotificationwSwitch.checked
            }
        }

        CustomSwitch {
            id: newsNotificationsSwitch
            //% "News"
            text: qsTrId("settings-notifications-news")
            font.pixelSize: 14
            Layout.fillWidth: true
            checked: viewModel.notificationsSettings.isBeamNewsActive
            Binding {
                target: viewModel.notificationsSettings
                property: "isBeamNewsActive"
                value: newsNotificationsSwitch.checked
            }
        } */
    }
}