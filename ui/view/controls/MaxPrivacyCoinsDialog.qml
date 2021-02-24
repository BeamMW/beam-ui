import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

Dialog {
    id: dialog
    modal: true
    property string locked
    property string secondCurrencyLabel
    property string secondCurrencyRateValue

    UtxoViewModel {
        id: viewModel
        maturingMaxPrivacy: true
    }
    
    width:          460
    height:         610
    parent:         Overlay.overlay
    padding:        30
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    background: Rectangle {
        radius: 10
        color: Style.background_popup
        anchors.fill: parent            
    }

    onClosed : {
        tableView.sortIndicatorOrder = Qt.AscendingOrder;
        tableView.sortIndicatorColumn = 1;
    }

    contentItem: ColumnLayout {
        Layout.fillWidth: true
        
        spacing: 20

        SFText {
            //% "Max privacy"
            text: qsTrId("max-privacy-dialog-header")
            Layout.alignment:   Qt.AlignHCenter
            font.pixelSize: 18
            font.styleName: "Bold"; font.weight: Font.Bold
            color: Style.content_main
        }

        RowLayout {
            Layout.fillWidth: true

            SFText {
                id:             maxPrivacyLabel
                font.pixelSize: 12
                font.styleName: "Regular"
                font.weight:    Font.Normal
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Locked"
                text:           qsTrId("max-privacy-dialog-locked") + ":"
                Layout.alignment: Qt.AlignTop
            }

            BeamAmount {
                Layout.leftMargin: 24
                amount:            locked
                //unitName:          control.totals.unitName
                //rateUnit:          control.totals.rateUnit
                //rate:              control.totals.rate
                spacing:           15
                font.styleName:    "Bold"
                font.weight:       Font.Bold
                font.pixelSize:    12
            }
        }

        CustomTableView {
            id: tableView
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.fillHeight: true
            frameVisible: false
            selectionMode: SelectionMode.NoSelection
            backgroundVisible: false
            sortIndicatorVisible: true
            sortIndicatorColumn: 1
            sortIndicatorOrder: Qt.AscendingOrder
            headerColor: Style.background_main_top
            mainBackgroundRect: dialog.background 
            model: SortFilterProxyModel {
                sortOrder: tableView.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortRole: tableView.getColumn(tableView.sortIndicatorColumn).role + "Sort"
                source: viewModel.allUtxos
                filterSyntax: SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
            }

            TableViewColumn {
                role: "amount"
                //% "Amount"
                title: qsTrId("max-privacy-dialog-amount")
                width: contentItem.width / 2
                resizable: false
                movable: false
            }

            TableViewColumn {
                role: "maturityTimeLeft"
                //% "Latest unlocked time"
                title: qsTrId("max-privacy-dialog-unlock-time")
                width: contentItem.width / 2
                resizable: false
                movable: false
                delegate: TableItem {
                    text:           Utils.formatHours(styleData.value)
                    elide:          styleData.elideMode
                }
            }

            rowDelegate: Item {
                height: 56
                anchors.left: parent.left
                anchors.right: parent.right

                Rectangle {
                    anchors.fill: parent
                    color: styleData.selected ? Style.row_selected :
                            (styleData.alternate ? Style.background_row_even : Style.background_row_odd)
                }
            }

            itemDelegate: TableItem {
                text: styleData.value
                elide: Text.ElideRight
            }
        }

        CustomButton {
            Layout.alignment:   Qt.AlignHCenter
            icon.source:    "qrc:/assets/icon-cancel-16.svg"
            //% "Close"
            text:           qsTrId("general-close")
            onClicked: {
                dialog.close()
            }
        }
    }
}
