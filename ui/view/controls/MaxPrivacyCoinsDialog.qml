import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.2
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

CustomDialog {
    id: dialog
    modal: true

    property string unitName
    property string lockedAmount
    property int assetId

    UtxoViewModel {
        id: viewModel
        maturingMaxPrivacy: true
        assetId: dialog.assetId
    }
    
    width:          460
    height:         610
    padding:        30
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    onClosed : {
        tableView.sortIndicatorOrder = Qt.AscendingOrder;
        tableView.sortIndicatorColumn = 1;
    }

    contentItem: ColumnLayout {
        spacing: 15

        SFText {
            //% "Locked Max privacy coins"
            text: qsTrId("max-privacy-dialog-header")
            Layout.alignment:   Qt.AlignHCenter
            font.pixelSize: 18
            font.styleName: "Bold"; font.weight: Font.Bold
            color: Style.content_main
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            SFText {
                id:               maxPrivacyLabel
                font.pixelSize:   12
                font.styleName:   "Regular"
                font.weight:      Font.Normal
                color:            Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Locked"
                text:             qsTrId("max-privacy-dialog-locked") + ":"
            }

            BeamAmount {
                Layout.fillWidth:  true
                amount:            dialog.lockedAmount
                unitName:          dialog.unitName
                spacing:           15
                font.styleName:    "Bold"
                font.weight:       Font.Bold
                font.pixelSize:    12
                showTip:           true
                maxUnitChars:      15
                maxPaintedWidth:   true
            }
        }

        CustomTableView {
            id: tableView
            property int rowHeight: 56

            Layout.fillWidth:    true
            Layout.fillHeight:   true
            frameVisible:        false
            selectionMode:       SelectionMode.NoSelection
            backgroundVisible:   false
            headerColor:         Style.background_main_top
            mainBackgroundRect:  dialog.background

            model: SortFilterProxyModel {
                sortOrder:             tableView.sortIndicatorOrder
                sortRole:              tableView.getColumn(tableView.sortIndicatorColumn).role + "Sort"
                source:                viewModel.allUtxos
                sortCaseSensitivity:   Qt.CaseInsensitive
                filterSyntax:          SortFilterProxyModel.Wildcard
                filterCaseSensitivity: Qt.CaseInsensitive
            }

            sortIndicatorVisible: true
            sortIndicatorColumn:  1
            sortIndicatorOrder:   Qt.DescendingOrder

            TableViewColumn {
                role: "amount"
                //% "Amount"
                title:     qsTrId("max-privacy-dialog-amount")
                width:     tableView.width / 2
                resizable: false
                movable:   false

                delegate: RowLayout { BeamAmount {
                    Layout.leftMargin: 20
                    Layout.fillWidth: true
                    amount:   model.amount
                    unitName: model.unitName
                    showTip:  false
                    maxPaintedWidth: true
                    maxUnitChars: 15
                }}
            }

            TableViewColumn {
                id: unlockColumn
                role: "maturityTimeLeft"
                //% "Unlock Time"
                title:     qsTrId("max-privacy-dialog-unlock-time")
                width:     tableView.getAdjustedColumnWidth(unlockColumn)
                resizable: false
                movable:   false

                delegate: TableItem {
                    text:    Utils.formatHours(styleData.value)
                    elide:   styleData.elideMode
                }
            }

            rowDelegate: Rectangle {
                color:          styleData.alternate ? Style.background_row_even : Style.background_row_odd
                height:         tableView.rowHeight
                anchors.left:   parent.left
                anchors.right:  parent.right
            }

            itemDelegate: TableItem {
                text:  styleData.value
                elide: Text.ElideRight
            }
        }

        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            icon.source: "qrc:/assets/icon-cancel-16.svg"
            //% "Close"
            text: qsTrId("general-close")
            onClicked: {
                dialog.close()
            }
        }
    }
}
