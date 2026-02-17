import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Beam.Wallet 1.0
import "controls"
import "wallet"
import "utils.js" as Utils

ColumnLayout {
    id: assetsSwapLayout
    Layout.fillWidth:   true
    Layout.fillHeight:  true
    spacing: 0

    property var        assetsSwapStackView:    StackView.view
    function onClosed() {
        assetsSwapStackView.pop();
    }

    //% "Assets Swaps"
    property string     title:                  qsTrId("assets-swap-title")
    property var titleContent:
    RowLayout {
        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }
        CustomButton {
            id: sendOfferButton
            Layout.minimumWidth:    172
            Layout.preferredHeight: 32
            palette.button: Style.accent_incoming
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-create-offer.svg"
            //% "Create offer"
            text: qsTrId("atomic-swap-create")
            font.pixelSize: 12

            onClicked: {
                assetsSwapStackView.push(Qt.createComponent("create_asset_swap.qml"), {
                    "onClosed": function() {
                            ordersModel.updateAssets();
                            assetsSwapLayout.onClosed();
                        }
                    });
            }
        }
    }

    AssetsPanel {
        id: assets
        Layout.fillWidth:   true
        showSettingLink:    true

        Binding {
            target:    txTable
            property:  "selectedAssets"
            value:     assets.selectedIds
        }

        Binding {
            target:    ordersTable
            property:  "selectedAssets"
            value:     assets.selectedIds
        }
    }

    Item {
        id: tabSelector
        Layout.fillWidth: true
        Layout.topMargin: 25
        Layout.bottomMargin: 15
        height: 18

        property bool showOnlyMyOffers: false

        state: "offers"
        states: [
            State {
                name: "offers"
                PropertyChanges { target: offersTab; state: "active" }
                PropertyChanges { target: tabSelector; showOnlyMyOffers: false }
            },
            State {
                name: "myoffers"
                PropertyChanges { target: myOffersTab; state: "active" }
                PropertyChanges { target: tabSelector; showOnlyMyOffers: true }
            },
            State {
                name: "transactions"
                PropertyChanges { target: txsTab; state: "active" }
            }
        ]
            
        RowLayout {
            spacing: 25

            TabButton {
                id: offersTab
                //% "Active offers"
                label:              qsTrId("swap-active-offers-tab")
                Layout.alignment:   Qt.AlignVCenter
                onClicked:          tabSelector.state = "offers"
            }

            TabButton {
                id: myOffersTab
                //% "My offers"
                label:              qsTrId("swap-my-offers-tab")
                Layout.alignment:   Qt.AlignVCenter

                onClicked: function () {
                    tabSelector.state = "myoffers"
                    checkboxFitBalance.checked = false
                }
            }

            TabButton {
                id: txsTab
                label:              qsTrId("wallet-transactions-title")
                Layout.alignment:   Qt.AlignVCenter

                onClicked:          tabSelector.state = "transactions"
            }
        }
    }

    DexOrdersModel {
        id: ordersModel
    }

    RowLayout {
        visible: tabSelector.state != "transactions"
        spacing: 0
        Layout.minimumHeight: 20
        Layout.maximumHeight: 20
        Layout.bottomMargin: 15
        CustomCheckBox {
            id: checkboxFitBalance
            Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
            checked: false
            enabled: !tabSelector.showOnlyMyOffers
            //% "Fit my current balance"
            text: qsTrId("atomic-swap-fit-current-balance")
        }
    }

    CustomTableView {
        id: ordersTable
        Layout.alignment:     Qt.AlignTop
        Layout.fillWidth:     true
        Layout.fillHeight:    true
        Layout.bottomMargin:  9

        property var          selectedAssets: []

        selectionMode: SelectionMode.noSelection
        sortIndicatorVisible: true
        sortIndicatorColumn: 4
        sortIndicatorOrder: Qt.DescendingOrder

        onSortIndicatorColumnChanged: {
            sortIndicatorOrder = sortIndicatorColumn != 1
                ? Qt.AscendingOrder
                : Qt.DescendingOrder;
        }

        model: SortFilterProxyModel {
            id: ordersProxyModel
            source: SortFilterProxyModel {
                id:           assetFilterProxy
                filterRole:   "assetsFilter"
                filterString: ordersTable.selectedAssets.reduce(function(sum, current) { return sum + ["|", "\\b", current, "\\b"].join(""); }, "").slice(1)
                filterSyntax: SortFilterProxyModel.RegExp

                source: SortFilterProxyModel {
                    id:                    fitBalanceFilterProxy
                    filterRole:            "hasAssetToSend"
                    filterString:          checkboxFitBalance.visible && checkboxFitBalance.checked ? "true" : "*"
                    filterSyntax:          SortFilterProxyModel.Wildcard
                    filterCaseSensitivity: Qt.CaseInsensitive

                    source: ordersModel.orders
                }
            }

            filterRole: "isMine"
            filterString: tabSelector.showOnlyMyOffers ? "true" : "*"
            filterSyntax: SortFilterProxyModel.Wildcard
            filterCaseSensitivity: Qt.CaseInsensitive
            sortOrder: ordersTable.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortRole: ordersTable.getColumn(ordersTable.sortIndicatorColumn).role + "Sort"
        }
        visible: tabSelector.state != "transactions" && model.count > 0

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
                        height: sourceSize.height
                        width:  sourceSize.width
                        sourceSize: Qt.size(26, 26)
                        source: styleData.value["sendIcon"] ? styleData.value["sendIcon"] : ""
                    }
                    SvgImage {
                        height: sourceSize.height
                        width:  sourceSize.width
                        sourceSize: Qt.size(26, 26)
                        source: styleData.value["receiveIcon"] ? styleData.value["receiveIcon"] : ""
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
            delegate: 
            Item {
                width: parent.width
                height: ordersTable.rowHeight
                clip:true
                SFLabel {
                    font.pixelSize: 14
                    anchors.left: parent.left
                    anchors.right: parent.right
                    elide: Text.ElideMiddle
                    anchors.verticalCenter: parent.verticalCenter
                    text: styleData.value
                    color: Style.content_main
                    copyMenuEnabled: true
                    onCopyText: BeamGlobals.copyToClipboard(text)
                }
            }
        }

        TableViewColumn {
            role: "receive"

            //% "Receive"
            title: qsTrId("general-receive")
            width:     150 * ordersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate: 
            Item {
                width: parent.width
                height: ordersTable.rowHeight
                clip:true
                SFLabel {
                    font.pixelSize: 14
                    anchors.left: parent.left
                    anchors.right: parent.right
                    elide: Text.ElideMiddle
                    anchors.verticalCenter: parent.verticalCenter
                    text: styleData.value
                    color: Style.content_main
                    copyMenuEnabled: true
                    onCopyText: BeamGlobals.copyToClipboard(text)
                }
            }
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
                    id: cancelRow
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
                }
                MouseArea {
                    anchors.fill: cancelRow
                    visible: cancelRow.visible
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var orderId = ordersTable.model.getRoleValue(styleData.row, "id");
                        ordersModel.cancelOrder(orderId);
                    }
                }
                RowLayout {
                    id: acceptRow
                    visible: !styleData.value && !!ordersTable.model.getRoleValue(styleData.row, "hasAssetToSend");
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
                }
                MouseArea {
                    anchors.fill: acceptRow
                    visible: acceptRow.visible
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

    TxTable {
        Layout.fillWidth:  true
        Layout.fillHeight: true
        id: txTable
        dexFilter: true
        visible: tabSelector.state == "transactions"
    }

    ColumnLayout {
        Layout.minimumWidth: parent.width
        Layout.minimumHeight: parent.height
        visible: !ordersTable.visible && tabSelector.state != "transactions"

        SvgImage {
            Layout.topMargin: 100
            Layout.alignment: Qt.AlignHCenter
            source:     "qrc:/assets/icon-dex.svg"
            sourceSize: Qt.size(60, 60)
        }

        SFText {
            Layout.topMargin:     30
            Layout.alignment:     Qt.AlignHCenter
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            color:                Style.content_main
            opacity:              0.5
            lineHeight:           1.43

            text:                 tabSelector.showOnlyMyOffers ?
//% "There are no offers yet."
            qsTrId("atomic-no-my-offers")
            :
/*% "There are no active offers at the moment.
Please try again later or create an offer yourself."
*/
            qsTrId("atomic-no-offers")
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
