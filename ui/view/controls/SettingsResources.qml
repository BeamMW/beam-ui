import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.15
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
        columnSpacing: 25

        /*SFText {
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
        }*/

        SFText {
            //: settings tab, general section, wallet data folder location label
            //% "Wallet folder location"
            text: qsTrId("settings-wallet-location-label")
            color: Style.content_secondary
            font.pixelSize: 14
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
        }

        ColumnLayout {
            spacing: 10

            SFText {
                id: folderText
                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.walletLocation
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            SFText {
                font.pixelSize: 14
                color: Style.active
                //% "Show in folder"
                text: qsTrId("general-show-in-folder")
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: function () {
                        viewModel.openFolder(viewModel.walletLocation);
                    }
                }
            }
        }

        SFText {
            //% "IPFS repository location"
            text: qsTrId("settings-ipfs-location")
            color: Style.content_secondary
            font.pixelSize: 14
            Layout.alignment: Qt.AlignTop
            visible: viewModel.ipfsSupported
        }

        ColumnLayout {
            spacing: 10
            visible: viewModel.ipfsSupported

            SFText {
                font.pixelSize: 14
                color: Style.content_main
                text: viewModel.ipfsLocation
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            SFText {
                font.pixelSize: 14
                color: Style.active
                //% "Show in folder"
                text: qsTrId("general-show-in-folder")
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: function () {
                        viewModel.openFolder(viewModel.ipfsLocation);
                    }
                }
            }
        }
    }
}
