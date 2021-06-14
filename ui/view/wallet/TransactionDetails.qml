import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "../controls"
import "."

RowLayout {
    id: "root"
    property var sendAddress
    property var receiveAddress
    property var senderIdentity
    property var receiverIdentity
    property var comment
    property var txID
    property var kernelID
    property var status
    property var failureReason
    property var isIncome
    property var hasPaymentProof
    property var isSelfTx
    property var rawTxID
    property var stateDetails
    property var searchRegExp: new RegExp("("+root.searchFilter+")", "gi")
    property var searchRegExp2:  new RegExp("("+root.searchFilter+")", "i")
    property string token

    property string fee
    property string feeUnit
    property string feeRate
    property string feeRateUnit

    property string cidsStr
    property string searchFilter: ""
    property bool   hideFiltered: false
    property string addressType
    property bool   isShieldedTx: false
    property bool   isCompleted:  false
    property bool   isContractTx: false
    property int minConfirmations: 0
    property string confirmationsProgress: ""

    property var  assetNames
    property var  assetIcons
    property var  assetAmounts
    property var  assetIncome
    property var  assetRates
    property var  assetIDs
    property var  rateUnit
    property var  totalValue
    readonly property int assetCount: assetNames ? assetNames.length : 0

    property var onOpenExternal: null
    signal textCopied(string text)
    signal copyPaymentProof()
    signal showPaymentProof()

    spacing: 0

    function isFieldVisible() {
        return root.searchFilter.length == 0 || hideFiltered == false;
    }

    function isTextFieldVisible(text) {
        return isFieldVisible()
        || (root.searchFilter.length > 0 && text.search(root.searchRegExp2) >= 0);
    }

    function getHighlitedText(text) {
        
        if (root.searchFilter.length == 0)
            return text;

        var start = text.search(root.searchRegExp2);
        if (start == -1)
            return text;

        return text.replace(root.searchRegExp, '<font color="' + Style.active.toString() + '">$1</font>');
    }

    function isZeroed(s) {
        for (var i = 0; i < s.length; ++i) {
            if (s[i] != '0')
                return false;
        }
        return true;
    }

    property bool hasToken: token.length > 0 

    GridLayout {
        Layout.fillWidth: true
        Layout.preferredWidth: 4
        Layout.leftMargin: 30
        Layout.rightMargin: 30
        Layout.topMargin: 30
        Layout.bottomMargin: 30
        columnSpacing: 40
        rowSpacing: 14
        columns: 2

        SFText {
            font.pixelSize: 14
            color: Style.content_main
            //% "General transaction info"
            text: qsTrId("tx-details-title")
            font.styleName: "Bold"; font.weight: Font.Bold
            Layout.columnSpan: 2
            visible: root.isFieldVisible()
        }
        
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Sending address"
            text: qsTrId("tx-details-sending-addr-label") + ":"
            visible: sendAddressField.visible
        }
        SFLabel {
            id: sendAddressField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            elide: Text.ElideMiddle
            text: getHighlitedText(root.sendAddress)
            onCopyText: textCopied(root.sendAddress)
            visible: isTextFieldVisible(root.sendAddress) && root.sendAddress.length && !(isIncome && isShieldedTx)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Sender identity"
            text: qsTrId("tx-details-sender-identity") + ":"
            visible: senderIdentityField.visible
        }
        SFLabel {
            id: senderIdentityField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            elide: Text.ElideMiddle
            text: getHighlitedText(root.senderIdentity)
            onCopyText: textCopied(root.senderIdentity)
            visible: root.senderIdentity.length > 0 && (root.receiverIdentity.length > 0 || root.isShieldedTx ) && isTextFieldVisible(root.senderIdentity)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Receiving address"
            text: qsTrId("tx-details-receiving-addr-label") + ":"
            visible: receiveAddressField.visible
        }
        SFLabel {
            property var receiveAddressOrToken : hasToken ? root.token : root.receiveAddress
            id: receiveAddressField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            elide: Text.ElideMiddle
            text: getHighlitedText(receiveAddressOrToken)
            onCopyText: textCopied(receiveAddressOrToken)
            visible: receiveAddressOrToken.length && isTextFieldVisible(receiveAddressOrToken)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Receiver identity"
            text: qsTrId("tx-details-receiver-identity") + ":"
            visible: receiverIdentityField.visible
        }
        SFLabel {
            id: receiverIdentityField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            elide: Text.ElideMiddle
            text: getHighlitedText(root.receiverIdentity)
            onCopyText: textCopied(root.receiverIdentity)
            visible: root.senderIdentity.length > 0 && root.receiverIdentity.length > 0 && isTextFieldVisible(root.receiverIdentity)
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
            text:                   root.addressType
            color:                  Style.content_main
            visible:                !root.isContractTx && isTextFieldVisible(root.addressType)
        }

        // CID
        SFText {
            Layout.alignment:       Qt.AlignTop
            font.pixelSize:         14
            color:                  Style.content_secondary
            //% "Contract ID"
            text:                   qsTrId("address-info-cid") + ":"
            visible:                cidText.visible
        }

        SFLabel {
            id:               cidText
            font.pixelSize:   14
            color:            Style.content_main
            text:             root.cidsStr
            elide:            Text.ElideRight
            copyMenuEnabled:  true
            onCopyText:       textCopied(text)
            visible:          root.isContractTx
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Confirmation status"
            text: qsTrId("tx-details-confirmation-status-label") + ":"
            visible:          root.minConfirmations
        }

        SFLabel {
            font.pixelSize:   14
            color:            Style.content_main
            //% "Confirmed (%1)"
            text:             qsTrId("tx-details-confirmation-progress-label").arg(root.confirmationsProgress)
            visible:          root.minConfirmations
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Amount"
            text: qsTrId("tx-details-amount-label") + ":"
            visible: amountsList.visible
        }

        ColumnLayout {
            id: amountsList
            Layout.fillWidth: true
            spacing: 10
            visible: true// visibleChildren.length > 0

            Repeater {
                model: root.assetCount

                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumWidth : 450
                    BeamAmount {
                        Layout.fillWidth: true

                        visible:      true //isTextFieldVisible(root.assetAmounts[index])
                        amount:       root.assetAmounts[index]
                        unitName:     root.assetNames[index]
                        iconSource:   root.assetCount > 1 ? root.assetIcons[index] : ""
                        iconSize:     Qt.size(20, 20)
                        color:        root.assetIncome[index] ? Style.accent_incoming : Style.accent_outgoing
                        prefix:       this.amount == "0" ? "" : (root.assetIncome[index] ? "+ " : "- ")
                        rate:         root.assetRates[index]
                        rateUnit:     this.rate != "0" ? root.rateUnit : ""
                        showTip:      false
                        //maxPaintedWidth: this.width don't enable, causes freeze of animations, neet to refactor
                        font {
                        styleName:  "Bold"
                        weight:     Font.Bold
                        pixelSize:  14
                        }
                    }

                    SFText {
                        Layout.alignment: Qt.AlignTop
                        font.pixelSize: 14
                        color: Style.content_secondary
                        //% "Confidential asset ID"
                        text: qsTrId("general-ca-id") + ":"
                        visible: root.assetIDs[index] != "0"
                    }
                    SFLabel {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true
                        copyMenuEnabled: true
                        font.pixelSize: 14
                        color: Style.content_main
                        text: root.assetIDs[index]
                        onCopyText: textCopied(root.kernelID)
                        visible: root.assetIDs[index] != "0"
                    }
                }
            }
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize:   14
            color:            Style.content_secondary
            visible:          totalValueCtrl.visible || totalWarningCtrl.visible

            text: (root.isContractTx ?
                     //% "Total value"
                     qsTrId("general-total value") :
                     //% "%1 Value"
                     qsTrId("general-smth-value").arg(root.rateUnit)
                  ) + ": "
        }

        ColumnLayout {
            Layout.fillWidth: true
            id:               totalValueCtrl
            visible:          root.totalValue != "0" && isTextFieldVisible(root.totalValue)
            spacing:          2

            SFText {
                Layout.fillWidth: true
                font.pixelSize:   14
                color:            Style.content_secondary
                text:             [root.totalValue, root.rateUnit].join(' ')
            }

            SFText {
                Layout.fillWidth: true
                font.pixelSize:   13
                font.italic:      true
                color:            Style.content_secondary
                //% "For the day of the transaction"
                text: qsTrId("tx-details-rate-notice")
            }
        }

        SFText
        {
            id:               totalWarningCtrl
            visible:          root.totalValue == "0" && isTextFieldVisible(text)
            Layout.fillWidth: true
            font.pixelSize:   14
            color:            Style.content_secondary
            //% "Exchange rate to %1 was not available at the time of transaction"
            text:             qsTrId("tx-details-exchange-rate-not-available").arg(root.rateUnit)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Transaction fee"
            text: qsTrId("general-fee") + ": "
            visible: root.isFieldVisible() && root.fee.length
        }

        BeamAmount {
            Layout.fillWidth: true
            visible: root.isFieldVisible() && root.fee.length

            amount:    root.fee
            unitName:  root.feeUnit
            rateUnit:  root.feeRateUnit
            rate:      root.feeRate
            showTip:   false
        }
        
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Comment"
            text: qsTrId("general-comment") + ":"
            visible: commentTx.visible
        }

        SFLabel {
            Layout.fillWidth: true
            id: commentTx
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            wrapMode: Text.WrapAnywhere 
            text: getHighlitedText(root.comment)
            elide: Text.ElideRight
            onCopyText: textCopied(root.comment)
            visible: isTextFieldVisible(root.comment)
        }
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Transaction ID"
            text: qsTrId("tx-details-tx-id-label") + ":"
            visible: transactionID.visible 
        }
        SFLabel {
            Layout.fillWidth: true
            id: transactionID
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            text: getHighlitedText(root.txID)
            elide: Text.ElideMiddle
            onCopyText: textCopied(root.txID)
            visible: isTextFieldVisible(root.txID) && !isZeroed(root.txID)
        }
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Kernel ID"
            text: qsTrId("general-kernel-id") + ":"
            visible: kernelID.visible
        }
        SFLabel {
            Layout.fillWidth: true
            id: kernelID
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            //wrapMode: Text.Wrap
            text: getHighlitedText(root.kernelID)
            elide: Text.ElideMiddle
            onCopyText: textCopied(root.kernelID)
            visible: isTextFieldVisible(root.kernelID) && !isZeroed(root.kernelID)
        }

        Item {
            Layout.preferredHeight: 16
            visible: root.isCompleted && root.isFieldVisible() && kernelID.visible
        }
        Item {
            Layout.preferredWidth: openInExplorer.width + 10 + openInExplorerIcon.width
            Layout.preferredHeight: 16
            visible: root.isCompleted && root.isFieldVisible() && kernelID.visible
        
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
                    if (onOpenExternal && typeof onOpenExternal === 'function') {
                        onOpenExternal();
                    }
                }
                hoverEnabled: true
            }
        }
        
        RowLayout {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            visible: root.stateDetails != "" && root.isFieldVisible()
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
                text: root.stateDetails
                onCopyText: textCopied(text)
            }
        }
        
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Error"
            text: qsTrId("tx-details-error-label") + ":"
            visible: root.failureReason.length > 0 && root.isFieldVisible()
        }
        SFLabel {
            id: failureReason
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            wrapMode: Text.Wrap
            visible: root.failureReason.length > 0 && root.isFieldVisible()
            text: root.failureReason.length > 0 ? root.failureReason : ""
            elide: Text.ElideRight
            onCopyText: textCopied(text)
        }
    }

    GridLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: 3
        Layout.rightMargin: 30
        Layout.topMargin: 30
        Layout.bottomMargin: 30
        columns: 2
        columnSpacing: 44
        rowSpacing: 14
        visible: !root.isIncome && root.isFieldVisible() && root.hasPaymentProof && !root.isSelfTx
    
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.columnSpan: 2
        }
        SFText {
            font.pixelSize: 14
            color: Style.content_main
            //% "Payment proof"
            text: qsTrId("general-payment-proof")
            font.styleName: "Bold"; font.weight: Font.Bold
            Layout.columnSpan: 2
        }
        Row {
            spacing: 20
            CustomButton {
                //% "Details"
                text: qsTrId("general-details")
                icon.source: "qrc:/assets/icon-details.svg"
                icon.width: 21
                icon.height: 14
                enabled: root.hasPaymentProof && !root.isSelfTx
                onClicked: {
                    showPaymentProof();
                }
            }
            CustomButton {
                //% "Copy"
                text: qsTrId("general-copy")
                icon.source: "qrc:/assets/icon-copy.svg"
                enabled: root.hasPaymentProof && !root.isSelfTx
                onClicked: {
                    copyPaymentProof();
                }
            }
        }
    }
}
