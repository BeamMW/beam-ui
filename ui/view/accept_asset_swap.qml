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

    //
    // Title row
    //
    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30

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
                            readOnlyA:    false
                            multi:        false
                            color:        Style.accent_outgoing
                            currColor:    Style.content_main
                            error: {
                                if (!viewModel.isEnough)
                                {
                                    var maxAmount = Utils.uiStringToLocale(viewModel.maxSendAmount)
                                    //% "Insufficient funds to complete the transaction. Maximum amount is %1 %2."
                                    return qsTrId("send-no-funds").arg(maxAmount).arg(Utils.limitText(sendAmountInput.currencyUnit, 10))
                                }
                                return ""
                            }
                        }
                    }

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title:             qsTrId("general-comment")
                        Layout.fillWidth:        true

                        content:
                        ColumnLayout {
                            SFTextInput {
                                id:               commentInput
                                Layout.fillWidth: true
                                font.pixelSize:   14
                                color:            Style.content_main
                                selectByMouse:    true
                                maximumLength:    BeamGlobals.maxCommentLength()
                                //% "Comments are local and won't be shared"
                                placeholderText:  qsTrId("general-comment-local")
                            }

                            Binding {
                                target:   viewModel
                                property: "comment"
                                value:    commentInput.text
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


                                // SFText {
                                //     Layout.alignment:       Qt.AlignTop
                                //     font.pixelSize:         14
                                //     color:                  Style.content_secondary
                                //     //% "Swap token"
                                //     text:                   qsTrId("send-swap-token") + ":"
                                // }
                                // RowLayout {
                                //     Layout.fillWidth:        true
                                //     SFLabel {
                                //         id:                  tokenLabel
                                //         Layout.fillWidth:    true
                                //         font.pixelSize:      14
                                //         color:               Style.content_main
                                //         elide:               Text.ElideMiddle
                                //         text:                viewModel.token
                                //     }
                                
                                //     LinkButton {
                                //         //% "Token details"
                                //         text: qsTrId("swap-token-details")
                                //         linkColor: Style.accent_outgoing
                                //         onClicked: {
                                //             tokenInfoDialog.open();
                                //         }
                                //     }
                                // }

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
                    viewModel.startSwap()
                    thisView.onClosed()
                }
            }  // CustomButton

        }  // ColumnLayout
    } // Scroll
}
