import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"
import "../utils.js" as Utils
import "."

Control {
    id: control

    TxTableViewModel {
        id: tableViewModel
    }

    property int selectedAsset: -1

    state: "all"
    readonly property bool isContracts: state == "contracts"

    states: [
        State {
            name: "all"
            PropertyChanges { target: allTab; state: "active" }
            PropertyChanges { target: contractFilterProxy;  filterRole: "isContractTx" }
            PropertyChanges { target: contractFilterProxy;  filterString: "false" }
            PropertyChanges { target: emptyMessage;  
                //% "Your transaction list is empty"
                text: qsTrId("tx-empty")
            }
            
        },
        State {
            name: "inProgress"
            PropertyChanges { target: inProgressTab; state: "active" }
            PropertyChanges { target: txProxyModel; filterRole: "isInProgress" }
            PropertyChanges { target: txProxyModel; filterString: "true" }
            PropertyChanges { target: contractFilterProxy;  filterRole: "isContractTx" }
            PropertyChanges { target: contractFilterProxy;  filterString: "false" }
            PropertyChanges { target: emptyMessage;  
                //% "There are no in progress transactions yet."
                text: qsTrId("tx-in-progress-empty")
            }
        },
        State {
            name: "sent"
            PropertyChanges { target: sentTab; state: "active" }
            PropertyChanges { target: txProxyModel; filterRole: "isSent" }
            PropertyChanges { target: txProxyModel; filterString: "true" }
            PropertyChanges { target: contractFilterProxy;  filterRole: "isContractTx" }
            PropertyChanges { target: contractFilterProxy;  filterString: "false" }
            PropertyChanges { target: emptyMessage;  
                //% "There are no sent transactions yet."
                text: qsTrId("tx-sent-empty")
            }
        },
        State {
            name: "received"
            PropertyChanges { target: receivedTab; state: "active" }
            PropertyChanges { target: txProxyModel; filterRole: "isReceived" }
            PropertyChanges { target: txProxyModel; filterString: "true" }
            PropertyChanges { target: contractFilterProxy;  filterRole: "isContractTx" }
            PropertyChanges { target: contractFilterProxy;  filterString: "false" }
            PropertyChanges { target: emptyMessage;  
                //% "There are no received transactions yet."
                text: qsTrId("tx-received-empty")
            }
        },
        State {
            name: "contracts"
            PropertyChanges { target: contractsTab; state: "active" }
            PropertyChanges { target: txProxyModel; filterRole: "" }
            PropertyChanges { target: txProxyModel; filterString: "" }
            PropertyChanges { target: contractFilterProxy;  filterRole: "isContractTx" }
            PropertyChanges { target: contractFilterProxy;  filterString: "true" }
            PropertyChanges { target: emptyMessage;  
                //% "There are no contracts transactions yet."
                text: qsTrId("tx-contracts-empty")
            }
        }
    ]

    onStateChanged: {
        transactionsTable.positionViewAtRow(0, ListView.Beginning)
    }

    ConfirmationDialog {
        id: deleteTransactionDialog
        //% "Delete"
        okButtonText: qsTrId("general-delete")

        property var txID
        onAccepted: function () {
            tableViewModel.deleteTx(txID)
       }
    }

    PaymentInfoDialog {
        id: paymentInfoDialog
        onTextCopied: function(text) {
            BeamGlobals.copyToClipboard(text);
        }
    }

    PaymentInfoItem {
        id: verifyInfo
    }

    PaymentInfoDialog {
        id: paymentInfoVerifyDialog
        shouldVerify: true

        model:verifyInfo
        onTextCopied: function(text) {
            BeamGlobals.copyToClipboard(text);
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            Layout.fillWidth:    true
            Layout.bottomMargin: 10
            visible:             tableViewModel.transactions.rowCount() > 0
            TxFilter {
                id: allTab
                Layout.alignment: Qt.AlignVCenter
                //% "All"
                label: qsTrId("wallet-transactions-all-tab")
                onClicked: control.state = "all"
            }

            TxFilter {
                id: inProgressTab
                Layout.alignment: Qt.AlignVCenter
                //% "In progress"
                label: qsTrId("wallet-transactions-in-progress-tab")
                onClicked: control.state = "inProgress"
            }

            TxFilter {
                id: sentTab
                Layout.alignment: Qt.AlignVCenter
                //% "Sent"
                label: qsTrId("wallet-transactions-sent-tab")
                onClicked: control.state = "sent"
            }

            TxFilter {
                id: receivedTab
                Layout.alignment: Qt.AlignVCenter
                //% "Received"
                label: qsTrId("wallet-transactions-received-tab")
                onClicked: control.state = "received"
            }

            TxFilter {
                id: contractsTab
                Layout.alignment: Qt.AlignVCenter
                //% "Contracts"
                label: qsTrId("wallet-transactions-contracts-tab")
                onClicked: control.state = "contracts"
            }

            Item {
                Layout.fillWidth: true
            }

            SearchBox {
               id: searchBox
               Layout.preferredWidth: 300
               Layout.alignment: Qt.AlignVCenter
               //% "Enter search text..."
               placeholderText: qsTrId("wallet-search-transactions-placeholder")
            }

            CustomToolButton {
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/assets/icon-export.svg"
                //: transactions history screen, export button tooltip and open file dialog
                //% "Export transactions history"
                ToolTip.text: qsTrId("wallet-export-tx-history")
                onClicked: {
                    tableViewModel.exportTxHistoryToCsv();
                }
            }

            CustomToolButton {
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/assets/icon-proof.svg"
                //% "Verify payment"
                ToolTip.text: qsTrId("wallet-verify-payment")
                onClicked: {
                    paymentInfoVerifyDialog.model.reset();
                    paymentInfoVerifyDialog.open();
                }
            }
        }

        ColumnLayout {
            Layout.topMargin: 90
            Layout.alignment: Qt.AlignHCenter
            visible: transactionsTable.model.count == 0

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/icon-wallet-empty.svg"
                sourceSize: Qt.size(60, 60)
            }

            SFText {
                id:                   emptyMessage
                Layout.topMargin:     30
                Layout.alignment:     Qt.AlignHCenter
                horizontalAlignment:  Text.AlignHCenter
                font.pixelSize:       14
                color:                Style.content_main
                opacity:              0.5
                lineHeight:           1.43
                //% "Your transaction list is empty"
                text: qsTrId("tx-empty")
            }

            Item {
                Layout.fillHeight: true
            }
        }

        CustomTableView {
            id: transactionsTable
            Component.onCompleted: {
                transactionsTable.model.modelReset.connect(function(){
                    if (root.openedTxID != "") {
                        var index = tableViewModel.transactions.index(0, 0);
                        var indexList = tableViewModel.transactions.match(index, TxObjectList.Roles.TxID, root.openedTxID)
                        if (indexList.length > 0) {
                            index = contractFilterProxy.mapFromSource(indexList[0])
                            index = assetFilterProxy.mapFromSource(index)
                            index = searchProxyModel.mapFromSource(index)
                            index = txProxyModel.mapFromSource(index)
                            transactionsTable.positionViewAtRow(index.row, ListView.Beginning)
                        }
                    }
                })
            }

            Layout.alignment:     Qt.AlignTop
            Layout.fillWidth:     true
            Layout.fillHeight:    true
            Layout.bottomMargin:  9
            visible:              transactionsTable.model.count > 0
            property real rowHeight: 56
            property real resizableWidth: transactionsTable.width - 140 /*actionsColumn.width + coinColumn.width*/
            property real columnResizeRatio: resizableWidth / 610

            selectionMode: SelectionMode.NoSelection
            sortIndicatorVisible: true
            sortIndicatorColumn: 1
            sortIndicatorOrder: Qt.DescendingOrder

            onSortIndicatorColumnChanged: {
                sortIndicatorOrder = sortIndicatorColumn != 1
                    ? Qt.AscendingOrder
                    : Qt.DescendingOrder;
            }

            model: SortFilterProxyModel {
                id: txProxyModel

                source: SortFilterProxyModel {
                    id: searchProxyModel
                    filterRole: "search"
                    filterString: searchBox.text
                    filterSyntax: SortFilterProxyModel.Wildcard
                    filterCaseSensitivity: Qt.CaseInsensitive

                    source: SortFilterProxyModel {
                        id: assetFilterProxy
                        filterRole:   "assetFilter"
                        filterString: control.selectedAsset < 0 ? "" : ["\\b", control.selectedAsset, "\\b"].join("")
                        filterSyntax: SortFilterProxyModel.RegExp

                        source: SortFilterProxyModel {
                            id: contractFilterProxy
                            filterRole:   "isContractTx"
                            filterString: "false"
                            filterSyntax: SortFilterProxyModel.FixedString
                            source: tableViewModel.transactions
                        }
                    }
                }

                sortOrder: transactionsTable.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: transactionsTable.getColumn(transactionsTable.sortIndicatorColumn).role + "Sort"
                filterSyntax: SortFilterProxyModel.Wildcard
            }

            rowDelegate: ExpandableRowDelegate {
                id:         rowItem
                collapsed:  true
                rowInModel: styleData.row !== undefined && styleData.row >= 0 &&  styleData.row < txProxyModel.count
                rowHeight:  transactionsTable.rowHeight
                tableView:  transactionsTable

                backgroundColor: styleData.selected ?
                                 Style.row_selected :
                                 (styleData.alternate ? (!collapsed || animating ? Style.background_row_details_even : Style.background_row_even)
                                                      : (!collapsed || animating ? Style.background_row_details_odd : Style.background_row_odd))

                property var model: parent.model
                property bool hideFiltered: true

                onLeftClick: function() {
                    if (!collapsed && searchBox.text.length && hideFiltered) {
                        hideFiltered = false;
                        return false;
                    }
                    return true;
                }

                delegate: TransactionDetails {
                    id: detailsPanel
                    width: transactionsTable.width
                    sendAddress:        model && model.addressFrom ? model.addressFrom : ""
                    receiveAddress:     model && model.addressTo ? model.addressTo : ""
                    senderIdentity:     model && model.senderIdentity ? model.senderIdentity : ""
                    receiverIdentity:   model && model.receiverIdentity ? model.receiverIdentity : ""
                    comment:            model && model.comment ? model.comment : ""
                    txID:               model && model.txID ? model.txID : ""
                    kernelID:           model && model.kernelID ? model.kernelID : ""
                    status:             model && model.status ? model.status : ""
                    failureReason:      model && model.failureReason ? model.failureReason : ""
                    isIncome:           model && model.isIncome ? model.isIncome : false
                    hasPaymentProof:    model && model.hasPaymentProof ? model.hasPaymentProof : false
                    isSelfTx:           model && model.isSelfTransaction ? model.isSelfTransaction : false
                    isContractTx:       model ? !!model.isContractTx : false
                    rawTxID:            model && model.rawTxID ? model.rawTxID : null
                    stateDetails:       model && model.stateDetails ? model.stateDetails : ""
                    isCompleted:        model && model.isCompleted ? model.isCompleted : false

                    addressType:        {
                        if (model) {
                            if (model.isMaxPrivacy) {
                                //% "Max privacy"
                                return qsTrId("tx-address-max-privacy")
                            }
                            if (model.isOfflineToken) {
                                //% "Offline"
                                return qsTrId("tx-address-offline") 
                            }
                            if (model.isPublicOffline) {
                                //% "Public offline"
                                return qsTrId("tx-address-public-offline") 
                            }
                            //% "Regular"
                            return qsTrId("tx-address-regular");
                        }
                        return ""
                    }

                    assetNames:         model && model.assetNames ? model.assetNames : []
                    assetIcons:         model ? model.assetIcons: []
                    assetAmounts:       model ? model.assetAmounts: []
                    assetIncome:        model ? model.assetAmountsIncome: []
                    assetRates:         model ? model.assetRates: []
                    totalValue:         model ? model.amountSecondCurrency : "0"
                    rateUnit:           tableViewModel.rateUnit

                    fee:             model && model.fee ? model.fee : "0"
                    feeRate:         model && model.feeRate ? model.feeRate : "0"
                    feeUnit:         qsTrId("general-beam")
                    feeRateUnit:     tableViewModel.rateUnit

                    searchFilter:    searchBox.text
                    hideFiltered:    rowItem.hideFiltered
                    token:           model ? model.token : ""
                    isShieldedTx:    model && model.isShieldedTx ? true : false

                    onSearchFilterChanged: function(text) {
                        rowItem.collapsed = searchBox.text.length == 0;
                        rowItem.hideFiltered = true;
                    }

                    onOpenExternal : function() {
                        var url = tableViewModel.explorerUrl + "block?kernel_id=" + detailsPanel.kernelID;
                        Utils.openExternalWithConfirmation(url);
                    }

                    onTextCopied: function (text) {
                        BeamGlobals.copyToClipboard(text);
                    }

                    onCopyPaymentProof: function() {
                        if (detailsPanel.rawTxID)
                        {
                            var paymentInfo = tableViewModel.getPaymentInfo(detailsPanel.rawTxID);
                            if (paymentInfo.paymentProof.length === 0)
                            {
                                paymentInfo.paymentProofChanged.connect(function() {
                                    textCopied(paymentInfo.paymentProof);
                                });
                            }
                            else
                            {
                                textCopied(paymentInfo.paymentProof);
                            }
                        }
                    }
                    onShowPaymentProof: {
                        if (detailsPanel.rawTxID)
                        {
                            paymentInfoDialog.model = tableViewModel.getPaymentInfo(detailsPanel.rawTxID);
                            paymentInfoDialog.open();
                        }
                    }
                }
            }

            itemDelegate: Item {
                Item {
                    width: parent.width
                    height: transactionsTable.rowHeight

                    TableItem {
                        text:  styleData.value || ""
                        elide: styleData.elideMode
                        onCopyText: BeamGlobals.copyToClipboard(styleData.value)
                    }
                }
            }

            TableViewColumn {
                role: "assetNames"
                id: coinColumn

                //% "Coin"
                title:     qsTrId("wallet-txs-coin")
                width:     100
                movable:   false
                resizable: false

                delegate: Item { CoinsList {
                    width:  parent.width
                    height: transactionsTable.rowHeight
                    icons:  model ? model.assetIcons : undefined
                    names:  model ? model.assetNames : undefined
                }}
            }

            TableViewColumn {
                role: "timeCreated"
                id: timeColumn

                //% "Created on"
                title:      qsTrId("wallet-txs-date-time")
                elideMode:  Text.ElideRight
                width:      110 * transactionsTable.columnResizeRatio
                movable:    false
                resizable:  false
            }

            TableViewColumn {
                role: "comment"

                //% "Description"
                title:     qsTrId("general-description")
                elideMode: Text.ElideRight
                width:     200 * transactionsTable.columnResizeRatio
                movable:   false
                resizable: false
                visible:   control.isContracts
            }

            TableViewColumn {
                role: "amountGeneral"

                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     200 * transactionsTable.columnResizeRatio
                movable:   false
                resizable: false

                delegate: Item { RowLayout {
                    width:  parent.width
                    height: transactionsTable.rowHeight

                    property var isIncome:    model && model.isIncome
                    property var prefix:      model && model.amountGeneral == "0" ? "" : (isIncome ? "+ " : "- ")
                    property var amountText:  model ? [prefix, Utils.uiStringToLocale(model.amountGeneral)].join('') : "0"

                    //% "Multiple assets"
                    property var displayText: model && model.isMultiAsset ? qsTrId("general-multiple-assets") : amountText

                    SFText {
                        text:              parent.displayText
                        color:             parent.isIncome ? Style.accent_incoming : Style.accent_outgoing
                        Layout.fillWidth:  true
                        Layout.leftMargin: 20
                        elide:             Text.ElideRight
                        font {
                            styleName: "Bold"
                            weight:    Font.Bold
                            pixelSize: 14
                        }
                    }
                }}
            }

            TableViewColumn {
                role: "amountSecondCurrency"

                title:     [tableViewModel.rateUnit || "USD", qsTrId("general-amount")].join(' ')
                elideMode: Text.ElideRight
                width:     200 * transactionsTable.columnResizeRatio
                movable:   false
                resizable: false
                visible:   !control.isContracts

                delegate: Item { RowLayout {
                    width:  parent.width
                    height: transactionsTable.rowHeight

                    property var amount: model ? model.amountSecondCurrency : "0"
                    property var prefix: model && model.isIncome ? "+ " : "- "

                    SFText {
                        text:                   parent.amount != "0" ? [parent.prefix, Utils.uiStringToLocale(parent.amount)].join('') : ""
                        color:                  Style.content_main
                        Layout.fillWidth:       true
                        Layout.leftMargin:      20
                        elide:                  Text.ElideRight
                        font {
                            styleName: "Bold"
                            weight:    Font.Bold
                            pixelSize: 14
                        }
                    }
                }}
            }

            TableViewColumn {
                id: statusColumn
                role: "status"
                //% "Status"
                title: qsTrId("general-status")
                width: transactionsTable.getAdjustedColumnWidth(statusColumn) // 100 * transactionsTable.columnResizeRatio
                movable: false
                resizable: false
                delegate: Item {
                    property var myModel: model
                    Item {
                        width: parent.width
                        height: transactionsTable.rowHeight

                        RowLayout {
                            id: statusRow
                            Layout.alignment: Qt.AlignLeft
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            spacing: 10

                            SvgImage {
                                id: statusIcon;
                                Layout.alignment: Qt.AlignLeft

                                sourceSize: Qt.size(20, 20)
                                source: getIconSource()
                                function getIconSource() {
                                    if (!model) {
                                        return "";
                                    }
                                    if (model.isInProgress) {
                                        if (model.isSelfTransaction) {
                                            return "qrc:/assets/icon-sending-own.svg";
                                        }
                                        return model.isIncome
                                            ? !model.isShieldedTx ? "qrc:/assets/icon-receiving.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-receiving-max-offline.svg" : "qrc:/assets/icon-receiving-max-online.svg"
                                            : !model.isShieldedTx ? "qrc:/assets/icon-sending.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-sending-max-offline.svg" : "qrc:/assets/icon-sending-max-online.svg";
                                    }
                                    else if (model.isCompleted) {
                                        if (model.isSelfTransaction) {
                                            return "qrc:/assets/icon-sent-own.svg";
                                        }
                                        return model.isIncome
                                            ? !model.isShieldedTx ? "qrc:/assets/icon-received.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-received-max-offline.svg" : "qrc:/assets/icon-received-max-online.svg"
                                            : !model.isShieldedTx ? "qrc:/assets/icon-sent.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-sent-max-offline.svg" : "qrc:/assets/icon-sent-max-online.svg";
                                    }
                                    else if (model.isExpired) {
                                        return "qrc:/assets/icon-expired.svg"
                                    }
                                    else if (model.isFailed) {
                                        return model.isIncome
                                            ? "qrc:/assets/icon-receive-failed.svg"
                                            : !model.isShieldedTx ? "qrc:/assets/icon-send-failed.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-failed-max-offline.svg" : "qrc:/assets/icon-failed-max-online.svg";
                                    }
                                    else {
                                        return model.isIncome
                                            ? "qrc:/assets/icon-receive-canceled.svg"
                                            : !model.isShieldedTx ? "qrc:/assets/icon-send-canceled.svg" :
                                                    model.isOfflineToken ? "qrc:/assets/icon-canceled-max-offline.svg" : "qrc:/assets/icon-canceled-max-online.svg";
                                    }
                                }
                            }
                            SFLabel {
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                Layout.fillWidth: true
                                font.pixelSize: 14
                                font.italic: true
                                wrapMode: Text.WordWrap
                                text: styleData.value
                                verticalAlignment: Text.AlignBottom
                                color: {
                                    if (!model || model.isExpired) {
                                        return Style.content_secondary;
                                    }
                                    if (model.isInProgress || model.isCompleted) {
                                        if (model.isSelfTransaction) {
                                            return Style.content_main;
                                        }
                                        return model.isIncome ? Style.accent_incoming : Style.accent_outgoing;
                                    } else if (model.isFailed) {
                                        return Style.accent_fail;
                                    }
                                    else {
                                        return Style.content_secondary;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            TableViewColumn {
                id: actionsColumn
                elideMode: Text.ElideRight
                width: 40
                movable: false
                resizable: false
                delegate: txActions
            }

            function showContextMenu(row) {
                txContextMenu.cancelEnabled = transactionsTable.model.getRoleValue(row, "isCancelAvailable");
                txContextMenu.deleteEnabled = transactionsTable.model.getRoleValue(row, "isDeleteAvailable");
                txContextMenu.txID = transactionsTable.model.getRoleValue(row, "rawTxID");
                txContextMenu.popup();
            }

            Component {
                id: txActions
                Item {
                    Item {
                        width: parent.width
                        height: transactionsTable.rowHeight
                        CustomToolButton {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 12
                            icon.source: "qrc:/assets/icon-actions.svg"
                            //% "Actions"
                            ToolTip.text: qsTrId("general-actions")
                            onClicked: {
                                transactionsTable.showContextMenu(styleData.row);
                            }
                        }
                    }
                }
            }

            ContextMenu {
                id: txContextMenu
                modal: true
                dim: false
                property bool cancelEnabled
                property bool deleteEnabled
                property var txID

                Action {
                    //% "Cancel"
                    text: qsTrId("general-cancel")
                    icon.source: "qrc:/assets/icon-cancel.svg"
                    enabled: txContextMenu.cancelEnabled
                    onTriggered: {
                        tableViewModel.cancelTx(txContextMenu.txID);
                    }
                }
                Action {
                    //% "Delete"
                    text: qsTrId("general-delete")
                    icon.source: "qrc:/assets/icon-delete.svg"
                    enabled: txContextMenu.deleteEnabled
                    onTriggered: {
                        //% "The transaction will be deleted. This operation can not be undone"
                        deleteTransactionDialog.text = qsTrId("wallet-txs-delete-message");
                        deleteTransactionDialog.txID = txContextMenu.txID
                        deleteTransactionDialog.open();
                    }
                }
            }
        }
    }
}
