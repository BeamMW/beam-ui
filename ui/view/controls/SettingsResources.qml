import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: resourcesBlock
    property var viewModel
    //% "Resources"
    title: qsTrId("settings-resources-title")

    content: GridLayout {
        columns:       2
        rows:          2
        rowSpacing:    30
        columnSpacing: 10

        SFText {
            Layout.preferredWidth: parent.width / 2 - parent.columnSpacing / 2
            //% "Keep wallet logs"
            text: qsTrId("settings-keep-logs")
            color: Style.content_secondary
            font.pixelSize: 14
        }

        CustomComboBox {
            id: logsTimeoutControl
            Layout.fillWidth: true
            fontPixelSize: 14
            // TODO:SETTINGS implement viewModel.logsTimeout
            currentIndex: 0 // viewModel.logsTimeout
            model: [
                //% "For all time"
                qsTrId("settings-logs-forever"),
                //% "6 months"
                qsTrId("settings-logs-6m"),
                //% "3 months"
                qsTrId("settings-logs-3m"),
                //% "1 month"
                qsTrId("settings-logs-1m"),
                //% "1 week"
                qsTrId("settings-logs-1w"),
                //% "1 day"
                qsTrId("settings-logs-1d"),
            ]
            onActivated: {
                viewModel.logsTimeout = logsTimeoutControl.currentIndex
            }
        }


        SFText {
            //: settings tab, general section, wallet data folder location label
            //% "Wallet folder location"
            text: qsTrId("settings-wallet-location-label")
            color: Style.content_secondary
            font.pixelSize: 14
            Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
            spacing: 10
            Layout.preferredHeight: spacing + folderText.height + folderButton.height

            SFText {
                id: folderText
                Layout.fillWidth: true
                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.walletLocation
                wrapMode: Text.WrapAnywhere
            }

            SFText {
                id: folderButton
                Layout.fillWidth: false
                font.pixelSize: 14
                color: Style.active
                //% "Show in folder"
                text: qsTrId("general-show-in-folder")
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        viewModel.openFolder(viewModel.walletLocation);
                    }
                }
            }
        }
    }
}
