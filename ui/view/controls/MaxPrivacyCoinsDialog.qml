import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Dialog {
    id: dialog

    property string locked
    property string secondCurrencyLabel
    property string secondCurrencyRateValue

    UtxoViewModel {
        id: viewModel
    }
    
    width: 460
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    background: Rectangle {
        radius: 10
        color: Style.background_popup
        anchors.fill: parent            
    }

    header: SFText {
        //% "Max privacy"
        text: qsTrId("max-privacy-dialog-header")
        topPadding: 30
        horizontalAlignment : Text.AlignHCenter
        font.pixelSize: 18
        font.styleName: "Bold"; font.weight: Font.Bold
        color: Style.content_main
    }

    contentItem: ColumnLayout {
        Layout.fillWidth: true

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 24
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.bottomMargin: 20
            Layout.maximumHeight: 40

            SFText {
                id:             maxPrivacyLabel
                font.pixelSize: 12
                font.styleName: "Light"
                font.weight:    Font.Light
                color:          Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                //% "Locked"
                text:           qsTrId("max-privacy-dialog-locked") + ":"
                Layout.alignment: Qt.AlignTop
            }

            BeamAmount {
                Layout.leftMargin: 24
                amount:            locked
                currencySymbol:    BeamGlobals.getCurrencyLabel(Currency.CurrBeam)
                secondCurrencyLabel:        dialog.secondCurrencyLabel
                secondCurrencyRateValue:    dialog.secondCurrencyRateValue
                spacing:           15
                lightFont:         false
                fontSize:          12
            }
        }

        ColumnLayout {
            id: tableColumn
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.preferredHeight: 375

            Rectangle {
                id: selectBackground
                color: Style.background_popup
                Layout.fillWidth: true
                Layout.fillHeight: true
                // anchors.fill: parent
            }

            CustomTableView {
                id: rootControl
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true
                frameVisible: false
                selectionMode: SelectionMode.NoSelection
                backgroundVisible: false
                sortIndicatorVisible: true
                sortIndicatorColumn: 1
                sortIndicatorOrder: Qt.DescendingOrder
                headerColor: Style.background_main_top
                mainBackgroundRect: selectBackground 
                model: SortFilterProxyModel {
                    sortOrder: tableView.sortIndicatorOrder
                    sortCaseSensitivity: Qt.CaseInsensitive
                    sortRole: tableView.getColumn(tableView.sortIndicatorColumn).role + "Sort"
                    source: viewModel.mpMaturingUtxos
                    filterSyntax: SortFilterProxyModel.Wildcard
                    filterCaseSensitivity: Qt.CaseInsensitive
                }

                TableViewColumn {
                    role: "amount"
                    //% "Amount"
                    title: qsTrId("max-privacy-dialog-amount")
                    width: tableColumn.width / 2
                    resizable: false
                    movable: false
                }

                TableViewColumn {
                    role: "maturityPercentage"
                    //% "Latest unlocked time"
                    title: qsTrId("max-privacy-dialog-unlock-time")
                    width: tableColumn.width / 2
                    resizable: false
                    movable: false
                    delegate: TableItem {
                       text:           styleData.value + "%"
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
        }
    }

    footer: RowLayout {
        Layout.fillWidth: true

        CustomButton {
            Layout.alignment:   Qt.AlignHCenter
            Layout.preferredWidth: 125
            Layout.bottomMargin: 30
            Layout.topMargin: 10
            icon.source:    "qrc:/assets/icon-cancel-16.svg"
            //% "Close"
            text:           qsTrId("general-close")
            onClicked: {
                dialog.close()
            }
        }
    }
}
