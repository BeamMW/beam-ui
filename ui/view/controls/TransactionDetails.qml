import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

RowLayout {
    id: "root"
    property var sendAddress
    property var receiveAddress
    property var senderIdentity
    property var receiverIdentity
    property var fee
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
    property string token
    property string amount
    property string secondCurrencyRate
    property string secondCurrencyUnitName
    property string searchFilter: ""
    property bool hideFiltered: false
    property var searchRegExp: new RegExp("("+root.searchFilter+")", "gi")
    property var searchRegExp2:  new RegExp("("+root.searchFilter+")", "i")
    property string transactionType
    property string tokenType
    property bool isMaxPrivacy

    readonly property string amountPrefix: root.isIncome ? "+" : "-"
    readonly property string amountWithLabel: amountPrefix + " " + root.amount + " " + BeamGlobals.getCurrencyUnitName(Currency.CurrBeam)
    readonly property string secondCurrencyAmount: getAmountInSecondCurrency()

    property var onOpenExternal: null
    signal textCopied(string text)
    signal copyPaymentProof()
    signal showPaymentProof()

    spacing: 30

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

    function getAmountInSecondCurrency() {
        if (root.amount !== "") {
            var amountInSecondCurrency = Utils.formatAmountToSecondCurrency(
                root.amount,
                root.secondCurrencyRate,
                root.secondCurrencyUnitName);
            if (amountInSecondCurrency == "") {
                //% "Exchange rate to %1 was not available at the time of transaction"
                return  qsTrId("tx-details-exchange-rate-not-available").arg(root.secondCurrencyUnitName);
            }
            else {
                //% "(for the day of transaction)"
                return root.amountPrefix + " " + amountInSecondCurrency + " " + qsTrId("tx-details-second-currency-notification");
            }
        }
        else return "";
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
        columnSpacing: 44
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
            visible: isTextFieldVisible(root.sendAddress) && root.sendAddress.length
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
            visible: root.senderIdentity.length > 0 && (root.receiverIdentity.length > 0 || root.isMaxPrivacy ) && isTextFieldVisible(root.senderIdentity)
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

        // Transaction type:
        SFText {
            Layout.alignment:       Qt.AlignTop
            font.pixelSize:         14
            color:                  Style.content_secondary
            //% "Transaction type"
            text:                   qsTrId("token-info-transaction-type") + ":"
            visible:                isTextFieldVisible(root.transactionType)
        }
            
        SFText {
            Layout.fillWidth:       true
            wrapMode:               Text.Wrap
            font.pixelSize:         14
            color:                  Style.content_main
            text:                   root.transactionType
            verticalAlignment:      Text.AlignBottom
            visible:                isTextFieldVisible(root.transactionType)
        }

        // Address type
        SFText {
            Layout.alignment:       Qt.AlignTop
            font.pixelSize:         14
            color:                  Style.content_secondary
            //% "Address type"
            text:                   qsTrId("address-info-type") + ":"
            visible:                isTextFieldVisible(root.tokenType)
        }
            
        SFText {
            Layout.fillWidth:       true
            wrapMode:               Text.Wrap
            font.pixelSize:         14
            text:                   root.tokenType
            color:                  Style.content_main
            visible:                isTextFieldVisible(root.tokenType)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Amount"
            text: qsTrId("tx-details-amount-label") + ":"
            visible: amountField.visible
        }
        SFLabel {
            id: amountField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            font.styleName: "Bold"; font.weight: Font.Bold
            color: root.isIncome ? Style.accent_incoming : Style.accent_outgoing
            elide: Text.ElideMiddle
            text: root.amountWithLabel
            onCopyText: textCopied(root.amount)
            visible: isTextFieldVisible(root.amount)
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Currency amount"
            text: qsTrId("tx-details-second-currency-amount-label") + ":"
            visible: secondCurrencyAmountField.visible
        }
        SFLabel {
            id: secondCurrencyAmountField
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            text: root.secondCurrencyAmount
            onCopyText: textCopied(secondCurrencyAmountField.text)
            visible: isTextFieldVisible(secondCurrencyAmountField.text) && root.secondCurrencyUnitName != ""
        }
        
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Transaction fee"
            text: qsTrId("general-fee") + ":"
            visible: root.isFieldVisible() && root.fee.length
        }
        SFLabel {
            Layout.fillWidth: true
            copyMenuEnabled: true
            font.pixelSize: 14
            color: Style.content_main
            text: root.fee
            onCopyText: textCopied(text)
            visible: root.isFieldVisible() && root.fee.length
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
            font.styleName: "Italic"
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
            font.styleName: "Italic"
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
            font.styleName: "Italic"
            elide: Text.ElideMiddle
            onCopyText: textCopied(root.kernelID)
            visible: isTextFieldVisible(root.kernelID) && !isZeroed(root.kernelID)
        }

        function canOpenInBlockchainExplorer(status) {
            switch(status) {
                case "completed":
                case "received":
                case "sent":
                    return true;
                default:
                    return false;
            }
        }
        
        Item {
            Layout.preferredHeight: 16
            visible: parent.canOpenInBlockchainExplorer(root.status) && root.isFieldVisible() && kernelID.visible
        }
        Item {
            Layout.preferredWidth: openInExplorer.width + 10 + openInExplorerIcon.width
            Layout.preferredHeight: 16
            visible: parent.canOpenInBlockchainExplorer(root.status) && root.isFieldVisible() && kernelID.visible
        
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
            font.styleName: "Italic"
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
