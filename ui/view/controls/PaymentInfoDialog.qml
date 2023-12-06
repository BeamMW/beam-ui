import QtQuick 2.15
import QtQuick.Controls 2.15

import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "."

CustomDialog {
    PaymentInfoItem {
        id: model
    }

    property bool isModelValid: model ? model.isValid : false

    id: dialog
    modal: true

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: Overlay.overlay
    padding: 30

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onClosed: {
        paymentProofInput.text = ""
    }

    onOpened: {
        forceActiveFocus();
        paymentProofInput.forceActiveFocus();
    }

    contentItem: Item {
        height: contentLayout.height
        ColumnLayout {
            id: contentLayout
            spacing:    0

            onHeightChanged: {
                dialog.height = contentLayout.height + 2 * dialog.padding
            }

            RowLayout {
                id: headerLayout
                SFText {
                    Layout.fillWidth:       true
                    horizontalAlignment:    Text.AlignHCenter
                    leftPadding:            30
                    font.pixelSize:         18
                    font.styleName:         "Bold"
                    font.weight:            Font.Bold
                    color:                  Style.content_main
                    //% "Payment proof verification"
                    text: qsTrId("payment-info-proof-verification")
                }

                CustomToolButton {
                    icon.source: "qrc:/assets/icon-cancel-16.svg"
                    //% "Close"
                    ToolTip.text: qsTrId("general-close")
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.timeout: 2000
                    hoverEnabled: true
                    onClicked: {
                        dialog.close();
                    }
                }
            }

            ColumnLayout {
                id: verifyLayout
                Layout.fillWidth: true
                Layout.preferredWidth: 400
                Layout.topMargin:       20
                Layout.alignment: Qt.AlignTop
                SFText {
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    opacity: 0.5
                    font.pixelSize: 14
                    color: Style.content_main
                    //% "Paste your payment proof here"
                    text: qsTrId("payment-info-proof-label")
                }

                function isInvalidPaymentProof()
                {
                    return model && !model.isValid && paymentProofInput.length > 0;
                }

                ScrollView {
                    id: scrollView
                    clip: true
                    Layout.fillWidth: true
                    Layout.maximumHeight: 130

                    SFTextArea {
                        id: paymentProofInput
                        focus: true
                        activeFocusOnTab: true
                        font.pixelSize: 14
                        wrapMode: TextInput.Wrap
                        color: verifyLayout.isInvalidPaymentProof() ? Style.validator_error : Style.content_main
                        backgroundColor: verifyLayout.isInvalidPaymentProof() ? Style.validator_error : Style.content_main
                        text: model ? model.paymentProof : ""
                        Binding {
                            target: model
                            property: "paymentProof"
                            value: paymentProofInput.text.trim()
                        }
                    }
                }

                SFText {
                    Layout.fillWidth: true
                    font.pixelSize: 14
                    font.italic: true
                    //% "Cannot decode a proof, illegal sequence."
                    text: qsTrId("payment-info-proof-decode-fail")
                    color: Style.validator_error
                    visible: verifyLayout.isInvalidPaymentProof()
                }
            }

            GridLayout {
                id: detailsLayout
                Layout.fillWidth: true
                Layout.preferredWidth: 400
                Layout.topMargin:       20
                rowSpacing: 20
                columnSpacing: 13
                columns: 2
                visible: isModelValid

                SFText {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 10
                    Layout.columnSpan: 2
                    font.pixelSize: 18
                    font.styleName: "Bold";
                    font.weight: Font.Bold
                    color: Style.content_main
                    //% "Details"
                    text: qsTrId("general-details")
                }

                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize: 14
                    font.styleName: "Bold"
                    font.weight: Font.Bold
                    color: Style.content_main
                    //% "Sender"
                    text: qsTrId("payment-info-proof-sender-label") + ":"
                }

                SFText {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    font.pixelSize: 14
                    color: Style.content_disabled
                    text: model ? model.sender : ""
                    verticalAlignment: Text.AlignBottom
                }

                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize: 14
                    font.styleName: "Bold"
                    font.weight: Font.Bold
                    color: Style.content_main
                    //% "Receiver"
                    text: qsTrId("payment-info-proof-receiver-label") + ":"
                }

                SFText {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    font.pixelSize: 14
                    color: Style.content_disabled
                    text: model ? model.receiver : ""
                }

                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize: 14
                    font.styleName: "Bold"
                    font.weight: Font.Bold
                    color: Style.content_main
                    //% "Amount"
                    text: qsTrId("general-amount") + ":"
                }

                BeamAmount {
                    Layout.fillWidth: true
                    visible: model.amountValue ? model.amountValue : ""
                    amount: model ? model.amountValue : ""
                    unitName: model ? model.unitName : ""
                    color: Style.content_disabled
                    showTip: false
                    maxUnitChars: 30
                    font.pixelSize: 14
                }

                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize: 14
                    font.styleName: "Bold"
                    font.weight: Font.Bold
                    color: Style.content_main
                    //% "Kernel ID"
                    text: qsTrId("general-kernel-id") + ":"
                }

                SFText {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    font.pixelSize: 14
                    color: Style.content_disabled
                    text: model ? model.kernelID : ""
                }
            }

            Row {
                id: buttonsLayout
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin:       20
                spacing: 20
                visible: isModelValid

                PrimaryButton {
                    icon.source: "qrc:/assets/icon-copy-blue.svg"
                    //% "Copy details"
                    text: qsTrId("payment-info-copy-details-button")
                    visible: isModelValid
                    onClicked: {
                        if (model)
                        {
                            BeamGlobals.copyToClipboard(
                                "Sender: " + model.sender +
                                "\nReceiver: " + model.receiver +
                                "\nAmount: " + model.amountValue + " " + model.unitName +
                                "\nKernel ID: " + model.kernelID);
                        }
                    }
                }
            }
        }
    }
}