import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 1
import QtGraphicalEffects 1
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

StartLayout {
    property Item defaultFocusItem: startMigration

    ColumnLayout {
        id: migrateColumn
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 0

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 40
            Layout.maximumHeight: 180
            Layout.fillWidth: true
            SFText {
                anchors.horizontalCenter: parent.horizontalCenter
                //% "Your wallet will be migrated to v "
                text: qsTrId("start-migration-message") + viewModel.walletVersion()
                color: Style.content_main
                font.pixelSize: 14
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 40
            Layout.maximumHeight: 180
            Layout.fillWidth: true
            visible: !viewModel.isOnlyOneInstanceStarted
            SFText {
                anchors.horizontalCenter: parent.horizontalCenter
                //% "The wallet is already started. Close all running wallets and start again."
                text: qsTrId("start-second-copy-error")
                color: Style.validator_error
                font.pixelSize: 14
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 20

            PrimaryButton {
                id: startMigration

                //: migration screen, start auto migration button
                //% "start auto migration"
                text: qsTrId("start-migration-button")
                icon.source: "qrc:/assets/icon-repeat.svg"
                enabled: viewModel.isOnlyOneInstanceStarted && viewModel.walletDBpaths[0].isPreferred
                onClicked: 
                {
                    for (var path of viewModel.walletDBpaths) {
                        if (path.isPreferred) {
                            migrateWalletDB(path.fullPath);
                            break;
                        }
                    }
                }
            }

            CustomButton {
                //: migration screen, select db file button
                //% "start manual migration"
                text: qsTrId("start-migration-manual-button")
                icon.source: "qrc:/assets/icon-folder.svg"
                enabled: viewModel.isOnlyOneInstanceStarted
                onClicked: {
                    startWizzardView.push(selectWalletDBPage);
                }
            }
        }

        Item {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
        }

        LinkButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 64
            Layout.bottomMargin: 37
            //% "Restore wallet or create a new one"
            text: qsTrId("general-restore-or-create-wallet")
            visible: viewModel.isOnlyOneInstanceStarted

            onClicked: {
                startWizzardView.push(walletStartPage);
            }
        }
    }
}