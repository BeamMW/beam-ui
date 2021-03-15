import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: control
    spacing: 0
    property var defaultFocusItem: tokenInput

    SendViewModel {
        id: viewModel

        onSendMoneyVerified: function () {
            onAccepted()
        }

        onCantSendToExpired: function () {
            Qt.createComponent("send_expired.qml")
                .createObject(control)
                .open();
        }
    }

    property alias assetId:   viewModel.assetId
    property alias assetIdx:  sendAmountInput.currencyIdx
    property var   assetInfo: viewModel.assetsList[assetIdx]

    property var   sendUnit:  assetInfo.unitName
    property var   rate:      assetInfo.rate
    property var   rateUnit:  assetInfo.rateUnit

    // callbacks set by parent
    property var onAccepted:  undefined
    property var onClosed:    undefined
    property var onSwapToken: undefined

    onAssetIdChanged: function () {
        // C++ provides asset id, combobox exepects index, need to fix this at some point
        for (var idx = 0; idx < viewModel.assetsList.length; ++idx) {
            if (viewModel.assetsList[idx].assetId == assetId) {
                 if (assetIdx != idx) {
                    assetIdx = idx
                 }
            }
        }
    }

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    TokenInfoDialog {
        id:         tokenInfoDialog
        token:      viewModel.token
        incoming:   false
    }

    //
    // Title row
    //
    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30

        //% "Send"
        text:   qsTrId("send-title")
        onBack: control.onClosed
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
                    // Transaction info
                    //
                    Panel {
                        //% "Send to"
                        title:            qsTrId("general-send-to")
                        Layout.fillWidth: true
                        content: ColumnLayout {
                            spacing: 0
                            SFTextInput {
                                property bool tokenError:  viewModel.token && !viewModel.tokenValid
                                property bool isSwap:      BeamGlobals.isSwapToken(text)

                                Layout.fillWidth: true
                                id:               tokenInput
                                font.pixelSize:   14
                                color:            tokenError ? Style.validator_error : Style.content_main
                                backgroundColor:  tokenError ? Style.validator_error : Style.content_main
                                font.italic :     tokenError
                                text:             viewModel.token
                                validator:        RegExpValidator { regExp: /[0-9a-zA-Z]{1,}/ }
                                selectByMouse:    true

                                //% "Paste recipient address here"
                                placeholderText:  qsTrId("send-contact-address-placeholder")
                                onTextChanged: function () {
                                    if (isSwap && typeof onSwapToken == "function") {
                                        onSwapToken(text);
                                    }
                                }
                            }

                            SFText {
                                Layout.alignment: Qt.AlignTop
                                Layout.fillWidth: true
                                id:               receiverTAError
                                color:            tokenInput.tokenError ? Style.validator_error : Style.content_secondary
                                font.italic:      tokenInput.tokenError
                                font.pixelSize:   12
                                wrapMode:         Text.Wrap
                                text:             tokenInput.tokenError ?
                                                  //% "Invalid wallet address"
                                                  qsTrId("wallet-send-invalid-address-or-token") :
                                                  viewModel.newTokenMsg
                                visible:          tokenInput.tokenError || viewModel.newTokenMsg
                            }
                    
                            Binding {
                                target:   viewModel
                                property: "token"
                                value:    tokenInput.text
                            }

                            SFText {
                                Layout.alignment:   Qt.AlignTop
                                Layout.fillWidth:   true
                                id:                 addressNote
                                color:              Style.content_secondary
                                font.italic:        true
                                font.pixelSize:     12
                                wrapMode:           Text.Wrap
                                text:               viewModel.tokenType
                                visible:            viewModel.tokenValid
                            }
                        }
                    }

                    Panel {
                        //% "Transaction type"
                        title: qsTrId("general-tx-type")
                        Layout.fillWidth: true
                        visible: viewModel.canChoose

                        content: ColumnLayout {
                            spacing: 20
                            id: addressType

                            Pane {
                                padding: 2

                                background: Rectangle {
                                    color:  Qt.rgba(1, 1, 1, 0.1)
                                    radius: 16
                                }

                                ButtonGroup {
                                    id: txTypeGroup
                                }

                                RowLayout {
                                    spacing: 0

                                    CustomButton {
                                        Layout.preferredHeight: 30
                                        Layout.preferredWidth: maxPrivacyCheck.width
                                        id: regularCheck
                                        //% "Regular"
                                        text:               qsTrId("tx-regular")
                                        ButtonGroup.group:  txTypeGroup
                                        checkable:          true
                                        hasShadow:          false
                                        checked:            !viewModel.choiceOffline
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary
                                    }

                                    CustomButton {
                                        Layout.preferredHeight: 30
                                        Layout.minimumWidth: 137
                                        id: offlineCheck
                                        //% "Offline"
                                        text:               qsTrId("tx-offline")
                                        ButtonGroup.group:  txTypeGroup
                                        checkable:          true
                                        checked:            viewModel.choiceOffline
                                        hasShadow:          false
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary
                                    }

                                    Binding {
                                        target:   viewModel
                                        property: "choiceOffline"
                                        value:    offlineCheck.checked
                                    }
                                }
                            }
                        }
                    }

                    //
                    // Amount
                    //
                    Panel {
                        //% "Amount"
                        title: qsTrId("general-amount")
                        Layout.fillWidth: true

                        content: RowLayout {
                            spacing: 7

                            AmountInput {
                                id:                sendAmountInput
                                amountIn:          viewModel.sendAmount
                                color:             Style.accent_outgoing
                                Layout.fillWidth:  true
                                currencies:        viewModel.assetsList
                                multi:             viewModel.assetsList.length > 1

                                error: {
                                    if (!viewModel.isEnough)
                                    {
                                       var amount = Utils.uiStringToLocale(viewModel.maxSendAmount)
                                       //% "Insufficient funds to complete the transaction. Maximum amount is %1 %2."
                                       return qsTrId("send-no-funds").arg(amount).arg(control.sendUnit)
                                    }
                                    return ""
                                }

                                onCurrencyIdxChanged: function () {
                                    var idx = sendAmountInput.currencyIdx
                                    control.assetId = viewModel.assetsList[idx].assetId
                                }
                            }

                            Binding {
                                target:   viewModel
                                property: "sendAmount"
                                value:    sendAmountInput.amount
                            }

                            Row {
                                Layout.leftMargin: 10
                                Layout.fillHeight: true
                                spacing:           0

                                SvgImage {
                                    source:     "qrc:/assets/icon-send-blue-copy-2.svg"
                                    sourceSize: Qt.size(16, 16)
                                    y:          30

                                    MouseArea {
                                        anchors.fill:    parent
                                        acceptedButtons: Qt.LeftButton
                                        cursorShape:     Qt.PointingHandCursor
                                        onClicked:       function () {
                                            sendAmountInput.clearFocus()
                                            viewModel.setMaxAvailableAmount()
                                        }
                                    }
                                }

                                SFText {
                                    font.pixelSize:   14
                                    font.styleName:   "Bold";
                                    font.weight:      Font.Bold
                                    color:            Style.accent_outgoing
                                    y:                30
                                    //% "add max"
                                    text:             " " + qsTrId("amount-input-add-max")

                                    MouseArea {
                                        anchors.fill:    parent
                                        acceptedButtons: Qt.LeftButton
                                        cursorShape:     Qt.PointingHandCursor
                                        onClicked:       function () {
                                            sendAmountInput.clearFocus()
                                            viewModel.setMaxPossibleAmount()
                                        }
                                    }
                                }
                            }
                        }
                    }

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title: qsTrId("general-comment")
                        Layout.fillWidth: true
                        folded: false

                        content: ColumnLayout {
                            SFTextInput {
                                id:               addressComment
                                font.pixelSize:   14
                                Layout.fillWidth: true
                                color:            Style.content_main
                                text:             viewModel.comment
                                maximumLength:    BeamGlobals.maxCommentLength()
                                //% "Comments are local and won't be shared"
                                placeholderText:  qsTrId("general-comment-local")
                            }
                 
                            Binding {
                                target:   viewModel
                                property: "comment"
                                value:    addressComment.text
                            }
                        }
                    }
                }

                //
                // Right column
                //
                ColumnLayout {
                    Layout.alignment:      Qt.AlignTop
                    Layout.fillWidth:      true
                    Layout.preferredWidth: 400
                    spacing:               10

                    Pane {
                        Layout.fillWidth:        true
                        padding:                 20

                        background: Rectangle {
                            radius: 10
                            color:  Style.background_button
                        }

                        GridLayout {
                            anchors.fill:   parent
                            columnSpacing:  35
                            rowSpacing:     14
                            columns:        2

                            SFText {
                                Layout.alignment:  Qt.AlignTop
                                font.pixelSize:    14
                                color:             viewModel.isEnough ? Style.content_secondary : Style.validator_error
                                //% "Amount to send"
                                text:              qsTrId("send-amount-label") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:  Qt.AlignTop
                                Layout.fillWidth:  true
                                error:             !viewModel.isEnough
                                amount:            viewModel.sendAmount
                                unitName:          control.sendUnit
                                rateUnit:          control.rateUnit
                                rate:              control.rate
                                font.styleName:    "Bold"
                                font.weight:       Font.Bold
                                maxPaintedWidth:   false
                                maxUnitChars:      20
                            }
                    
                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                font.pixelSize:         14
                                color:                  viewModel.isEnough ? Style.content_secondary : Style.validator_error
                                text:                   qsTrId("general-change") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:  Qt.AlignTop
                                Layout.fillWidth:  true
                                error:             !viewModel.isEnough
                                amount:            viewModel.changeAsset
                                unitName:          control.sendUnit
                                rateUnit:          control.assetId == 0 ? control.rateUnit : ""
                                rate:              control.rate
                                font.styleName:    "Bold"
                                font.weight:       Font.Bold
                                maxPaintedWidth:   false
                                maxUnitChars:      20
                            }

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                font.pixelSize:         14
                                color:                  viewModel.isEnough ? Style.content_secondary : Style.validator_error
                                text:                   qsTrId("send-regular-fee") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:  Qt.AlignTop
                                Layout.fillWidth:  true
                                error:             !viewModel.isEnough
                                amount:            viewModel.fee
                                unitName:          BeamGlobals.beamUnit
                                rateUnit:          viewModel.feeRateUnit
                                rate:              viewModel.feeRate
                                font.styleName:    "Bold"
                                font.weight:       Font.Bold
                                maxPaintedWidth:   false
                                maxUnitChars:      20
                            }

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                font.pixelSize:         14
                                color:                  viewModel.isEnough ? Style.content_secondary : Style.validator_error
                                //% "Remaining"
                                text:                   qsTrId("send-remaining-label") + ":"
                            }
                    
                            BeamAmount {
                                Layout.alignment:  Qt.AlignTop | Qt.AlignLeft
                                Layout.fillWidth:  true
                                error:             !viewModel.isEnough
                                amount:            viewModel.assetAvailable
                                unitName:          control.sendUnit
                                rateUnit:          control.rateUnit
                                rate:              control.rate
                                font.styleName:    "Bold"
                                font.weight:       Font.Bold
                                maxPaintedWidth:   false
                                maxUnitChars:      20
                            }

                            SFText {
                                Layout.alignment:       Qt.AlignTop
                                font.pixelSize:         14
                                color:                  viewModel.isEnough ? Style.content_secondary : Style.validator_error
                                visible:                control.assetId != 0
                                //% "BEAM Remaining"
                                text:                   qsTrId("send-remaining-beam-label") + ":"
                            }

                            BeamAmount {
                                Layout.alignment:  Qt.AlignTop | Qt.AlignLeft
                                Layout.fillWidth:  true
                                error:             !viewModel.isEnough
                                amount:            viewModel.beamRemaining
                                unitName:          BeamGlobals.beamUnit
                                rateUnit:          viewModel.feeRateUnit
                                rate:              viewModel.feeRate
                                visible:           control.assetId != 0
                                font.styleName:    "Bold"
                                font.weight:       Font.Bold
                                maxPaintedWidth:   false
                                maxUnitChars:      20
                            }
                        }
                    }
                }
            }

            //
            // Footers
            //
            CustomButton {
                Layout.alignment:    Qt.AlignHCenter
                Layout.topMargin:    30
                Layout.bottomMargin: 30
                //% "Send"
                text:                qsTrId("general-send")
                palette.buttonText:  Style.content_opposite
                palette.button:      Style.accent_outgoing
                icon.source:         "qrc:/assets/icon-send-blue.svg"
                enabled:             viewModel.canSend
                onClicked: {                
                    const dialog = Qt.createComponent("send_confirm.qml")
                    const instance = dialog.createObject(control,
                        {
                            addressText:   viewModel.token,
                            typeText:      function () {
                                               //% "Max Privacy"
                                               if (viewModel.isMaxPrivacy) return qsTrId("tx-max-privacy")
                                               //% "Public offline"
                                               if (viewModel.isPublicOffline) return qsTrId("tx-address-public-offline")
                                               //% "Offline"
                                               if (viewModel.isOffline) return qsTrId("tx-address-offline")
                                               //% "Regular"
                                               return qsTrId("tx-regular")
                                           },

                            isOnline:      !viewModel.isMaxPrivacy && !viewModel.isPublicOffline && !viewModel.isOffline,
                            amount:        viewModel.sendAmount,
                            fee:           viewModel.fee,
                            flatFee:       true,
                            unitName:      control.sendUnit,
                            rate:          control.rate,
                            rateUnit:      control.rateUnit,
                            acceptHandler: acceptedCallback,
                        })
                    instance.open()

                    function acceptedCallback() {
                        viewModel.sendMoney();
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }  // ColumnLayout
    }  // ScrollView
} // ColumnLayout
