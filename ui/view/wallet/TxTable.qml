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

    property int    selectedAsset: -1
    property int    emptyMessageMargin: 90
    property int    activeTxCnt: 0
    property alias  headerShaderVisible: transactionsTable.headerShaderVisible
    property var    dappFilter: undefined
    readonly property bool sourceVisible: dappFilter ? dappFilter == "all" : true

    state: "all"

    states: [
        State {
            name: "all"
            PropertyChanges { target: allTab; state: "active" }
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
            PropertyChanges { target: emptyMessage;  
                //% "There are no received transactions yet."
                text: qsTrId("tx-received-empty")
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

    TransactionDetailsPopup {
        id: txDetails
        onTextCopied: function(text) {
            BeamGlobals.copyToClipboard(text);
        }
        onOpenExternal: function(kernelID) {
            var url = tableViewModel.explorerUrl + "block?kernel_id=" + kernelID;
            Utils.openExternalWithConfirmation(url);
        };
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
            Layout.topMargin: emptyMessageMargin
            Layout.alignment: Qt.AlignHCenter
            visible: transactionsTable.model.count == 0

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/icon-wallet-empty.svg"
                sourceSize: Qt.size(60, 60)
            }

            SFText {
                id:                   emptyMessage
                Layout.topMargin:     emptyMessageMargin / 3
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

            property var initTxDetailsFromRow: function (model, row) {
                var addressFrom = model.getRoleValue(row, "addressFrom");
                txDetails.sendAddress = addressFrom ? addressFrom : "";
                var addressTo = model.getRoleValue(row, "addressTo");
                txDetails.receiveAddress = addressTo ? addressTo : "";
                var senderIdentity = model.getRoleValue(row, "senderIdentity");
                txDetails.senderIdentity = senderIdentity ? senderIdentity : "";
                var receiverIdentity = model.getRoleValue(row, "receiverIdentity");
                txDetails.receiverIdentity = receiverIdentity ? receiverIdentity : "";
                var comment = model.getRoleValue(row, "comment");
                txDetails.comment = comment ? comment : "";
                var txID = model.getRoleValue(row, "txID");
                txDetails.txID = txID ? txID : "";
                var kernelID = model.getRoleValue(row, "kernelID");
                txDetails.kernelID = kernelID ? kernelID : "";
                var status = model.getRoleValue(row, "status");
                txDetails.status = status ? status : "";
                var failureReason = model.getRoleValue(row, "failureReason");
                txDetails.failureReason = failureReason ? failureReason : "";
                var isIncome = model.getRoleValue(row, "isIncome");
                txDetails.isIncome = isIncome;
                var hasPaymentProof = model.getRoleValue(row, "hasPaymentProof");
                txDetails.hasPaymentProof = hasPaymentProof;
                var isSelfTransaction = model.getRoleValue(row, "isSelfTransaction");
                txDetails.isSelfTx = isSelfTransaction;
                var isContractTx = model.getRoleValue(row, "isContractTx");
                txDetails.isContractTx = isContractTx;
                var cidsStr = model.getRoleValue(row, "cidsStr");
                txDetails.cidsStr = cidsStr ? cidsStr : "";
                var rawTxID = model.getRoleValue(row, "rawTxID");
                txDetails.rawTxID = rawTxID ? rawTxID : null;
                var stateDetails = model.getRoleValue(row, "stateDetails");
                txDetails.stateDetails = stateDetails ? stateDetails : "";
                var isCompleted = model.getRoleValue(row, "isCompleted");
                txDetails.isCompleted = isCompleted;
                var minConfirmations = model.getRoleValue(row, "minConfirmations");
                txDetails.minConfirmations = minConfirmations ? minConfirmations : 0;
                var confirmationsProgress = model.getRoleValue(row, "confirmationsProgress");
                txDetails.confirmationsProgress = confirmationsProgress ? confirmationsProgress : "";
                var dappName = model.getRoleValue(row, "dappName");
                txDetails.dappName = dappName ? dappName : "";

                var addrModel = {
                    isMaxPrivacy: model.getRoleValue(row, "isMaxPrivacy"),
                    isOfflineToken: model.getRoleValue(row, "isOfflineToken"),
                    isPublicOffline: model.getRoleValue(row, "isPublicOffline")
                };
                txDetails.addressType = Utils.getAddrTypeFromModel(addrModel);

                var assetNames = model.getRoleValue(row, "assetNames");
                txDetails.assetNames = assetNames ? assetNames : [];
                var assetIcons = model.getRoleValue(row, "assetIcons");
                txDetails.assetIcons = assetIcons ? assetIcons: [];
                var assetAmounts = model.getRoleValue(row, "assetAmounts");
                txDetails.assetAmounts = assetAmounts ? assetAmounts: [];
                var assetAmountsIncome = model.getRoleValue(row, "assetAmountsIncome");
                txDetails.assetIncome = assetAmountsIncome ? assetAmountsIncome: [];
                var assetRates = model.getRoleValue(row, "assetRates");
                txDetails.assetRates = assetRates ? assetRates: [];
                var assetIDs = model.getRoleValue(row, "assetIDs");
                txDetails.assetIDs = assetIDs ? assetIDs: [];
                var amountSecondCurrency = model.getRoleValue(row, "amountSecondCurrency");
                txDetails.totalValue = amountSecondCurrency ? amountSecondCurrency : "0";
                txDetails.rateUnit = tableViewModel.rateUnit;

                var fee = model.getRoleValue(row, "fee");
                txDetails.fee = fee ? fee : "0";
                var feeRate = model.getRoleValue(row, "feeRate");
                txDetails.feeRate = feeRate ? feeRate : "0";
                txDetails.feeUnit = qsTrId("general-beam");
                txDetails.feeRateUnit = tableViewModel.rateUnit;

                txDetails.searchFilter = searchBox.text;
                var addressFrom = model.getRoleValue(row, "token");
                txDetails.token = token ? token : "";
                var isShieldedTx = model.getRoleValue(row, "isShieldedTx");
                txDetails.isShieldedTx = isShieldedTx;
                txDetails.initialState = "tx_info";

                txDetails.getPaymentProof = function(rawTxID) {
                    return rawTxID ? tableViewModel.getPaymentInfo(rawTxID) : null;
                }
            }

            Component.onCompleted: function () {
                txProxyModel.source.countChanged.connect(function() {
                    control.activeTxCnt = 0
                    var source = txProxyModel.source
                    for (var idx = 0; idx < source.count; ++idx) {
                        var qindex = source.index(idx, 0);
                        if (source.data(qindex, TxObjectList.Roles.IsActive)) {
                            ++control.activeTxCnt
                        }
                    }
                })
                transactionsTable.model.modelReset.connect(function() {
                    if (root && root.openedTxID != "") {
                        var index = tableViewModel.transactions.index(0, 0);
                        var indexList = tableViewModel.transactions.match(index, TxObjectList.Roles.TxID, root.openedTxID);
                        if (indexList.length > 0) {
                            index = dappFilterProxy.mapFromSource(indexList[0]);
                            index = assetFilterProxy.mapFromSource(index);
                            index = searchProxyModel.mapFromSource(index);
                            index = txProxyModel.mapFromSource(index);
                            transactionsTable.positionViewAtRow(index.row, ListView.Beginning);

                            initTxDetailsFromRow(transactionsTable.model, index.row);
                            txDetails.open();
                        }
                    }
                });

                root.onOpenedTxIDChanged.connect(function() {
                    var index = tableViewModel.transactions.index(0, 0);
                    var indexList = tableViewModel.transactions.match(index, TxObjectList.Roles.TxID, root.openedTxID);
                    initTxDetailsFromRow(transactionsTable.model, index.row);
                    txDetails.open();
                })
            }

            Layout.alignment:     Qt.AlignTop
            Layout.fillWidth:     true
            Layout.fillHeight:    true
            Layout.bottomMargin:  9
            visible:              transactionsTable.model.count > 0
            enableOwnMouseArea:   false

            property real rowHeight: 56
            property real resizableWidth: transactionsTable.width - 140
            property real columnResizeRatio: resizableWidth / (610 - (sourceVisible ? 0 : 140))

            selectionMode: SelectionMode.NoSelection
            sortIndicatorVisible: true
            sortIndicatorColumn: 4
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
                        id:           assetFilterProxy
                        filterRole:   "assetFilter"
                        filterString: control.selectedAsset < 0 ? "" : ["\\b", control.selectedAsset, "\\b"].join("")
                        filterSyntax: SortFilterProxyModel.RegExp

                        source: SortFilterProxyModel {
                            id:           dappFilterProxy
                            filterRole:   dappFilter ? (dappFilter == "all" ? "isDappTx" : "dappId") : ""
                            filterString: dappFilter ? (dappFilter == "all" ? "true" : dappFilter) : ""
                            filterSyntax: SortFilterProxyModel.FixedString
                            filterCaseSensitivity: Qt.CaseInsensitive
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
                id:         rowItemDelegate
                collapsed:  true
                rowInModel: styleData.row !== undefined && styleData.row >= 0 && styleData.row < txProxyModel.count
                rowHeight:  transactionsTable.rowHeight
                tableView:  transactionsTable

                backgroundColor: !rowInModel ? "transparent":
                                 styleData.selected ?
                                 Style.row_selected :
                                 hovered 
                                    ? Qt.rgba(Style.active.r, Style.active.g, Style.active.b, 0.1)
                                    : (styleData.alternate ? (!collapsed || animating ? Style.background_row_details_even : Style.background_row_even)
                                                           : (!collapsed || animating ? Style.background_row_details_odd : Style.background_row_odd))

                property var model: parent.model
                property bool hideFiltered: true
                property string searchBoxText: searchBox.text
                property int rowNumber: styleData.row !== undefined? styleData.row : "-1"
                onSearchBoxTextChanged: function() {
                    if (searchBoxText.length == 0) {
                        collapse(false);
                        return;
                    }
                    if (rowNumber == 0 && collapsed)
                        expand(false);

                    if (rowNumber != 0 && !collapsed)
                        collapse(false);
                }

                onHoveredChanged: function() {
                    transactionsTable.containsMouse = hovered;
                }

                onRowNumberChanged: function() {
                    if(searchBoxText.length && rowNumber == 0 && collapsed)
                        expand(false);
                }

                onLeftClick: function() {
                    transactionsTable.initTxDetailsFromRow(transactionsTable.model, rowNumber);
                    txDetails.open();
                    return false;
                }

                delegate: TransactionsSearchHighlighter {
                    id: detailsPanel
                    width: transactionsTable.width
                    sendAddress:            model && model.addressFrom ? model.addressFrom : ""
                    receiveAddress:         model && model.addressTo ? model.addressTo : ""
                    senderIdentity:         model && model.senderIdentity ? model.senderIdentity : ""
                    receiverIdentity:       model && model.receiverIdentity ? model.receiverIdentity : ""
                    txID:                   model && model.txID ? model.txID : ""
                    kernelID:               model && model.kernelID ? model.kernelID : ""

                    searchFilter:    searchBoxText
                    hideFiltered:    rowItemDelegate.hideFiltered
                    token:           model ? model.token : ""

                    onTextCopied: function (text) {
                        BeamGlobals.copyToClipboard(text);
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
                title:     qsTrId("general-coin")
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
                role: "amountGeneral"

                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     130 * transactionsTable.columnResizeRatio
                movable:   false
                resizable: false

                delegate: Item { RowLayout {
                    width:  parent.width
                    height: transactionsTable.rowHeight

                    property var isIncome:    model && model.isIncome
                    property var prefix:      model && model.amountGeneral == "0" ? "" : (isIncome ? "+ " : "- ")
                    property var amountText:  model && model.amountGeneral ? [prefix, Utils.uiStringToLocale(BeamGlobals.roundWithPrecision(model.amountGeneral, 6))].join('') : "0"

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

                title:     [tableViewModel.rateUnit || "USD",
                            //% "Value"
                            qsTrId("general-value")].join(' ')

                elideMode: Text.ElideRight
                width:     130 * transactionsTable.columnResizeRatio
                movable:   false
                resizable: false
                visible:   !control.isContracts

                delegate: Item { RowLayout {
                    width:  parent.width
                    height: transactionsTable.rowHeight

                    property var amount: BeamGlobals.roundWithPrecision(model && model.amountSecondCurrency ? model.amountSecondCurrency : "0", tableViewModel.rateUnit ? 6 : 2)
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
                role: "source"
                id: sourceColumn

                //% "Source"
                title:      qsTrId("wallet-txs-source")
                elideMode:  Text.ElideRight
                width:      140 * transactionsTable.columnResizeRatio
                movable:    false
                resizable:  false
                visible:    sourceVisible
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
                                text: styleData && styleData.value ? styleData.value : ""
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
                if (transactionsTable.model.getRoleValue(row, "isContractTx"))
                {
                    contractTxContextMenu.deleteEnabled = transactionsTable.model.getRoleValue(row, "isDeleteAvailable");
                    contractTxContextMenu.txID = transactionsTable.model.getRoleValue(row, "rawTxID");
                    contractTxContextMenu.popup();
                }
                else
                {
                    txContextMenu.cancelEnabled = transactionsTable.model.getRoleValue(row, "isCancelAvailable");
                    txContextMenu.deleteEnabled = transactionsTable.model.getRoleValue(row, "isDeleteAvailable");
                    txContextMenu.txID = transactionsTable.model.getRoleValue(row, "rawTxID");
                    txContextMenu.popup();
                }
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

            function showDeleteTransactionDialog(txId) {
                //% "The transaction will be deleted. This operation can not be undone"
                deleteTransactionDialog.text = qsTrId("wallet-txs-delete-message");
                deleteTransactionDialog.txID = txId;
                deleteTransactionDialog.open();
            }

            ContextMenu {
                id: txContextMenu
                modal: true
                dim: false
                property bool cancelEnabled
                property bool deleteEnabled
                property var txID

                Action {
                    //% "Show details"
                    text: qsTrId("general-show-tx-details")
                    icon.source: "qrc:/assets/icon-show_tx_details.svg"
                    onTriggered: {
                        var id = txContextMenu.txID;
                        if (typeof id != "string") {
                            id = BeamGlobals.rawTxIdToStr(id);
                        }
                        root.openedTxID = id;
                    }
                }

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
                        transactionsTable.showDeleteTransactionDialog(txContextMenu.txID);
                    }
                }
            }

            ContextMenu {
                id: contractTxContextMenu
                modal: true
                dim: false
                property bool deleteEnabled
                property var txID

                Action {
                    //% "Show details"
                    text: qsTrId("general-show-tx-details")
                    icon.source: "qrc:/assets/icon-show_tx_details.svg"
                    onTriggered: {
                        var id = contractTxContextMenu.txID;
                        if (typeof id != "string") {
                            id = BeamGlobals.rawTxIdToStr(id);
                        }
                        root.openedTxID = id;
                    }
                }

                Action {
                    //% "Delete"
                    text: qsTrId("general-delete")
                    icon.source: "qrc:/assets/icon-delete.svg"
                    enabled: contractTxContextMenu.deleteEnabled
                    onTriggered: {
                        transactionsTable.showDeleteTransactionDialog(contractTxContextMenu.txID);
                    }
                }
            }
        }
    }
}
