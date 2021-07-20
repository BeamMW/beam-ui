import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "../controls"

CustomDialog {
    id: "dialog"

    property var sendAddress: ""
    property var receiveAddress: ""
    property var senderIdentity: ""
    property var receiverIdentity: ""
    property var comment: ""
    property var txID: ""
    property var kernelID: ""
    property var status: ""
    property var failureReason: ""
    property var isIncome: ""
    property var hasPaymentProof: ""
    property var isSelfTx: ""
    property var rawTxID: ""
    property var stateDetails: ""
    property string token
    property bool hasToken: token.length > 0 

    property string fee
    property string feeUnit
    property string feeRate
    property string feeRateUnit

    property string  cidsStr
    property string  searchFilter: ""
    property string  addressType
    property bool    isShieldedTx: false
    property bool    isCompleted:  false
    property bool    isContractTx: false
    property int     minConfirmations: 0
    property string  confirmationsProgress: ""
    property string  dappName: ""

    property var assetNames: []
    property var assetIcons: [""]
    property var assetAmounts: []
    property var assetIncome: []
    property var assetRates: []
    property var assetIDs: []
    property string rateUnit
    readonly property int assetCount: assetNames ? assetNames.length : 0

    property alias initialState: stm.state
    property var getPaymentProof: function (rawTxId) { return null; }

    function getHighlitedText(text) {
        return Utils.getHighlitedText(text, dialog.searchFilter, Style.active.toString());
    }

    property PaymentInfoItem paymentInfo
    signal textCopied(string text)
    signal openExternal(string kernelId)

    modal: true
    width: 760
    height: 650
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    header: ColumnLayout {
        SFText {
            Layout.topMargin: 30
            Layout.bottomMargin: 30
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 18
            font.styleName: "Bold";
            font.weight: Font.Bold
            color: Style.content_main
            //% "Transaction info"
            text: qsTrId("tx-details-popup-title")
        }
    }

    contentItem: ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        id: stm
        state: "tx_info"
        states: [
            State {
                name: "tx_info";
                PropertyChanges {target: txInfo; state: "active"}
            },
            State {
                name: "payment_proof";
                PropertyChanges {target: paymentProof; state: "active"}
            }
        ]

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Row {
                TxFilter {
                    id: txInfo
                    //% "General info"
                    label: qsTrId("tx-details-general-info")
                    inactiveColor: Style.content_disabled
                    onClicked: stm.state = "tx_info"
                }

                TxFilter {
                    id: paymentProof
                    //% "Payment proof"
                    label: qsTrId("general-payment-proof")
                    inactiveColor: Style.content_disabled
                    onClicked: {
                        if (dialog.hasPaymentProof && getPaymentProof && typeof getPaymentProof == "function") {
                            dialog.paymentInfo = getPaymentProof(dialog.rawTxID);
                            stm.state = "payment_proof";
                        }
                    }
                    visible: dialog.hasPaymentProof && !dialog.isSelfTx
                }
            }
        }

        GridLayout {
            id: grid
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.topMargin: 30
            Layout.alignment: Qt.AlignTop
            columnSpacing: 40
            rowSpacing: 14
            columns: 2
            
            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Sending address"
                text: qsTrId("tx-details-sending-addr-label") + ":"
                visible: sendAddressField.parent.visible
            }
            RowLayout {
                visible: stm.state == "tx_info" && dialog.sendAddress.length && !(isIncome && isShieldedTx)
                SFLabel {
                    id: sendAddressField
                    Layout.fillWidth: true
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    elide: Text.ElideMiddle
                    text: getHighlitedText(dialog.sendAddress)
                    onCopyText: textCopied(dialog.sendAddress)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(sendAddressField.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Sender's wallet signature"
                text: qsTrId("tx-details-sender-identity") + ":"
                visible: senderIdentityField.parent.visible
            }
            RowLayout {
                visible: stm.state == "payment_proof" && dialog.senderIdentity.length > 0 && (dialog.receiverIdentity.length > 0 || dialog.isShieldedTx )
                SFLabel {
                    id: senderIdentityField
                    Layout.fillWidth: true
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    elide: Text.ElideMiddle
                    text: getHighlitedText(dialog.senderIdentity)
                    onCopyText: textCopied(dialog.senderIdentity)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(senderIdentityField.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Receiving address"
                text: qsTrId("tx-details-receiving-addr-label") + ":"
                visible: receiveAddressField.parent.visible
            }
            RowLayout {
                visible: stm.state == "tx_info" && !dialog.isContractTx && receiveAddressField.receiveAddressOrToken.length
                SFLabel {
                    property var receiveAddressOrToken : hasToken ? dialog.token : dialog.receiveAddress
                    id: receiveAddressField
                    Layout.fillWidth: true
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    elide: Text.ElideMiddle
                    text: getHighlitedText(receiveAddressOrToken)
                    onCopyText: textCopied(receiveAddressOrToken)
                }
                CustomToolButton {
                    Layout.alignment:Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(receiveAddressField.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Receiver's wallet signature"
                text: qsTrId("tx-details-receiver-identity") + ":"
                visible: receiverIdentityField.parent.visible
            }
            RowLayout {
                visible: stm.state == "payment_proof" && dialog.senderIdentity.length > 0 && dialog.receiverIdentity.length > 0
                SFLabel {
                    id: receiverIdentityField
                    Layout.fillWidth: true
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    elide: Text.ElideMiddle
                    text: getHighlitedText(dialog.receiverIdentity)
                    onCopyText: textCopied(dialog.receiverIdentity)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(receiverIdentityField.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            // Address type
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_secondary
                //% "Address type"
                text:                   qsTrId("address-info-type") + ":"
                visible:                addrTypeText.visible
            }
                
            SFText {
                id:                     addrTypeText
                Layout.fillWidth:       true
                wrapMode:               Text.Wrap
                font.pixelSize:         14
                text:                   dialog.addressType
                color:                  Style.content_main
                visible:                !dialog.isContractTx && stm.state == "tx_info"
            }

            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Confirmation status"
                text: qsTrId("tx-details-confirmation-status-label") + ":"
                visible:          minConfirmationsField.visible
            }

            SFLabel {
                id:               minConfirmationsField
                font.pixelSize:   14
                color:            Style.content_main
                //% "Confirmed (%1)"
                text:             qsTrId("tx-details-confirmation-progress-label").arg(dialog.confirmationsProgress)
                visible:          dialog.minConfirmations && stm.state == "tx_info"
            }

            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Amount"
                text: qsTrId("tx-details-amount-label") + ":"
            }

            ColumnLayout {
                id: amountsList
                Layout.fillWidth: true
                spacing: 10

                Repeater {
                    model: dialog.assetCount

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.maximumWidth : dialog.width - 60

                        BeamAmount {
                            id: amountField
                            Layout.fillWidth: true

                            visible:      true
                            amount:       dialog.assetAmounts ? (dialog.assetAmounts[index] || "") : ""
                            unitName:     dialog.assetNames[index] || ""
                            iconSource:   dialog.assetIcons[index] || ""
                            iconSize:     Qt.size(20, 20)
                            color:        dialog.assetIncome[index] ? Style.accent_incoming : Style.accent_outgoing
                            prefix:       this.amount == "0" ? "" : (dialog.assetIncome[index] ? "+ " : "- ")
                            rate:         dialog.assetRates ? (dialog.assetRates[index] || "") : ""
                            rateUnit:     dialog.rateUnit// this.rate != "0" ? dialog.rateUnit : ""
                            ratePostfix:  this.rate != "0"
                                //% "calculated with the exchange rate at the day of transaction"
                                ? "(" + qsTrId("tx-details-rate-notice") + ")"
                                //% "exchange rate was not available at the time of transaction"
                                : "(" + qsTrId("tx-details-exchange-rate-not-available") + ")"
                            showTip:      false
                            maxUnitChars: 25
                            maxPaintedWidth: false
                            font {
                                styleName:  "Bold"
                                weight:     Font.Bold
                                pixelSize:  14
                            }
                        }

                        SFText {
                            font.pixelSize: 14
                            color: Style.content_secondary
                            //% "Confidential asset ID"
                            text: qsTrId("general-ca-id") + ":"
                            visible: assetIdField.visible
                        }
                        SFLabel {
                            id: assetIdField
                            copyMenuEnabled: true
                            font.pixelSize: 14
                            color: Style.content_main
                            text: dialog.assetIDs[index] || ""
                            onCopyText: textCopied(dialog.assetIDs[index])
                            visible: dialog.assetIDs[index] != "0"
                        }
                        CustomToolButton {
                            Layout.alignment: Qt.AlignRight
                            icon.source: "qrc:/assets/icon-copy.svg"
                            onClicked: textCopied(dialog.assetIDs[index])
                            visible: dialog.assetIDs[index] != "0"
                            padding: 0
                            background.implicitHeight: 16
                        }
                    }
                }
            }

            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Transaction fee"
                text: qsTrId("general-fee") + ": "
                visible: feeField.parent.visible
            }

            RowLayout {
                visible: dialog.fee.length && stm.state == "tx_info"
                BeamAmount {
                    id: feeField
                    Layout.fillWidth: true

                    amount:    dialog.fee
                    unitName:  dialog.feeUnit
                    rateUnit:  dialog.feeRateUnit
                    rate:      dialog.feeRate
                    showTip:   false
                    maxPaintedWidth: false
                    iconSource:   "qrc:/assets/icon-beam.svg"
                    iconSize:     Qt.size(20, 20)
                }
            }

            // CID
            SFText {
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                color:                  Style.content_secondary
                //% "DAPP name"
                text:                   qsTrId("address-info-dapp") + ":"
                visible:                dappNameText.visible
            }

            SFLabel {
                id:               dappNameText
                font.pixelSize:   14
                color:            Style.content_main
                text:             dialog.dappName
                elide:            Text.ElideRight
                copyMenuEnabled:  false
                visible:          dialog.isContractTx && stm.state == "tx_info"
            }

            // CID
            SFText {
                font.pixelSize:         14
                color:                  Style.content_secondary
                //% "Application shader ID"
                text:                   qsTrId("address-info-cid") + ":"
                visible:                cidText.parent.visible
            }

            RowLayout {
                visible:          dialog.isContractTx && stm.state == "tx_info"
                SFLabel {
                    id:               cidText
                    Layout.fillWidth: true
                    font.pixelSize:   14
                    color:            Style.content_main
                    text:             dialog.cidsStr
                    elide:            Text.ElideRight
                    copyMenuEnabled:  true
                    onCopyText:       textCopied(text)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(cidText.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_secondary

                text: isContractTx ?
                    //% "Description"
                    qsTrId("general-description") + ":" :
                    //% "Comment"
                    qsTrId("general-comment") + ":"

                visible: commentTx.visible
            }

            SFLabel {
                Layout.fillWidth: true
                id: commentTx
                copyMenuEnabled: true
                font.pixelSize: 14
                color: Style.content_main
                wrapMode: Text.WrapAnywhere 
                text: getHighlitedText(dialog.comment)
                elide: Text.ElideRight
                onCopyText: textCopied(dialog.comment)
                visible: stm.state == "tx_info" && dialog.comment.length
            }

            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Transaction ID"
                text: qsTrId("tx-details-tx-id-label") + ":"
                visible: transactionID.parent.visible
            }
            RowLayout {
                visible: !Utils.isZeroed(dialog.txID) && stm.state == "tx_info"
                SFLabel {
                    Layout.fillWidth: true
                    id: transactionID
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    text: getHighlitedText(dialog.txID)
                    elide: Text.ElideMiddle
                    onCopyText: textCopied(dialog.txID)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(transactionID.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }
            SFText {
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Kernel ID"
                text: qsTrId("general-kernel-id") + ":"
                visible: kernelID.parent.visible
            }
            RowLayout {
                visible: !Utils.isZeroed(dialog.kernelID)
                SFLabel {
                    Layout.fillWidth: true
                    id: kernelID
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    text: getHighlitedText(dialog.kernelID)
                    elide: Text.ElideMiddle
                    onCopyText: textCopied(dialog.kernelID)
                }
                CustomToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(kernelID.text)
                    padding: 0
                    background.implicitHeight: 16
                }
            }

            Item {
                height: 16
                visible: dialog.isCompleted && kernelID.parent.visible
            }
            Item {
                Layout.preferredWidth: openInExplorer.width + 10 + openInExplorerIcon.width
                height: 16
                visible: dialog.isCompleted && kernelID.parent.visible

                SFText {
                    id: openInExplorer
                    font.pixelSize: 14
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.rightMargin: 10
                    color: Style.active
                    //% "Open in Blockchain Explorer"
                    text: qsTrId("open-in-explorer")
                }
                SvgImage {
                    id: openInExplorerIcon
                    anchors.top: parent.top
                    anchors.right: parent.right
                    source: "qrc:/assets/icon-external-link-green.svg"
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        openExternal(dialog.kernelID);
                    }
                    hoverEnabled: true
                }
            }

            RowLayout {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: stm.state == "tx_info" && dialog.stateDetails.length
                SvgImage {
                    Layout.alignment: Qt.AlignTop
                    sourceSize: Qt.size(16, 16)
                    source:  "qrc:/assets/icon-attention.svg"
                }
                SFLabel {
                    Layout.fillWidth: true
                    copyMenuEnabled: true
                    font.pixelSize: 14
                    color: Style.content_main
                    wrapMode: Text.Wrap
                    elide: Text.ElideMiddle
                    text: dialog.stateDetails
                    onCopyText: textCopied(text)
                }
            }
            
            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                color: Style.content_secondary
                //% "Error"
                text: qsTrId("tx-details-error-label") + ":"
                visible: dialog.failureReason.length
            }
            SFLabel {
                id: failureReason
                Layout.fillWidth: true
                copyMenuEnabled: true
                font.pixelSize: 14
                color: Style.content_main
                wrapMode: Text.Wrap
                visible: dialog.failureReason.length > 0 && stm.state == "tx_info"
                text: dialog.failureReason.length > 0 ? dialog.failureReason : ""
                elide: Text.ElideRight
                onCopyText: textCopied(text)
            }

            Rectangle {
                width: parent.width
                height: 1
                color: Style.background_button
                Layout.columnSpan: 2
                visible: stm.state == "payment_proof" && dialog.hasPaymentProof
            }

            SFText {
                Layout.alignment: Qt.AlignTop
                font.pixelSize: 14
                font.styleName: "Bold"
                font.weight: Font.Bold
                color: Style.content_secondary
                //% "Code"
                text: qsTrId("payment-info-proof-code-label") + ":"
                visible: stm.state == "payment_proof" && dialog.hasPaymentProof
            }

            RowLayout {
                visible: stm.state == "payment_proof" && dialog.hasPaymentProof
                ScrollView {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth:             true
                    Layout.maximumHeight:         120
                    clip:                         true
                    ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy:    ScrollBar.AsNeeded
                    SFText {
                        width:              450
                        wrapMode:           Text.Wrap
                        font.pixelSize:     14
                        text:               paymentInfo ? paymentInfo.paymentProof : ""
                        color:              Style.content_main
                    }
                }

                CustomToolButton {
                    Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    icon.source: "qrc:/assets/icon-copy.svg"
                    onClicked: textCopied(paymentInfo ? paymentInfo.paymentProof : "")
                    padding: 0
                    background.implicitHeight: 16
                }
            }
        }
    }

    footer: ColumnLayout {
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 30
            Layout.topMargin: 30
            spacing: 20

            CustomButton {
                //% "Close"
                text:               qsTrId("general-close")
                icon.source:        "qrc:/assets/icon-cancel-16.svg"
                onClicked:          dialog.close()
            }
        }
    }

    onOpened: {
        dialog.height = grid.height + 260;
    }

    onClosed: {
        dialog.height = 650;
    }
}
