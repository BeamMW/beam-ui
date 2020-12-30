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

    state: "allOffers"
    states: [
        State {
            name: "allOffers"
            PropertyChanges { target: allTab; state: "active" }
           // PropertyChanges { target: txProxyModel; filterRole: "isMy" }
          //  PropertyChanges { target: txProxyModel; filterString: "*" }
        },
        State {
            name: "myOffers"
            PropertyChanges { target: myTab; state: "active" }
            //PropertyChanges { target: txProxyModel; filterRole: "isMy" }
            //PropertyChanges { target: txProxyModel; filterString: "true" }
        },
        State {
            name: "otherOffers"
            PropertyChanges { target: otherTab; state: "active" }
            //PropertyChanges { target: txProxyModel; filterRole: "isMy" }
            //PropertyChanges { target: txProxyModel; filterString: "false" }
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
            id: offersTable
            Layout.fillWidth : true
            Layout.fillHeight : true

            property real rowHeight: 56
            property real resizableWidth: offersTable.width - actionsColumn.width
            property real resizeRatio: resizableWidth / 780

            /*
            TableViewColumn {
                role: "timestamp"
                //% "Date | Time"
                title:     qsTrId("dex-date-time")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }
            */

            TableViewColumn {
                role: "type"
                //% "Type"
                title:     qsTrId("dex-type")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "price"
                //% "Price"
                title:     qsTrId("dex-price")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "size"
                //% "Size"
                title:     qsTrId("dex-size")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "total"
                //% "Total"
                title:     qsTrId("dex-total")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            /*TableViewColumn {
                role: "amountA"
                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "amountB"
                //% "Amount"
                title:     qsTrId("general-amount")
                elideMode: Text.ElideRight
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
                role: "rate"
                //% "Exchange Rate"
                title:     qsTrId("general-rate")
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }
            */

            TableViewColumn {
                role: "expiration"
                //% "Expiration"
                title:     qsTrId("dex-expiration")
                width:     130 * offersTable.resizeRatio
                movable:   false
                resizable: false
            }

            TableViewColumn {
               id:       statusColumn
               role:     "status"
               //% "Status"
               title:     qsTrId("general-status")
               width:     offersTable.getAdjustedColumnWidth(statusColumn)
               movable:   false
               resizable: false
            }

            TableViewColumn {
                id:        actionsColumn
                width:     30
                movable:   false
                resizable: false
            }
        }

        ColumnLayout {
            Layout.topMargin: 100
            Layout.alignment: Qt.AlignHCenter
            visible: false

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
                //% "There are no active offers at the moment.\nPlease try again later or create an offer yourself."
                text: qsTrId("dex-no-offers")
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
