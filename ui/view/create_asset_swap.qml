import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: thisView
    spacing: 0
    // callbacks set by parent
    property var onClosed: undefined

    AssetSwapCreateViewModel {
        id: viewModel
    }

    TopGradient {
        mainRoot: main
        topColor: Style.accent_incoming
    }

    //% "Create Offer"
    property string title: qsTrId("wallet-create-asset-swap-title")

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
                            id:                         sentAmountInput
                            color:                      Style.accent_outgoing
                            currencies:                 viewModel.myCurrenciesList
                            currencyIdx:                viewModel.sendAssetIndex
                            amount:                     viewModel.amountToSend
                            multi:                      true
                            resetAmount:                false
                            currColor:                  Style.content_main
                            error: {
                                if (!viewModel.isEnough)
                                {
                                    var maxAmount = Utils.uiStringToLocale(viewModel.maxSendAmount)
                                    //% "Insufficient funds to complete the transaction. Maximum amount is %1 %2."
                                    return qsTrId("send-no-funds").arg(maxAmount).arg(Utils.limitText(sentAmountInput.currencyUnit, 10))
                                }
                                if (viewModel.isAssetsSame)
                                {
                                    //% "You can't create offer with identical assets on both sides"
                                    return qsTrId("assets-same-create");
                                }
                                return ""
                            }
                        }
                    }

                    Binding {
                        target:   viewModel
                        property: "amountToSend"
                        value:    sentAmountInput.amount
                    }

                    Binding {
                        target:   viewModel
                        property: "sendAssetIndex"
                        value:    sentAmountInput.currencyIdx
                    }

                    //
                    // Offer expiration time
                    //
                    FoldablePanel {
                        //% "Offer expiration time"
                        title:                   qsTrId("wallet-receive-offer-expires-label")
                        Layout.fillWidth:        true
                        folded:                  false
                        content: ColumnLayout {
                            CustomComboBox {
                                id:                  expiresCombo
                                Layout.fillWidth:    true
                                currentIndex:        viewModel.offerExpires
                                fontPixelSize:       14
                                model: [
                                    //% "30 minutes"
                                    qsTrId("wallet-receive-expires-30m"),
                                    //% "1 hour"
                                    qsTrId("wallet-receive-expires-1"),
                                    //% "2 hours"
                                    qsTrId("wallet-receive-expires-2"),
                                    //% "6 hours"
                                    qsTrId("wallet-receive-expires-6"),
                                    //% "12 hours"
                                    qsTrId("wallet-receive-expires-12")
                                ]
                            }
        
                            Binding {
                                target:   viewModel
                                property: "offerExpires"
                                value:    expiresCombo.currentIndex
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
                        title: qsTrId("receive-amount-swap-label")
                        Layout.fillWidth:        true
                        content:
                        AmountInput {
                            id:                         receiveAmountInput
                            color:                      Style.accent_outgoing
                            currencies:                 viewModel.currenciesList
                            currencyIdx:                viewModel.receiveAssetIndex
                            amount:                     viewModel.amountToReceive
                            multi:                      true
                            resetAmount:                false
                            currColor:                  Style.content_main
                            filterAssets:               true
                            error: {
                                if (viewModel.isAssetsSame)
                                {
                                    //% "You can't create offer with identical assets on both sides"
                                    return qsTrId("assets-same-create");
                                }
                                return ""
                            }
                        }

                        Binding {
                            target:   viewModel
                            property: "amountToReceive"
                            value:    receiveAmountInput.amount
                        }

                        Binding {
                            target:   viewModel
                            property: "receiveAssetIndex"
                            value:    receiveAmountInput.currencyIdx
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

                                SFText {
                                    font.pixelSize:         14
                                    color:                  Style.content_secondary
                                    //% "Exchange rate"
                                    text:                   qsTrId("general-rate") + ":"
                                }

                                SFText {
                                    font.pixelSize:   14
                                    color:            Style.content_main
                                    text:             viewModel.rate
                                }
                            }
                        }
                    }
                } // ColumnLayout
            }

            //
            // Footer
            //
            RowLayout {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                spacing:             30

                CustomButton {
                    //% "publish offer"
                    text:                qsTrId("wallet-receive-swap-publish")
                    palette.buttonText:  Style.content_opposite
                    icon.color:          Style.content_opposite
                    palette.button:      Style.active
                    icon.source:         "qrc:/assets/icon-share.svg"
                    enabled:             viewModel.canCreate
                    onClicked: {
                        const dialogComponent = Qt.createComponent("assets_swap_confirm.qml");
                        var dialogObject = dialogComponent.createObject(thisView,
                            {
                                createAssetSwap: true,
                                sendAmount: viewModel.amountToSend,
                                sendUnitName: sentAmountInput.currencyUnit,
                                receiveAmount: viewModel.amountToReceive,
                                receiveUnitName: receiveAmountInput.currencyUnit,
                            });

                        dialogObject.onAccepted.connect(function () {
                            viewModel.publishOffer();
                            thisView.onClosed();
                        });

                        dialogObject.open();
                    }

                }
            }
        }  // ColumnLayout
    } // Scroll
}
