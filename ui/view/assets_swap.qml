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

    Title {
        //% "Assets Swaps"
        text: qsTrId("assets-swap-title")
    }

    Component {
        id: assetsSwapComponent

        ColumnLayout {
            id: assetsSwapLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            function onClosed() {
                assetsSwapStackView.pop();
            }

            RowLayout {
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.topMargin: 30
                spacing: 20

                // CustomButton {
                //     id: acceptOfferButton
                //     Layout.minimumWidth: 172
                //     Layout.preferredHeight: 32
                //     Layout.maximumHeight: 32
                //     palette.button: Style.accent_outgoing
                //     palette.buttonText: Style.content_opposite
                //     icon.source: "qrc:/assets/icon-accept-offer.svg"
                //     //% "Accept offer"
                //     text: qsTrId("atomic-swap-accept")
                //     font.pixelSize: 12
                //     onClicked: {
                //         console.log('accept affer');
                //     }
                // }

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
                        assetsSwapStackView.push(Qt.createComponent("create_asset_swap.qml"), {"onClosed": assetsSwapLayout.onClosed});
                    }
                }
            }

            AssetsPanel {
                id: assets
                Layout.topMargin: 25
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

            }

            DexOrdersModel {
                id: ordersModel
            }

            CustomTableView {
                id: ordersTable
                Layout.alignment:     Qt.AlignTop
                Layout.fillWidth:     true
                Layout.fillHeight:    true
                Layout.topMargin:     25
                Layout.bottomMargin:  9

                selectionMode: SelectionMode.NoSelection
                sortIndicatorVisible: true
                sortIndicatorColumn: 0
                sortIndicatorOrder: Qt.DescendingOrder

                model: SortFilterProxyModel {
                    id: ordersProxyModel

                    source: ordersModel.orders
                }
                visible: model.count > 0

                property real rowHeight: 56
                property double columnResizeRatio: width / 1000

                rowDelegate: Item {
                    height: ordersTable.rowHeight
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Rectangle {
                        anchors.fill: parent
                        color: styleData.selected ? Style.row_selected :
                                (styleData.alternate ? Style.background_row_even : Style.background_row_odd)
                    }
                }

                itemDelegate: TableItem {
                    text:  styleData.value
                    elide: styleData.elideMode
                    onCopyText: BeamGlobals.copyToClipboard(styleData.value)
                }

                TableViewColumn {
                    role: "coins"
                    id: coinColumn

                    //% "Coin"
                    title:     qsTrId("general-coin")
                    width:     105 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false

                    delegate: Item {
                        width: parent.width
                        height: ordersTable.rowHeight

                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 12

                        RowLayout {
                            spacing: -8
                            SvgImage {
                                z: 1
                                sourceSize: Qt.size(26, 26)
                                source: styleData.value["sendIcon"]
                            }
                            SvgImage {
                                sourceSize: Qt.size(26, 26)
                                source: styleData.value["receiveIcon"]
                            }
                        }
                    }
                }

                TableViewColumn {
                    role: "send"

                    //% "Send"
                    title:     qsTrId("general-send")
                    width:     150 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false
                }

                TableViewColumn {
                    role: "receive"

                    //% "Receive"
                    title: qsTrId("general-receive")
                    width:     150 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false
                }

                TableViewColumn {
                    role: "rate"

                    //% "Rate"
                    title: qsTrId("swap-rate")
                    width:     60 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false
                }

                TableViewColumn {
                    role: "created"

                    //% "Created on"
                    title: qsTrId("swap-time-created")
                    width:     150 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false
                }

                TableViewColumn {
                    role: "expiration"

                    //% "Expiration"
                    title: qsTrId("swap-expiration")
                    width:     150 * ordersTable.columnResizeRatio
                    movable:   false
                    resizable: false
                }

                TableViewColumn {
                    role: "isMine"
                    id:   actionColumn

                    width:     ordersTable.getAdjustedColumnWidth(actionColumn)
                    movable:   false
                    resizable: false

                    delegate: Item {
                        width: parent.width
                        height: ordersTable.rowHeight

                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.topMargin: 16

                        RowLayout {
                            visible: styleData.value
                            Layout.fillWidth: true
                            SvgImage {
                                z: 1
                                sourceSize: Qt.size(16, 16)
                                source: "qrc:/assets/icon-offer-cancel.svg"
                            }
                            SFLabel {
                                Layout.fillWidth: true
                                font {
                                    styleName: "Bold"
                                    weight:    Font.Bold
                                    pixelSize: 16
                                }
                                //% "cancel offer"
                                text: qsTrId("swap-cancel-offer")
                                color: Style.accent_fail
                                copyMenuEnabled: false
                            }
                            MouseArea {
                                x: parent.x
                                y: parent.y
                                width: parent.width
                                height: parent.height
                                acceptedButtons: Qt.LeftButton
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    console.log('cancel offer');
                                }
                            }
                        }
                        RowLayout {
                            visible: !styleData.value
                            SvgImage {
                                z: 1
                                sourceSize: Qt.size(16, 16)
                                source: "qrc:/assets/icon-offer-accept.svg"
                            }
                            SFLabel {
                                font {
                                    styleName: "Bold"
                                    weight:    Font.Bold
                                    pixelSize: 16
                                }
                                //% "accept offer"
                                text: qsTrId("swap-accept-offer")
                                color: Style.accent_incoming
                                copyMenuEnabled: false
                            }
                            MouseArea {
                                x: parent.x
                                y: parent.y
                                width: parent.width
                                height: parent.height
                                acceptedButtons: Qt.LeftButton
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    var orderId = ordersTable.model.getRoleValue(styleData.row, "id");
                                    assetsSwapStackView.push(
                                        Qt.createComponent("accept_asset_swap.qml"),
                                        {
                                            "onClosed": assetsSwapLayout.onClosed,
                                            "orderId": orderId
                                        });
                                }
                            }
                        }
                    }
                }
            }

            Item {
                Layout.topMargin:  25
                Layout.fillWidth:  true
                Layout.fillHeight: true
                visible:           !ordersTable.visible
            }

        }
    }

    StackView {
        id: assetsSwapStackView

        anchors.fill: parent
        anchors.topMargin: -27
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
