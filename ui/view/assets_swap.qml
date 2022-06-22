import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "controls"
import "wallet"
import "utils.js" as Utils

Item {
    id: assetsSwapRoot
    Layout.fillWidth: true
    Layout.fillHeight: true

    RowLayout {
        Title {
            //% "Assets Swaps"
            text: qsTrId("assets-swap-title")
        }

    }

    Component {
        id: assetsSwapComponent

        ColumnLayout {
            id: assetsSwapLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 30
            spacing: 0

            RowLayout {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.topMargin: 30
                spacing: 20

                CustomButton {
                    id: acceptOfferButton
                    Layout.minimumWidth: 172
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32
                    palette.button: Style.accent_outgoing
                    palette.buttonText: Style.content_opposite
                    icon.source: "qrc:/assets/icon-accept-offer.svg"
                    //% "Accept offer"
                    text: qsTrId("atomic-swap-accept")
                    font.pixelSize: 12
                    onClicked: {
                        console.log('accept affer');
                    }
                }
                
                CustomButton {
                    id: sendOfferButton
                    Layout.minimumWidth: 172
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32
                    palette.button: Style.accent_incoming
                    palette.buttonText: Style.content_opposite
                    icon.source: "qrc:/assets/icon-create-offer.svg"
                    //% "Create offer"
                    text: qsTrId("atomic-swap-create")
                    font.pixelSize: 12

                    onClicked: {
                        console.log('Create offer');
                    }
                }
            }

            AssetsPanel {
                id: assets
                Layout.topMargin: 25
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                showSelected: false

            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    StackView {
        id: assetsSwapStackView

        anchors.fill: parent
        initialItem: assetsSwapComponent

        pushEnter: Transition {
            enabled: false
        }
        pushExit: Transition {
            enabled: false
        }
        popEnter: Transition {
            enabled: false
        }
        popExit: Transition {
            enabled: false
        }

        onCurrentItemChanged: {
            if (currentItem && currentItem.defaultFocusItem) {
                assetsSwapStackView.currentItem.defaultFocusItem.forceActiveFocus();
            }
        }
    }
}
