import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.15
import "../controls"
import "../wallet"

Control {
    id: appInfoControl
    property string dappName
    property string dappFilter
    property bool  showBalance:       true
    property bool  folded:            true
    property bool  foldsUp:           true
    property alias bkColor:           appBackground.color
    property int   contentItemHeight: 0
    property alias tableOwner:        txTable.owner
    property bool  maximized:         false
    horizontalPadding: 2

    function showTxDetails(id) {
        txTable.showTxDetails(id)
    }

    //% "(%1 active)"
    property string txTip: txTable.activeTxCnt ? qsTrId("apps-inprogress-tip").arg(txTable.activeTxCnt) : ""
    spacing: 0
    padding: 20

    state: "transactions"
    states: [
        State {
            name: "balance"
            PropertyChanges { target: balanceTab; state: "active" }
            PropertyChanges { target: assetsList; visible: true }
        },
        State {
            name: "transactions"
            PropertyChanges { target: txsTab; state: "active" }
            PropertyChanges { target: txTable; visible: true }
        }
    ]

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            id: headerRow
            Layout.fillWidth: true
            spacing: 0
            Layout.leftMargin: 20
            Layout.rightMargin: 20

            component AppInfoTabButton : TabButton {
                opacity:    (folded || this.state != "active") ? 0.5 : 1
                activeColor: folded ? Style.content_main : Style.active
            }

            AppInfoTabButton {
                id: balanceTab
                //% "Wallet Balance"
                label: qsTrId("wallet-balance-title")
                Layout.alignment:   Qt.AlignVCenter
                Layout.rightMargin: 20

                onClicked: function () {
                    if (appInfoControl.folded || appInfoControl.state == "balance")
                        appInfoControl.folded = !appInfoControl.folded
                    appInfoControl.state = "balance"
                }
                visible:    showBalance
            }

            AppInfoTabButton {
                id: txsTab
                label: (dappName ? dappName + " " : "") + qsTrId("wallet-transactions-title")
                Layout.alignment: Qt.AlignVCenter

                onClicked: function () {
                    if (appInfoControl.folded || appInfoControl.state == "transactions")
                        appInfoControl.folded = !appInfoControl.folded
                    appInfoControl.state = "transactions"
                }
            }

            SFText {
                color:              Style.content_main
                text:               txTip
                visible:            txTip.length != 0
                Layout.alignment:   Qt.AlignVCenter
                Layout.leftMargin:  5
                font {
                    styleName:      "Bold"
                    weight:         Font.Bold
                    pixelSize:      14
                    letterSpacing:  0.35
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape:  Qt.PointingHandCursor
                    onClicked: {
                        appInfoControl.state = "transactions"
                        if (appInfoControl.folded)
                            appInfoControl.folded = !appInfoControl.folded
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth:  true
            }

            SvgImage {
                id: arrowSec
                Layout.alignment:       Qt.AlignCenter
                Layout.maximumHeight:   8
                Layout.maximumWidth:    13
                Layout.leftMargin:      7
                visible:                !appInfoControl.folded && !appInfoControl.maximized
                source:                 "qrc:/assets/icon-grey-arrow-up.svg"
                transform: Rotation {
                    angle: foldsUp ? 180 : 0
                    origin.x: arrowSec.width/2
                    origin.y: arrowSec.height/2
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape:  Qt.PointingHandCursor
                    onClicked: {
                        appInfoControl.maximized = true
                    }
                }
            }

            Item {
                width: 7
                height: 1
            }

            SvgImage {
                id: arrow
                Layout.alignment:       Qt.AlignCenter
                Layout.maximumHeight:   8
                Layout.maximumWidth:    13
                source:                 appInfoControl.folded ? "qrc:/assets/icon-grey-arrow-down.svg" : "qrc:/assets/icon-grey-arrow-up.svg"
                transform: Rotation {
                    angle: foldsUp ? 0 : 180
                    origin.x: arrow.width/2
                    origin.y: arrow.height/2
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape:  Qt.PointingHandCursor
                    onClicked: {
                        appInfoControl.folded = !appInfoControl.folded
                        if (appInfoControl.maximized) {
                            appInfoControl.maximized = false;
                        }
                    }
                }
            }
        }

        RowLayout {
            id: contentRow
            Layout.fillWidth:       true
            Layout.topMargin:       folded ? 0 : 20
            Layout.leftMargin:      20
            Layout.rightMargin:     20
            Layout.alignment:       Qt.AlignTop
            opacity:                folded ? 0.0 : 1.0

            Item {
                Layout.fillWidth:       true
                Layout.fillHeight:      true
                TxTable {
                    id: txTable
                    anchors.fill:           parent
                    owner:                  appInfoControl
                    emptyMessageMargin:     60
                    mainBackgroundRect:     appBackground
                    dappFilter:             appInfoControl.dappFilter
                    visible:                false
                }
                AssetsPanel {
                    id:             assetsList
                    anchors.fill:   parent
                    visible:        false
                }
            }

            Layout.preferredHeight: folded ? 0 : appInfoControl.contentItemHeight

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration:  100 }
            }
            Behavior on Layout.topMargin {
                NumberAnimation { duration:  100 }
            }
            Behavior on opacity {
                NumberAnimation { duration:  200 }
            }
        }
    }

    background: Rectangle {
        id:      appBackground
        radius:  10
        color:   Style.background_second
    }
}
