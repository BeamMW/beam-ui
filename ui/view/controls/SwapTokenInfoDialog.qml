import QtQuick 2.11
import QtQuick.Controls 2.4

import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: dialog
    modal: true
        
    property alias token:           viewModel.token
    property alias expirationTime:  viewModel.expirationTime
    property string rate:           ""
    property string offered:        ""
    property string expires:        ""
    property bool copyTokenEnabled: true
    
    SwapTokenInfoItem {
        id:     viewModel
    }
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    
    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onClosed: {
        
    }

    onOpened: {
        forceActiveFocus();
    }

    contentItem: Item {
        ColumnLayout {
            GridLayout {
                id: contentLayout
                Layout.margins:         30
                rowSpacing:             20
                columnSpacing:          16
                columns:                2

                RowLayout {
                    Layout.columnSpan: 2
                    SFText {
                        Layout.fillWidth:   true
                        horizontalAlignment:Text.AlignHCenter
                        leftPadding:        30
                        font.pixelSize:     18
                        font.styleName:     "Bold"
                        font.weight:        Font.Bold
                        color:              Style.content_main
                        text:               qsTrId("send-swap-token")
                    }
                }

                // Send amount
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    text:                   qsTrId("sent-amount-label") + ":"
                    visible:                viewModel.sendAmount.length
                }

                SFLabel {
                    Layout.fillWidth:       true
                    copyMenuEnabled:        true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   viewModel.sendAmount
                    visible:                viewModel.sendAmount.length
                }

                // Receive amount:
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    text:                   qsTrId("receive-amount-swap-label") + ":"
                    visible:                viewModel.receiveAmount.length
                }

                SFText {
                    Layout.fillWidth:       true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   viewModel.receiveAmount
                    verticalAlignment:      Text.AlignBottom
                    visible:                viewModel.receiveAmount.length
                }

                // Exchange rate:
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    text:                   qsTrId("general-rate") + ":"
                    visible:                viewModel.exchangeRate.length
                }

                SFLabel {
                    Layout.fillWidth:       true
                    copyMenuEnabled:        true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   rate
                    onCopyText:             BeamGlobals.copyToClipboard(text)
                    visible:                viewModel.exchangeRate.length
                }

                // Expiration time:
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    //% "Expiration time"
                    text:                   qsTrId("swap-expiration-time") + ":"
                    visible:                viewModel.expirationTime.length
                }

                SFLabel {
                    Layout.fillWidth:       true
                    copyMenuEnabled:        true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   viewModel.expirationTime
                    onCopyText:             BeamGlobals.copyToClipboard(text)
                    visible:                viewModel.expirationTime.length
                }

                // Offered on:
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    text:                   qsTrId("wallet-send-swap-offered-label") + ":"
                    visible:                offered.length
                }

                SFLabel {
                    Layout.fillWidth:       true
                    copyMenuEnabled:        true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   offered
                    onCopyText:             BeamGlobals.copyToClipboard(text)
                    visible:                offered.length
                }

                // Expires on:
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    text:                   qsTrId("wallet-send-swap-expires-label") + ":"
                    visible:                expires.length
                }

                SFLabel {
                    Layout.fillWidth:       true
                    copyMenuEnabled:        true
                    wrapMode:               Text.Wrap
                    font.pixelSize:         14
                    color:                  Style.content_main
                    text:                   expires
                    onCopyText:             BeamGlobals.copyToClipboard(text)
                    visible:                expires.length
                }

                // Token
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    //% "Token"
                    text:                   qsTrId("token-info-token") + ":"
                    visible:                viewModel.token != viewModel.address
                }

                ScrollView {
                    Layout.fillWidth:             true
                    Layout.preferredWidth:        578
                    Layout.maximumHeight:         200
                    clip:                         true
                    ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy:    ScrollBar.AsNeeded
                    visible:                      viewModel.token != viewModel.address
                    SFLabel {
                        //Layout.fillWidth:       true
                        //Layout.preferredWidth:  578
                        width:                    578
                        copyMenuEnabled:          dialog.copyTokenEnabled
                        wrapMode:                 Text.Wrap
                        font.pixelSize:           14
                        color:                    Style.content_main
                        text:                     viewModel.token
                        onCopyText:               BeamGlobals.copyToClipboard(text)
                    }
                }
            }
            Row {
                id:                     buttonsLayout
                Layout.alignment:       Qt.AlignHCenter
                Layout.leftMargin:      30
                Layout.rightMargin:     30
                Layout.bottomMargin:    30
                Layout.preferredHeight: 38
                spacing:                20

                CustomButton {
                    icon.source:    "qrc:/assets/icon-cancel-16.svg"
                    //% "Close"
                    text:           qsTrId("general-close")
                    onClicked: {
                        dialog.close()
                    }
                }

                PrimaryButton {
                    visible:            dialog.copyTokenEnabled
                    icon.source:        "qrc:/assets/icon-copy-blue.svg"
                    palette.buttonText: Style.content_opposite
                    icon.color:         Style.content_opposite
                    palette.button:     Style.accent_incoming
                    //% "copy and close"
                    text:               qsTrId("general-copy-and-close")
                    onClicked: {
                        BeamGlobals.copyToClipboard(viewModel.token);
                        dialog.close();
                    }
                }
            }
        }
    }
}