import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import "../controls"
import "../wallet"

Pane {
    id: control
    property string dappName
    property string dappFilter

    property bool  folded:            true
    property bool  foldsUp:           true
    property alias bkColor:           background.color
    property int   contentItemHeight: 0
    property alias tableOwner:        txTable.owner
    horizontalPadding: 2

    property var content: Item {
        TxTable {
            property bool isTransactionsView: control.state == "transactions"
            anchors.leftMargin: isTransactionsView ? 25 : 0
            anchors.rightMargin: isTransactionsView ? 25 : 0
            id: txTable
            owner: control
            emptyMessageMargin: 60
            headerShaderVisible: false
            dappFilter: control.dappFilter
            anchors.fill: parent
            visible: isTransactionsView
        }
        AssetsPanel {
            id: assetsList
            anchors.fill: parent
            visible: control.state == "balance"
        }
    }

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
        },
        State {
            name: "transactions"
            PropertyChanges { target: txsTab; state: "active" }
        }
    ]

    contentItem: ColumnLayout {
        spacing: 0
        clip:    folded
        width: parent.width

        RowLayout {
            id: headerRow
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            spacing: 0
            Layout.leftMargin: 25
            Layout.rightMargin: 25

            TxFilter {
                id: balanceTab
                //% "Wallet Balance"
                label: qsTrId("wallet-balance-title")
                Layout.alignment: Qt.AlignVCenter

                onClicked: function () {
                    if (control.folded || control.state == "balance") control.folded = !control.folded
                    control.state = "balance"
                }

                showLed: false
                opacity: (folded || this.state != "active") ? 0.5 : 1
                activeColor: folded ? Style.content_main : Style.active
                inactiveColor: Style.content_main

                font {
                    styleName:      "Bold"
                    weight:         Font.Bold
                    pixelSize:      14
                    letterSpacing:  3.11
                    capitalization: Font.AllUppercase
                }
            }

            Item {
                width: 25
                Layout.fillHeight: true

                MouseArea {
                    anchors.fill: parent
                    cursorShape:  folded ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: function () {
                        if (control.folded) control.folded = !control.folded
                    }
                }
            }

            TxFilter {
                id: txsTab
                label: (dappName ? dappName + " " : "") + qsTrId("wallet-transactions-title")
                Layout.alignment: Qt.AlignVCenter

                onClicked: function () {
                    if (control.folded || control.state == "transactions") control.folded = !control.folded
                    control.state = "transactions"
                }

                showLed: false
                opacity: (folded || this.state != "active") ? 0.5 : 1
                activeColor: folded ? Style.content_main : Style.active
                inactiveColor: Style.content_main

                font {
                    styleName:      "Bold"
                    weight:         Font.Bold
                    pixelSize:      14
                    letterSpacing:  3.11
                    capitalization: Font.AllUppercase
                }
            }

            Item {
                width: 5
                Layout.fillHeight: true
                visible: txTip.length != 0
            }

            SFText {
                color:   Style.content_main
                text:    txTip
                visible: txTip.length != 0
                Layout.alignment: Qt.AlignVCenter

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
                        control.state = "transactions"
                        if (control.folded) control.folded = !control.folded
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth:  true
            }

            SvgImage {
                id: arrow
                Layout.alignment:       Qt.AlignCenter
                Layout.maximumHeight:   8
                Layout.maximumWidth:    13
                source:                 control.folded ? "qrc:/assets/icon-grey-arrow-down.svg" : "qrc:/assets/icon-grey-arrow-up.svg"
                transform: Rotation {
                    angle: foldsUp ? 0 : 180
                    origin.x: arrow.width/2
                    origin.y: arrow.height/2
                }
            }
        }

        Control {
            id:                     placeholder
            Layout.fillWidth:       true
            Layout.topMargin:       folded ? 0 : 20
            Layout.alignment:       Qt.AlignTop
            contentItem:            control.content

            opacity:                folded ? 0.0 : 1.0
            Layout.preferredHeight: folded ? 0 : (control.state == "transactions" 
                                                    ? control.contentItemHeight
                                                    : assetsList.scrollViewHeight
                                                      + assetsList.assetsFilterRowHeight)

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
        id:      background
        radius:  10
        color:   Style.background_second

        MouseArea {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height:  control.topPadding + headerRow.height +
                    ( control.folded ? control.bottomPadding : 0 ) +
                    ( placeholder.visible ? placeholder.Layout.topMargin : 0 )

            cursorShape:  Qt.PointingHandCursor
            onClicked: {
                control.folded = !control.folded
            }
        }
    }
}
