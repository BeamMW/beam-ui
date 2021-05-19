import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.2
import "controls"
import Beam.Wallet 1.0;

ColumnLayout {
    id:             root
    anchors.fill:  parent
    UtxoViewModel {
        id: viewModel
    }

    Title {
        //% "UTXO"
        text: qsTrId("utxo-utxo")
    }

    StatusBar {
        id: status_bar
        model: statusbarModel
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        Layout.bottomMargin: 10

        height: 80
        spacing: 10

        Item {
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            height: parent.height

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.topMargin: 10
                anchors.bottomMargin: 20
                spacing: 5

                SFText {
                    Layout.minimumHeight: 20
                    Layout.maximumHeight: 20
                    font.pixelSize: 18
                    font.styleName: "Bold"; font.weight: Font.Bold
                    color: Style.content_main
                    //% "Blockchain Height"
                    text: qsTrId("utxo-blockchain-height")
                }

                SFText {
                    Layout.minimumHeight: 20
                    Layout.maximumHeight: 20
                    font.pixelSize: 16
                    color: Style.active
                    text: viewModel.currentHeight
                }
            }

            Rectangle {
                anchors.fill: parent
                radius: 10
                color: Style.white
                opacity: 0.1
            }
        }

        Item {
            Layout.fillWidth: true
            height: parent.height

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottomMargin: 20
                spacing: 5

                SFText {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 20
                    Layout.maximumHeight: 20
                    font.pixelSize: 18
                    font.styleName: "Bold"; font.weight: Font.Bold
                    color: Style.content_main
                    //% "Last block hash"
                    text: qsTrId("utxo-last-block-hash")
                }

                SFText {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 20
                    Layout.maximumHeight: 20
                    font.pixelSize: 16
                    color: Style.active
                    text: viewModel.currentStateHash
                    elide: Text.ElideRight
                }
            }

            Rectangle {
                anchors.fill: parent
                radius: 10
                color: Style.white
                opacity: 0.1
            }
        }
    }

    //RowLayout {
    //    Layout.alignment: Qt.AlignTop
    //    Layout.fillWidth: true
    //    Layout.topMargin: 30
    //    Layout.preferredHeight: 32
    //    Layout.bottomMargin: 10
    //
    //    TxFilter {
    //        id: regularTabSelector
    //        Layout.alignment: Qt.AlignVCenter
    //        //% "Regular"
    //        label: qsTrId("wallet-utxo-regular-tab")
    //        onClicked: root.state = "all"
    //        capitalization: Font.AllUppercase
    //    }
    //    TxFilter {
    //        id: shieldedTabSelector
    //        Layout.alignment: Qt.AlignVCenter
    //        //% "Shielded"
    //        label: qsTrId("wallet-utxo-shielded-tab")
    //        onClicked: root.state = "shielded"
    //        capitalization: Font.AllUppercase
    //    }
    //}
    //state: "all"
    //states: [
    //    State {
    //        name: "all"
    //        PropertyChanges { target: regularTabSelector; state: "active" }
    //        //PropertyChanges { target: txProxyModel; filterRole: "status" }
    //        //PropertyChanges { target: txProxyModel; filterString: "*" }
    //    },
    //    State {
    //        name: "shielded"
    //        PropertyChanges { target: shieldedTabSelector; state: "active" }
    //    }
    //]


    CustomTableView {
        id: tableView
        property int rowHeight: 56
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.bottomMargin: 9
        visible: tableView.model.count > 0
        selectionMode: SelectionMode.NoSelection
        model: SortFilterProxyModel {
            sortOrder: tableView.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortRole: tableView.getColumn(tableView.sortIndicatorColumn).role + "Sort"
            source: viewModel.allUtxos
            filterSyntax: SortFilterProxyModel.Wildcard
            filterCaseSensitivity: Qt.CaseInsensitive
        }
        sortIndicatorVisible: true
        sortIndicatorColumn: 1
        sortIndicatorOrder: Qt.DescendingOrder

        onSortIndicatorColumnChanged: {
            if (sortIndicatorColumn == 1) {
                sortIndicatorOrder = Qt.DescendingOrder;
            } else {
                sortIndicatorOrder = Qt.AscendingOrder;
            }
        }

        property double columnResizeRatio: tableView.width / 800

        TableViewColumn {
            role: "amount"
            //% "Amount"
            title: qsTrId("general-amount")
            width: 300 * tableView.columnResizeRatio
            movable: false

            delegate: RowLayout { BeamAmount {
                Layout.leftMargin: 20
                Layout.fillWidth: true
                amount: model ? model.amount : "0"
                unitName: model ? model.unitName : ""
                showTip: false
            }}
        }

        TableViewColumn {
            role: "maturity"
            //% "Maturity"
            title: qsTrId("utxo-head-maturity")
            width: 150 * tableView.columnResizeRatio
            movable: false
        }

        TableViewColumn {
            role: "status"
            //% "Status"
            title: qsTrId("general-status")
            width: 200 * tableView.columnResizeRatio
            movable: false
            resizable: false
            delegate: Item {
                id: delegate_id
                width: parent.width
                height: tableView.rowHeight
                readonly property var lineSeparator: "\n"
                property var texts: utxoStatusText(styleData.value).split(lineSeparator)
                property color secondLineColor: Style.content_secondary

                ColumnLayout {
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    SFLabel {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.topMargin: secondLineEnabled() ? 0 : 20
                        color: getTextColor(styleData.value)
                        elide: Text.ElideRight
                        text: delegate_id.texts[0]
                        textFormat: Text.StyledText
                        font.italic: true
                        font.pixelSize: 14
                    }

                    SFLabel {
                        Layout.fillWidth: true
                        Layout.fillHeight: secondLineEnabled()
                        enabled: secondLineEnabled()
                        color: delegate_id.secondLineColor
                        elide: Text.ElideRight
                        text: secondLineEnabled() ? delegate_id.texts[1] : ""
                        textFormat: Text.StyledText
                        font.italic: true
                        font.pixelSize: 14
                    }
                }

                function getTextColor(value) {
                    switch(value) {
                        case UtxoStatus.Available:
                            return Style.active;
                        case UtxoStatus.Maturing:
                        case UtxoStatus.MaturingMP:
                        case UtxoStatus.Spent:
                        case UtxoStatus.Outgoing:
                            return Style.accent_outgoing;
                        case UtxoStatus.Incoming:
                            return (model && model.type == UtxoType.Change) ?
                                Style.accent_outgoing :
                                Style.accent_incoming;
                        case UtxoStatus.Unavailable:
                        default:
                            return Style.content_main;
                    }
                }

                function secondLineEnabled() {
                    return delegate_id.texts[1] !== undefined;
                }

                function utxoStatusText(value) {
                    switch(value) {
                        case UtxoStatus.Available:
                            //% "Available"
                            return qsTrId("utxo-status-available");
                        case UtxoStatus.Maturing:
                            //% "Maturing%1(till block height %2)"
                            return qsTrId("utxo-status-maturing").arg(lineSeparator).arg(model ? model.maturity : "?");
                        case UtxoStatus.Unavailable:
                            //% "Unavailable%1(mining result rollback)"
                            return qsTrId("utxo-status-unavailable").arg(lineSeparator);
                        case UtxoStatus.Outgoing:
                            //% "In progress%1(outgoing)"
                            return qsTrId("utxo-status-outgoing").arg(lineSeparator);
                        case UtxoStatus.Incoming:
                            return (model && model.type == UtxoType.Change) ?
                                //% "In progress%1(change)"
                                qsTrId("utxo-status-change").arg(lineSeparator) :
                                //% "In progress%1(incoming)"
                                qsTrId("utxo-status-incoming").arg(lineSeparator);
                        case UtxoStatus.Spent:
                            //% "Spent"
                            return qsTrId("utxo-status-spent");
                        case UtxoStatus.MaturingMP:
                            //% "Maturing%1(max privacy %2% unlinked)"
                            return qsTrId("utxo-status-maturing-mp").arg(lineSeparator).arg(model ? model.maturityPercentage : "?");
                        default:
                            return "";
                    }
                }
            }
        }


        TableViewColumn {
            id: typeColumn
            role: "type"
            //% "Type"
            title: qsTrId("utxo-head-type")
            width: tableView.getAdjustedColumnWidth(typeColumn)//150 * columnResizeRatio
            movable: false
            delegate: Item {
                id: utxoTypeDelegate
                width: parent.width
                height: tableView.rowHeight
                property var utxoType: utxoTypeText(styleData.value)

                ColumnLayout {
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    SFLabel {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Style.content_main
                        elide: Text.ElideRight
                        text: utxoTypeDelegate.utxoType
                        textFormat: Text.StyledText
                        font.pixelSize: 14
                    }
                }

                function utxoTypeText(value) {
                    switch(value) {
                        //% "Transaction fee"
                        //: UTXO type fee
                        case UtxoType.Comission: return qsTrId("general-fee");
                        //% "Coinbase"
                        //: UTXO type Coinbase
                        case UtxoType.Coinbase: return qsTrId("general-coinbase");
                        //% "Regular"
                        //: UTXO type Regular
                        case UtxoType.Regular: return qsTrId("general-regular");
                        //% "Change"
                        //: UTXO type Change
                        case UtxoType.Change: return qsTrId("general-change");
                        //% "Treasury"
                        //: UTXO type Treasury
                        case UtxoType.Treasury: return qsTrId("general-treasury");
                        //% "Shielded"
                        //: UTXO type Shielded
                        case UtxoType.Shielded: return qsTrId("general-shielded");
                        default : return "";
                    }
                }
            }
        }

        rowDelegate: Item {

            height: tableView.rowHeight

            anchors.left: parent.left
            anchors.right: parent.right

            Rectangle {
                anchors.fill: parent
                color: styleData.alternate ? Style.background_row_even : Style.background_row_odd
            }
        }

        itemDelegate: TableItem {
            text: styleData.value
            elide: Text.ElideRight
        }
    }
    ColumnLayout {
        Layout.topMargin: 70
        Layout.alignment: Qt.AlignHCenter
        visible: tableView.model.count == 0

        SvgImage {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/assets/icon-utxo-empty.svg"
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
            //% "Your UTXO list is empty"
            text: qsTrId("utxo-empty")
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
