import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.2
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: "dialog"

    modal: true
    width: 760
    height: 650
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: Overlay.overlay
    padding: 30

    header: ColumnLayout {
        SFText {
            Layout.topMargin: 50
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize: 18
            font.styleName: "Bold";
            font.weight: Font.Bold
            color: Style.content_main
            //% "UTXO"
            text: qsTrId("utxo-utxo")
        }
    }

    contentItem: ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        UtxoViewModel {
            id: viewModel
        }

        CustomTableView {
            id: tableView
            property int rowHeight: 72
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.bottomMargin: 9
            visible: tableView.model.count > 0
            selectionMode: SelectionMode.NoSelection
            backgroundVisible: false
            headerColor: Qt.rgba(Style.active.r, Style.active.g, Style.active.b, 0.1)
            mainBackgroundRect: dialog.background
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
                //% "Coin"
                title: qsTrId("general-coin")
                width: 300 * tableView.columnResizeRatio
                movable: false

                delegate: RowLayout {
                    BeamAmount {
                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        amount: model ? model.amount : "0"
                        unitName: model ? model.unitName : ""
                        iconSource: model ? model.iconSource : ""
                        verified: model ? model.verified : false
                        showTip: false
                        maxUnitChars: 18
                        maxPaintedWidth: false
                    }
                }
            }

            TableViewColumn {
                role: "maturity"
                //% "Maturity"
                title: qsTrId("utxo-head-maturity")
                width: 150 * tableView.columnResizeRatio
                movable: false
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
                        anchors.leftMargin: 20

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
                        anchors.leftMargin: 20

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
                            font.capitalization: Font.AllLowercase
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
                                return Style.content_main
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


    footer: ColumnLayout {
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 50
            spacing: 20

            CustomButton {
                //% "Close"
                text:               qsTrId("general-close")
                icon.source:        "qrc:/assets/icon-cancel-16.svg"
                onClicked:          dialog.close()
            }
        }
    }

}
