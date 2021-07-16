import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "../controls"
import "."

RowLayout {
    id: "control"
    property var sendAddress
    property var receiveAddress
    property var senderIdentity
    property var receiverIdentity
    property var txID
    property var kernelID
    property var comment
    property var isContractTx
    property var isShieldedTx
    property var searchRegExp2:  new RegExp("("+control.searchFilter+")", "i")
    property string token

    property string  searchFilter: ""
    property bool    hideFiltered: false

    signal textCopied(string text)

    spacing: 0

    function isFieldVisible() {
        return control.searchFilter.length == 0 || hideFiltered == false;
    }

    function isTextFieldVisible(text) {
        return isFieldVisible()
        || (control.searchFilter.length > 0 && text.search(control.searchRegExp2) >= 0);
    }

    function getHighlitedText(text) {
        return Utils.getHighlitedText(text, control.searchFilter, Style.active.toString());
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
            text: getHighlitedText(control.sendAddress)
            onCopyText: textCopied(control.sendAddress)
            visible: isTextFieldVisible(control.sendAddress) && control.sendAddress.length
        }

        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary
            //% "Sender wallet's signature"
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
            text: getHighlitedText(control.senderIdentity)
            onCopyText: textCopied(control.senderIdentity)
            visible: control.senderIdentity.length > 0 && (control.receiverIdentity.length > 0 || control.isShieldedTx ) && isTextFieldVisible(control.senderIdentity)
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
            property var receiveAddressOrToken : hasToken ? control.token : control.receiveAddress
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
            //% "Receiver wallet's signature"
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
            text: getHighlitedText(control.receiverIdentity)
            onCopyText: textCopied(control.receiverIdentity)
            visible: control.senderIdentity.length > 0 && control.receiverIdentity.length > 0 && isTextFieldVisible(control.receiverIdentity)
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
            text: getHighlitedText(control.txID)
            elide: Text.ElideMiddle
            onCopyText: textCopied(control.txID)
            visible: isTextFieldVisible(control.txID) && !Utils.isZeroed(control.txID)
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
            text: getHighlitedText(control.kernelID)
            elide: Text.ElideMiddle
            onCopyText: textCopied(control.kernelID)
            visible: isTextFieldVisible(control.kernelID) && !Utils.isZeroed(control.kernelID)
        }
        SFText {
            Layout.alignment: Qt.AlignTop
            font.pixelSize: 14
            color: Style.content_secondary

            text: control.isContractTx ?
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
            text: getHighlitedText(control.comment)
            elide: Text.ElideRight
            onCopyText: textCopied(control.comment)
            visible: isTextFieldVisible(control.comment)
        }
    }
}
