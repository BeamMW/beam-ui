import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils

Control {
    id: control

    property var viewModel


    state: "allOffers"
    states: [
        State {
            name: "allOffers"
            PropertyChanges { target: allTab; state: "active" }
            PropertyChanges { target: ordersProxyModel; filterRole: "isMine" }
            PropertyChanges { target: ordersProxyModel; filterString: "*" }
        },
        State {
            name: "myOffers"
            PropertyChanges { target: myTab; state: "active" }
            PropertyChanges { target: ordersProxyModel; filterRole: "isMine" }
            PropertyChanges { target: ordersProxyModel; filterString: "true" }
        },
        State {
            name: "otherOffers"
            PropertyChanges { target: otherTab; state: "active" }
            PropertyChanges { target: ordersProxyModel; filterRole: "isMine" }
            PropertyChanges { target: ordersProxyModel; filterString: "false" }
        }
    ]

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            Layout.fillWidth:    true
            Layout.bottomMargin: 10

            TxFilter {
                id: allTab
                Layout.alignment: Qt.AlignVCenter
                //% "All offers"
                label: qsTrId("dex-all-tab")
                onClicked: {
                    control.state = "allOffers"
                }
            }

            TxFilter {
                id: myTab
                Layout.alignment: Qt.AlignVCenter
                //% "Opened by me"
                label: qsTrId("dex-my-tab")
                onClicked: {
                    control.state = "myOffers"
                }
            }

            TxFilter {
                id: otherTab
                Layout.alignment: Qt.AlignVCenter
                //% "Opened by other users"
                label: qsTrId("dex-other-tab")
                onClicked: {
                    control.state = "otherOffers"
                }
            }
        }

        CustomTableView {
            id: ordersTable
            Layout.fillWidth : true
            Layout.fillHeight : true
            visible: model.count > 0

            property real rowHeight: 56
            property real resizableWidth: width - actionsColumn.width
            property real resizeRatio: resizableWidth / 780

            selectionMode:        SelectionMode.NoSelection
            sortIndicatorVisible: true
            sortIndicatorColumn:  0
            sortIndicatorOrder:   Qt.DescendingOrder

            model: SortFilterProxyModel {
                id: ordersProxyModel
                source: viewModel.orders

                sortOrder: ordersTable.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: ordersTable.getColumn(ordersTable.sortIndicatorColumn).role

                filterSyntax: SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
            }

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

            /*
            TableViewColumn {
                role: "timestamp"
                //% "Date | Time"
                title:     qsTrId("dex-date-time")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }
            */

            TableViewColumn {
                role: "type"
                //% "Type"
                title:     qsTrId("dex-type")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "price"
                //% "Price"
                title:     qsTrId("dex-price")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "size"
                //% "Size"
                title:     qsTrId("dex-size")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "total"
                //% "Total"
                title:     qsTrId("dex-total")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            /*TableViewColumn {
                role: "amountA"
                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "amountB"
                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "rate"
                //% "Exchange Rate"
                title:     qsTrId("general-rate")
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }
            */

            TableViewColumn {
                role: "expiration"
                //% "Expiration"
                title:     qsTrId("dex-expiration")
                width:     130 * ordersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
               id:       statusColumn
               role:     "status"
               //% "Status"
               title:     qsTrId("general-status")
               width:     ordersTable.getAdjustedColumnWidth(statusColumn)
               movable:   false
               resizable: false
            }

            TableViewColumn {
                id:        actionsColumn
                width:     40
                movable:   false
                resizable: false
                delegate:  CustomToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.verticalCenter
                    icon.source: "qrc:/assets/icon-actions.svg"
                    //% "Actions"
                    ToolTip.text: qsTrId("general-actions")
                    onClicked: function () {
                        orderMenu.orderID = ordersTable.model.getRoleValue(styleData.row, "id");
                        orderMenu.popup()
                    }
                }
            }

            ContextMenu {
                id:    orderMenu
                modal: true
                dim:   false
                property var orderID

                Action {
                    //% "Accept Order"
                    text: qsTrId("dex-accept-order")
                    icon.source: "qrc:/assets/icon-accept-offer.svg"
                    onTriggered: {
                        viewModel.acceptOrder(orderMenu.orderID)
                    }
                }
            }
        }

        ColumnLayout {
            Layout.topMargin: 100
            Layout.alignment: Qt.AlignHCenter
            visible: ordersTable.model.count == 0

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/atomic-empty-state.svg"
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
                //% "There are no active orders at the moment.\nPlease try again later or create an offer yourself."
                text: qsTrId("dex-no-orders")
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
