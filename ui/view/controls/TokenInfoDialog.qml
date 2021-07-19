import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: dialog
    modal: true
        
    property alias token:                 viewModel.token
    property alias ignoreStoredVouchers:  viewModel.ignoreStoredVouchers
    property bool  incoming:              true
    
    TokenInfoItem {
        id:  viewModel
    }

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    onOpened: {
        forceActiveFocus();
    }

    contentItem: Item {
        ColumnLayout {
            id: contentColumn

            GridLayout {
                id: contentGrid
                Layout.margins:         30
                rowSpacing:             25
                columnSpacing:          25
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

                        text: viewModel.isMaxPrivacy ?
                                //% "Address details with max anonymity set"
                                qsTrId("address-info-title-mp") :
                                //% "address details"
                                viewModel.transactionType + " " + qsTrId("address-info-title")
                    }
                }

                // Address
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    //% "Address"
                    text:                   qsTrId("address-info-address") + ":"
                    visible:                !viewModel.isMaxPrivacy
                }

                RowLayout {
                    Layout.fillWidth: true

                    ScrollView {
                        Layout.preferredWidth:        578
                        Layout.maximumHeight:         200
                        clip:                         true
                        ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy:    ScrollBar.AsNeeded

                        Column {
                            spacing: 10

                            SFLabel {
                                width:                    578
                                copyMenuEnabled:          true
                                wrapMode:                 Text.Wrap
                                font.pixelSize:           14
                                color:                    Style.content_main
                                text:                     viewModel.token
                                onCopyText: function () {
                                    BeamGlobals.copyToClipboard(text)
                                }
                            }

                            SFText {
                                width:          578
                                font.pixelSize: 14
                                font.italic:    true
                                color:          Style.content_disabled
                                //% "Regular address includes both online and offline addresses."
                                text: qsTrId("address-info-regular-notice")
                            }
                        }
                    }

                    CustomToolButton {
                        Layout.alignment:       Qt.AlignTop
                        Layout.leftMargin:      4
                        Layout.topMargin:       -8
                        icon.source:            "qrc:/assets/icon-copy-blue.svg"
                        //% "Copy"
                        ToolTip.text:           qsTrId("general-copy")
                        onClicked: function () {
                            BeamGlobals.copyToClipboard(viewModel.token)
                        }
                    }
                }

                Rectangle {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    height:  1
                    color:   "white"
                    opacity: 0.1
                    visible: sbbsAdrrCtrl.visible
                }

                // SBBS Address
                SFText {
                    Layout.alignment:       Qt.AlignTop
                    font.pixelSize:         14
                    color:                  Style.content_disabled
                    //% "Online (SBBS) Address"
                    text:                   qsTrId("address-info-sbbs-address") + ":"
                    visible:                sbbsAdrrCtrl.visible
                }

                RowLayout {
                    id: sbbsAdrrCtrl
                    Layout.fillWidth:  true
                    visible:           viewModel.address.length && !viewModel.isMaxPrivacy

                    SFLabel {
                        Layout.alignment:       Qt.AlignTop
                        Layout.preferredWidth:  578
                        wrapMode:               Text.Wrap
                        font.pixelSize:         14
                        color:                  Style.content_main
                        text:                   viewModel.address
                        copyMenuEnabled:        true
                        elide:                  Text.ElideMiddle
                        onCopyText: function () {
                            BeamGlobals.copyToClipboard(text)
                        }
                    }

                    CustomToolButton {
                        Layout.alignment:       Qt.AlignTop
                        Layout.leftMargin:      4
                        Layout.topMargin:       -8
                        icon.source:            "qrc:/assets/icon-copy-blue.svg"
                        //% "Copy"
                        ToolTip.text:           qsTrId("general-copy")
                        onClicked: function () {
                            BeamGlobals.copyToClipboard(viewModel.address)
                        }
                    }
                }
            }

            Row {
                id:                     buttonsLayout
                Layout.fillHeight:      true
                Layout.alignment:       Qt.AlignHCenter
                Layout.leftMargin:      30
                Layout.rightMargin:     30
                Layout.bottomMargin:    30
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
                    icon.source:        "qrc:/assets/icon-copy-blue.svg"
                    palette.buttonText: Style.content_opposite
                    icon.color:         Style.content_opposite
                    palette.button:     dialog.incoming ? Style.accent_incoming : Style.accent_outgoing
                    //% "copy address and close"
                    text:               qsTrId("address-info-copy-and-close")
                    onClicked: {
                        BeamGlobals.copyToClipboard(viewModel.token);
                        dialog.close();
                    }
                }
            }
        }
    }
}
