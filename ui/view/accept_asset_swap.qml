import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: thisView

    // callbacks set by parent
    property var onClosed: undefined
    property string orderId: ""

    AssetSwapAcceptViewModel {
        id: viewModel
        orderId: thisView.orderId
    }

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    Title {
        text: qsTrId("assets-swap-title")
    }

    //
    // Subtitle row
    //
    SubtitleRow {
        //% "Accept an Asset Swap Offer"
        text: qsTrId("wallet-accept-asset-swap-title")
        onBack: function () {
            onClosed()
        }
    }

    ScrollView {
        id:                  scrollView
        Layout.fillWidth:    true
        Layout.fillHeight:   true
        Layout.bottomMargin: 10
        clip:                true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy:   ScrollBar.AsNeeded

        ColumnLayout {
            width: scrollView.availableWidth

            //
            // Content row
            //
            RowLayout {
                Layout.fillWidth:   true
                spacing:  10

                //
                // Left column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    //
                    // Send amount
                    //
                    Panel {
                        //% "Send amount"
                        title:                   qsTrId("sent-amount-label")
                        Layout.fillWidth:        true

                        content:
                        AmountInput {
                            id:           sendAmountInput
                            amount:       viewModel.amountToSend
                            currencies:   viewModel.sendCurrencies
                            currencyIdx:  0
                            readOnlyA:    true
                            multi:        false
                            color:        Style.accent_outgoing
                            currColor:    Style.content_main
                            error: {
                                if (!viewModel.isEnough)
                                {
                                    if (!viewModel.isFeeEnough && sendAmountInput.currencyUnit != "BEAM")
                                    {
                                        //% "Insufficient funds to pay transaction fee."
                                        return qsTrId("send-no-funds-for-fee");
                                    }
                                    var maxAmount = Utils.uiStringToLocale(viewModel.maxSendAmount)
                                    //% "Insufficient funds to complete the transaction. Maximum amount is %1 %2."
                                    return qsTrId("send-no-funds").arg(maxAmount).arg(Utils.limitText(sendAmountInput.currencyUnit, 10))
                                }
                                if (viewModel.isAssetsSame)
                                {
                                    //% "You can't accept offer with identical assets on both sides"
                                    return qsTrId("assets-same-accept");
                                }
                                return ""
                            }
                        }
                    }
                }  // ColumnLayout

                //
                // Right column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    //
                    // Receive amount
                    //
                    Panel {
                        //% "Receive amount"
                        title:                   qsTrId("receive-amount-swap-label")
                        Layout.fillWidth:        true
                        content:

                        AmountInput {
                            id:            receiveAmountInput
                            amount:        viewModel.amountToReceive
                            currencies:    viewModel.receiveCurrencies
                            currencyIdx:   0
                            readOnlyA:     true
                            multi:         false
                            color:         Style.accent_incoming
                            currColor:     Style.content_main
                            error: {
                                if (viewModel.isAssetsSame)
                                {
                                    //% "You can't accept offer with identical assets on both sides"
                                    return qsTrId("assets-same-accept");
                                }
                                return ""
                            }
                        }
                    }

                    //
                    // Summary pane
                    //
                    Pane {
                        Layout.fillWidth:        true
                        padding:                 20

                        background: Rectangle {
                            radius: 10
                            color:  Style.background_button
                        }

                        ColumnLayout {
                            anchors.fill:        parent
                            spacing:             20
                            GridLayout {
                                Layout.fillWidth:    true
                                columnSpacing:       20
                                rowSpacing:          20
                                columns:             2

                                property bool showEstimatedFee: true

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Transaction fee"
                                    text:                   qsTrId("asset-swap-fee")
                                }
    
                                SFText {
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.fee
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Offered on"
                                    text:                   qsTrId("wallet-send-swap-offered-label") + ":"
                                }
    
                                SFText {
                                    id:               offered
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.offerCreated
                                }

                                SFText {
                                    id:                     expiresTitle
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Expires on"
                                    text:                   qsTrId("wallet-send-swap-expires-label") + ":"
                                }
                                SFText {
                                    id:               expires
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.offerExpires
                                }

                                SFText {
                                    Layout.alignment:       Qt.AlignTop
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Exchange rate"
                                    text:                   qsTrId("general-rate") + ":"
                                }
            
                                SFText {
                                    id:               rate
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.rate
                                }
                            } // GridLayoyut
                        } // ColumnLayout
                    }
                }  // ColumnLayout
            } // RowLayout

            //
            // Footer
            //
            CustomButton {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                //% "Swap"
                text:                qsTrId("general-swap")
                palette.buttonText:  Style.content_opposite
                palette.button:      Style.accent_outgoing
                icon.source:         "qrc:/assets/icon-create-offer.svg"
                enabled:             viewModel.canAccept
                onClicked: {
                    const dialogComponent = Qt.createComponent("assets_swap_confirm.qml");
                    var dialogObject = dialogComponent.createObject(thisView,
                        {
                            sendAmount: viewModel.amountToSend,
                            sendUnitName: sendAmountInput.currencyUnit,
                            receiveAmount: viewModel.amountToReceive,
                            receiveUnitName: receiveAmountInput.currencyUnit,
                            fee: viewModel.fee,
                        });

                    dialogObject.onAccepted.connect(function () {
                        viewModel.startSwap()
                        thisView.onClosed()
                    });

                    dialogObject.open();
                }
            }  // CustomButton

        }  // ColumnLayout
    } // Scroll
}
