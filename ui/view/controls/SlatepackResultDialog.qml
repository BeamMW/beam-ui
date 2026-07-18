import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

ConfirmationDialog {
    id: thisDialog
    property bool   ok:        true
    property var    info:      ({})   // { amount, fee, isSend, unitName, icon, addressFrom, addressTo, txId }
    property string errorText: ""
    property var    vm:        null   // WalletViewModel, for confirm/cancel

    title: ok
        //% "Review transaction"
        ? qsTrId("slatepack-review-title")
        //% "Import failed"
        : qsTrId("slatepack-import-failed")
    okButtonText:        ok ? qsTrId("general-send") : qsTrId("general-close")
    okButtonColor:       Style.active
    okButtonIconSource:  ok ? "qrc:/assets/icon-send-blue.svg" : ""
    cancelButtonVisible:    ok
    cancelButtonText:       qsTrId("general-cancel")
    cancelButtonColor:      Style.validator_error
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    width: 600

    // Send confirms the previewed Slatepack (the transaction proceeds); Cancel discards it.
    onAccepted: { if (ok && vm) vm.commitSlatepack(info.txId || "") }
    onRejected: { if (ok && vm) vm.cancelSlatepack(info.txId || "") }

    contentItem: ColumnLayout {
        spacing: 16

        SvgImage {
            visible:              !thisDialog.ok
            Layout.alignment:     Qt.AlignHCenter
            Layout.topMargin:     10
            Layout.maximumHeight: 80
            Layout.maximumWidth:  80
            source:               "qrc:/assets/upload-error.svg"
        }

        // Success: direction + amount (colored, asset icon + ticker) + fee — styled after the
        // transaction details popup.
        ColumnLayout {
            visible:          thisDialog.ok
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            spacing:          10

            SFText {
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize:   14
                font.italic:      true
                color:            Style.white
                text: thisDialog.info.isSend
                    //% "You are sending"
                    ? qsTrId("slatepack-import-sending")
                    //% "You are receiving"
                    : qsTrId("slatepack-import-receiving")
            }

            Rectangle {
                visible:            thisDialog.ok && (thisDialog.info.txId || "").length > 0
                Layout.fillWidth:   true
                Layout.topMargin:   8
                Layout.leftMargin:  20
                Layout.rightMargin: 20
                height:             1
                color:              Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1)
            }

            GridLayout {
                visible:            thisDialog.ok && (thisDialog.info.txId || "").length > 0
                Layout.fillWidth:   true
                Layout.leftMargin:  20
                Layout.rightMargin: 20
                columns:            2
                columnSpacing:      16
                rowSpacing:         12

                // Amount
                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize:   14
                    color:            Style.content_secondary
                    text:             qsTrId("tx-details-amount-label") + ":"
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing:          2
                    BeamAmount {
                        amount:          thisDialog.info.amount   || "0"
                        unitName:        thisDialog.info.unitName || ""
                        iconSource:      thisDialog.info.icon     || ""
                        iconSize:        Qt.size(20, 20)
                        color:           thisDialog.info.isSend ? Style.accent_outgoing : Style.accent_incoming
                        prefix:          (thisDialog.info.amount || "0") == "0" ? "" : (thisDialog.info.isSend ? "- " : "+ ")
                        showTip:         false
                        maxUnitChars:    25
                        maxPaintedWidth: false
                        font {
                            styleName:  "Bold"
                            weight:     Font.Bold
                            pixelSize:  16
                        }
                    }
                    RowLayout {
                        visible: (thisDialog.info.assetId || 0) > 0
                        spacing: 8
                        SFText {
                            Layout.alignment: Qt.AlignVCenter
                            font.pixelSize:   12
                            color:            Style.content_secondary
                            text:             qsTrId("general-ca-id") + ": " + (thisDialog.info.assetId || 0)
                        }
                        OpenInBlockchainExplorer {
                            Layout.alignment: Qt.AlignVCenter
                            showText:         false
                            onTriggered: function(kernelID) {
                                var url = BeamGlobals.getExplorerUrl() + "assets/details/" + (thisDialog.info.assetId || 0);
                                Utils.openExternalWithConfirmation(url);
                            }
                        }
                    }
                }

                // Transaction fee
                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize:   14
                    color:            Style.content_secondary
                    text:             qsTrId("general-fee") + ":"
                }
                BeamAmount {
                    Layout.fillWidth: true
                    amount:           thisDialog.info.fee || "0"
                    unitName:         BeamGlobals.beamUnit
                    iconSource:       "qrc:/assets/icon-beam.svg"
                    iconSize:         Qt.size(20, 20)
                    iconAnchorCenter: false
                    showTip:          false
                    maxPaintedWidth:  false
                    color:            Style.white
                    font {
                        styleName:  "Bold"
                        weight:     Font.Bold
                        pixelSize:  14
                    }
                }

                // Sending address
                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize:   14
                    color:            Style.content_secondary
                    text:             qsTrId("tx-details-sending-addr-label") + ":"
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing:          8
                    SFLabel {
                        Layout.fillWidth: true
                        copyMenuEnabled:  true
                        font.pixelSize:   14
                        color:            Style.content_main
                        wrapMode:         Text.WrapAnywhere
                        text:             thisDialog.info.addressFrom || ""
                        onCopyText:       BeamGlobals.copyToClipboard(thisDialog.info.addressFrom || "")
                    }
                    CustomToolButton {
                        Layout.alignment:          Qt.AlignRight | Qt.AlignTop
                        icon.source:               "qrc:/assets/icon-copy.svg"
                        onClicked:                 BeamGlobals.copyToClipboard(thisDialog.info.addressFrom || "")
                        padding:                   0
                        background.implicitHeight: 16
                    }
                }

                // Receiving address
                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize:   14
                    color:            Style.content_secondary
                    text:             qsTrId("tx-details-receiving-addr-label") + ":"
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing:          8
                    SFLabel {
                        Layout.fillWidth: true
                        copyMenuEnabled:  true
                        font.pixelSize:   14
                        color:            Style.content_main
                        wrapMode:         Text.WrapAnywhere
                        text:             thisDialog.info.addressTo || ""
                        onCopyText:       BeamGlobals.copyToClipboard(thisDialog.info.addressTo || "")
                    }
                    CustomToolButton {
                        Layout.alignment:          Qt.AlignRight | Qt.AlignTop
                        icon.source:               "qrc:/assets/icon-copy.svg"
                        onClicked:                 BeamGlobals.copyToClipboard(thisDialog.info.addressTo || "")
                        padding:                   0
                        background.implicitHeight: 16
                    }
                }

                // Transaction ID
                SFText {
                    Layout.alignment: Qt.AlignTop
                    font.pixelSize:   14
                    color:            Style.content_secondary
                    text:             qsTrId("tx-details-tx-id-label") + ":"
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing:          8
                    SFLabel {
                        Layout.fillWidth: true
                        copyMenuEnabled:  true
                        font.pixelSize:   14
                        color:            Style.content_main
                        wrapMode:         Text.WrapAnywhere
                        text:             thisDialog.info.txId || ""
                        onCopyText:       BeamGlobals.copyToClipboard(thisDialog.info.txId || "")
                    }
                    CustomToolButton {
                        Layout.alignment:          Qt.AlignRight | Qt.AlignTop
                        icon.source:               "qrc:/assets/icon-copy.svg"
                        onClicked:                 BeamGlobals.copyToClipboard(thisDialog.info.txId || "")
                        padding:                   0
                        background.implicitHeight: 16
                    }
                }
            }
        }

        // Error: reason
        SFText {
            visible:             !thisDialog.ok
            Layout.fillWidth:    true
            Layout.leftMargin:   30
            Layout.rightMargin:  30
            font.pixelSize:      14
            font.italic:         true
            color:               Style.validator_error
            wrapMode:            Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text:                thisDialog.errorText
        }
    }
}
