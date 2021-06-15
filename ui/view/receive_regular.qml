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
    id: control

    ReceiveViewModel {
        id: viewModel

        onNewAddressFailed: function () {
            var popup = Qt.createComponent("popup_message.qml").createObject(control)
            //% "You cannot generate new address. Your wallet doesn't have a master key."
            popup.message = qsTrId("can-not-generate-new-address-message")
            popup.open()
        }
    }

    property var defaultFocusItem: null
    property var onClosed: function () {} // set by parent

    property alias token:     viewModel.token
    property alias assetId:   viewModel.assetId
    property alias assetIdx:  amountInput.currencyIdx
    property var   assetInfo: viewModel.assetsList[control.assetIdx]

    function syncIdx () {
        for (var idx = 0; idx < viewModel.assetsList.length; ++idx) {
            if (viewModel.assetsList[idx].assetId == assetId) {
                 if (assetIdx != idx) {
                    assetIdx = idx
                 }
            }
        }
    }

    onAssetIdChanged: function () {
        // C++ sometimes provides asset id, combobox exepects index, need to fix this at some point
        syncIdx()
    }

    Component.onCompleted: function () {
        // asset id might be passed by other parts of the UI as a parameter to the receive view
        syncIdx()
    }

    Component.onDestruction: function () {
        viewModel.saveAddress()
    }

    TopGradient {
        mainRoot: main
        topColor: Style.accent_incoming
    }

    TokenInfoDialog {
        id:       tokenInfoDialog
        token:    viewModel.token
        incoming: true
    }

    function isValid () {
        return viewModel.commentValid
    }

    function copyAndClose() {
        if (isValid()) {
            BeamGlobals.copyToClipboard(viewModel.token)
            viewModel.saveAddress();
            control.onClosed()
        }
    }

    function copyAndSave() {
         if (isValid()) {
            BeamGlobals.copyToClipboard(viewModel.token)
            viewModel.saveAddress();
         }
    }

    //
    // Title row
    //
    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30

        //% "Receive"
        text: qsTrId("wallet-receive-title")
        onBack: function () {
            control.onClosed()
        }
    }

    QR {
        id: qrCode
        address: viewModel.token
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
                Layout.fillWidth: true
                spacing: 10

                //
                // Left column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    Panel {
                        //% "Transaction type"
                        title: qsTrId("general-tx-type")
                        Layout.fillWidth: true

                        content: ColumnLayout {
                            spacing: 20
                            id: addressType
                            property bool isShieldedSupported: statusbarModel.isConnectionTrusted && statusbarModel.isOnline

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
                                        checked:            !viewModel.isMaxPrivacy
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary
                                    }

                                    CustomButton {
                                        Layout.preferredHeight: 30
                                        Layout.minimumWidth: 137
                                        id: maxPrivacyCheck
                                        //% "Max Privacy"
                                        text:               qsTrId("tx-max-privacy")
                                        ButtonGroup.group:  txTypeGroup
                                        checkable:          true
                                        checked:            viewModel.isMaxPrivacy
                                        enabled:            addressType.isShieldedSupported
                                        hasShadow:          false
                                        radius:             16
                                        border.width:       1
                                        border.color:       checked ? Style.active : "transparent"
                                        palette.button:     checked ? Qt.rgba(0, 252/255, 207/255, 0.1) : "transparent"
                                        palette.buttonText: checked ? Style.active : Style.content_secondary
                                    }

                                    Binding {
                                        target:   viewModel
                                        property: "isMaxPrivacy"
                                        value:    maxPrivacyCheck.checked
                                    }
                                }
                            }

                            SFText {
                                Layout.fillWidth: true
                                visible:          !parent.isShieldedSupported
                                color:            Style.content_secondary
                                font.italic:      true
                                font.pixelSize:   14
                                wrapMode:         Text.WordWrap
                                //% "Connect to integrated or own node to enable receiving max privacy and offline transactions"
                                text: qsTrId("wallet-receive-max-privacy-unsupported")
                            }
                        }
                    }

                    //
                    // Requested amount
                    //
                    FoldablePanel {
                        //% "Requested amount"
                        title:             qsTrId("receive-request")
                        //% "(optional)"
                        titleTip:          qsTrId("receive-request-optional")
                        Layout.fillWidth:  true
                        folded:            false

                        //
                        // Amount
                        //
                        content: AmountInput {
                            id:          amountInput
                            amount:      viewModel.amount
                            currencies:  viewModel.assetsList
                            multi:       viewModel.assetsList.length > 1
                            resetAmount: false

                           onCurrencyIdxChanged: function () {
                               var idx = amountInput.currencyIdx
                               control.assetId = viewModel.assetsList[idx].assetId
                           }
                        }
                
                        Binding {
                            target:   viewModel
                            property: "amount"
                            value:    amountInput.amount
                        }
                    }

                    //
                    // Comment
                    //
                    FoldablePanel {
                        //% "Comment"
                        title:            qsTrId("general-comment")
                        Layout.fillWidth: true
                        folded:           false

                        content: ColumnLayout {
                            spacing: 0

                            SFTextInput {
                                id:               commentInput
                                font.pixelSize:   14
                                Layout.fillWidth: true
                                font.italic :     !viewModel.commentValid
                                backgroundColor:  viewModel.commentValid ? Style.content_main : Style.validator_error
                                color:            viewModel.commentValid ? Style.content_main : Style.validator_error
                                focus:            true
                                text:             viewModel.comment
                                placeholderText:  qsTrId("general-comment-local")
                                maximumLength:    BeamGlobals.maxCommentLength()
                            }
                 
                            Binding {
                                target:   viewModel
                                property: "comment"
                                value:    commentInput.text
                            }
                 
                            Item {
                                Layout.fillWidth: true
                                SFText {
                                    //% "Address with the same comment already exists"
                                    text:           qsTrId("general-addr-comment-error")
                                    color:          Style.validator_error
                                    font.pixelSize: 12
                                    visible:        !viewModel.commentValid
                                    font.italic:    true
                                }
                            }
                        }
                    }
                }

                //
                // Right column
                //
                ColumnLayout {
                    Layout.alignment:       Qt.AlignTop
                    Layout.fillWidth:       true
                    Layout.preferredWidth:  400
                    spacing:                10

                    Panel {
                        //% "Address"
                        title: qsTrId("receive-addr")
                        Layout.fillWidth: true

                        content: ColumnLayout {
                            spacing: 12

                            RowLayout {
                                spacing:          0
                                Layout.alignment: Qt.AlignHCenter

                                Image {
                                    Layout.preferredWidth:  270
                                    Layout.preferredHeight: 270
                                    fillMode:               Image.PreserveAspectFit
                                    source:                 qrCode.data
                                }
                            }

                            RowLayout {
                                spacing: 0

                                ColumnLayout {
                                    spacing: 0

                                    Layout.fillWidth:   true
                                    Layout.leftMargin:  20
                                    Layout.rightMargin: 2
                                    Layout.alignment:   Qt.AlignVCenter

                                    SFText {
                                        Layout.fillWidth:   true
                                        text:  viewModel.token
                                        width: parent.width
                                        color: Style.content_main
                                        elide: Text.ElideMiddle
                                    }

                                    LinkButton {
                                        Layout.alignment: Qt.AlignHCenter
                                        //% "More details"
                                        text:       qsTrId("more-details")
                                        linkColor:  Style.accent_incoming
                                        onClicked:  function () {
                                            tokenInfoDialog.open()
                                        }
                                    }
                                }

                                SvgImage {
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.bottomMargin: 15

                                    source: "qrc:/assets/icon-copy.svg"
                                    sourceSize: Qt.size(16, 16)
                                    opacity: control.isValid() ? 1.0 : 0.45

                                    MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton
                                    cursorShape: control.isValid() ? Qt.PointingHandCursor : Qt.ArrowCursor
                                    onClicked: function () {
                                            control.copyAndSave()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            CustomButton {
                id: copyButton
                Layout.topMargin:       30
                Layout.alignment:       Qt.AlignHCenter
                //% "copy and close"
                text:                   qsTrId("general-copy-and-close")
                Layout.preferredHeight: 38
                palette.buttonText:     Style.content_opposite
                icon.color:             Style.content_opposite
                palette.button:         Style.accent_incoming
                icon.source:            "qrc:/assets/icon-copy.svg"
                enabled:                control.isValid()

                onClicked: function () {
                    control.copyAndClose()
                }
            }

            //
            // Footers
            //
            SFText {
                property string mpLockTimeLimit: viewModel.mpTimeLimit
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 428
                Layout.topMargin:      15
                font.pixelSize:        14
                font.italic:           true
                color:                 Style.content_disabled
                wrapMode:              Text.WordWrap
                horizontalAlignment:   Text.AlignHCenter
                visible:               viewModel.isMaxPrivacy
                text: (mpLockTimeLimit != "0" ?
                    //% "Max Privacy transaction can last at most %1 hours."
                    qsTrId("wallet-receive-addr-message-mp").arg(mpLockTimeLimit) :
                    //% "Max Privacy transaction can last indefinitely."
                    qsTrId("wallet-receive-addr-message-mp-no-limit")) + "\n" +
                    //% "Min transaction fee to send Max privacy coins is %1."
                    qsTrId("wallet-receive-addr-message").arg("~%1 BEAM".arg(Utils.uiStringToLocale("0.01")))
            }

            SFText {
                Layout.alignment:      Qt.AlignHCenter
                Layout.preferredWidth: 400
                Layout.topMargin:      15
                Layout.bottomMargin:   50
                font.pixelSize:        14
                font.italic:           true
                color:                 Style.content_disabled
                wrapMode:              Text.WordWrap
                horizontalAlignment:   Text.AlignHCenter
                //% "Sender will be given a choice between regular and offline payment. For the regular transaction to complete, you should get online during the 12 hours after coins are sent."
                text: qsTrId("wallet-receive-text-online-time")
                visible:               !viewModel.isMaxPrivacy
            }
        }  // ColumnLayout
    }  // ScrollView
}
