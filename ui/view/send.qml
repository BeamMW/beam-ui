import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "controls"

ColumnLayout {
    id: sendView
    property var    defaultFocusItem: receiverTAInput
    property bool isValid: !receiverTAInput.text || BeamGlobals.isSwapToken(receiverTAInput.text)

    // callbacks set by parent
    property var    onClosed: function() {}
    property var    onSwapToken: function() {}

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    //
    // Title row
    //
    Item {
        Layout.fillWidth:    true
        Layout.topMargin:    100
        Layout.bottomMargin: 30
        CustomButton {
            anchors.left:   parent.left
            anchors.verticalCenter: parent.verticalCenter
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true
            //% "Back"
            text:           qsTrId("general-back")
            icon.source:    "qrc:/assets/icon-back.svg"
            onClicked:      {
                onClosed();
            }
        }

        
        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter:   parent.verticalCenter
            color:                    Style.content_main
            font {
                styleName:      "Bold"
                weight:         Font.Bold
                pixelSize:      14
                letterSpacing:  4
                capitalization: Font.AllUppercase
            }
            //% "Accept Swap Offer"
            text:               qsTrId("wallet-send-swap-title")
        }
    } // Item

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
            // Transaction token
            //
            Panel {
                title:                   qsTrId("token-info-title")
                Layout.fillWidth:        true
                content:
                ColumnLayout {
                    SFTextInput {
                        Layout.fillWidth: true
                        id:               receiverTAInput
                        font.pixelSize:   14
                        color:            isValid ? Style.content_main : Style.validator_error
                        backgroundColor:  isValid ? Style.content_main : Style.validator_error
                        font.italic:      !isValid
                        validator:        RegExpValidator { regExp: /[0-9a-zA-Z]{1,}/ }
                        selectByMouse:    true
                        //% "Paste token here"
                        placeholderText:  qsTrId("send-swap-token-hint")

                        onTextPasted: {
                            if (BeamGlobals.isSwapToken(text)) {
                                onSwapToken(text)
                            }
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        SFText {
                            Layout.alignment: Qt.AlignTop
                            id:               receiverTAError
                            color:            Style.validator_error
                            font.pixelSize:   12
                            font.italic:      true
                            visible:          !isValid
                            //% "Invalid swap token"
                            text:             qsTrId("wallet-send-invalid-token")
                        }
                    }
                }
            }
        }
        Item {
            Layout.fillWidth:       true
            Layout.preferredWidth:  400
        }
    } // RowLayout

    //CustomButton {
    //    Layout.alignment:    Qt.AlignHCenter
    //    Layout.topMargin:    30
    //    Layout.bottomMargin: 30
    //    id: actionButton
    //        //% "Swap"
    //    text:               qsTrId("general-swap")
    //    palette.buttonText: Style.content_opposite
    //    palette.button:     Style.accent_outgoing
    //    icon.source:        "qrc:/assets/icon-create-offer.svg"
    //    enabled:            receiverTAInput.text && isValid
    //    onClicked:          onSwapToken(receiverTAInput.text)
    //}
    Item {
        Layout.fillHeight: true
    }
} // ColumnLayout
