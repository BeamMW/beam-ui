import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ConfirmationDialog {
    onVisibleChanged: {
        if (!this.visible) {
            this.destroy();
        }
    }

    id: control
    parent: Overlay.overlay
    defaultFocusItem: BeamGlobals.needPasswordToSpend() ? requirePasswordInput : cancelButton

    property bool   createAssetSwap: false
    property string receiveUnitName: ""
    property string receiveAmount: "0"
    property string sendUnitName: ""
    property string sendAmount: "0"
    property string fee:       "0"

    okButtonText: control.createAssetSwap ?
                    //% "Create asset swap"
                    qsTrId("assets-swap-create-confirm-dialog-title"):
                    //% "Accept asset swap"
                    qsTrId("assets-swap-accept-confirm-dialog-title")

    okButtonColor:           Style.accent_outgoing
    okButtonIconSource:      "qrc:/assets/icon-send-blue.svg"
    okButtonEnable:          BeamGlobals.needPasswordToSpend() ? requirePasswordInput.text.length : true
    cancelButtonIconSource:  "qrc:/assets/icon-cancel-white.svg"

    beforeAccept: function () {
        if (BeamGlobals.needPasswordToSpend()) {
            if (requirePasswordInput.text.length == 0) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                return false
            }
            if (!BeamGlobals.isPasswordValid(requirePasswordInput.text)) {
                requirePasswordInput.forceActiveFocus(Qt.TabFocusReason);
                requirePasswordInput.selectAll();
                requirePasswordError.text = qsTrId("general-pwd-invalid");
                return false
            }
        }
        return true
    }

    function passworInputEnter() {
        okButton.forceActiveFocus(Qt.TabFocusReason);
        okButton.clicked();
    }

    function getFeeInSecondCurrency(feeValue) {
        return Utils.formatFeeToSecondCurrency(
            feeValue,
            control.rate,
            control.rateUnit)
    }

    topPadding: 30
    contentItem:
    ColumnLayout {
        spacing: 30

        SFText {
            id: title
            Layout.alignment:   Qt.AlignHCenter
            font.pixelSize:     18
            font.styleName:     "Bold";
            font.weight:        Font.Bold
            color:              Style.content_main
            text:               control.createAssetSwap ?
                                //% "Confirm asset swap creation"
                                qsTrId("asset-swap-confirmation-create") :
                                //% "Accept asset swap"
                                qsTrId("asset-swap-confirmation-accept")
        }

        GridLayout {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            columnSpacing:          14
            rowSpacing:             14
            columns:                2

            //
            // Send amount
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                Layout.topMargin:       8
                font.pixelSize:         14
                color: Style.content_disabled
                //% "Send amount"
                text: qsTrId("assets-swap-confirm-dialog-send-amount") + ":"
                verticalAlignment: Text.AlignTop
            }

            BeamAmount {
                visible: true
                spacing: 15
                font.pixelSize:    24
                font.styleName:    "Bold"
                font.weight:       Font.Bold
                rateFontSize:      14
                copyMenuEnabled:   true
                unitName:          control.sendUnitName
                amount:            control.sendAmount
                color:             Style.accent_outgoing
                maxPaintedWidth:   false
                maxUnitChars:      15
                Layout.fillWidth:  true
            }

            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                Layout.topMargin:       8
                font.pixelSize:         14
                color: Style.content_disabled
                //% "Receive amount"
                text: qsTrId("assets-swap-confirm-dialog-receive-amount") + ":"
                verticalAlignment: Text.AlignTop
            }

            BeamAmount {
                visible: true
                spacing: 15
                font.pixelSize:    24
                font.styleName:    "Bold"
                font.weight:       Font.Bold
                rateFontSize:      14
                copyMenuEnabled:   true
                unitName:          control.receiveUnitName
                amount:            control.receiveAmount
                color:             Style.accent_incoming
                maxPaintedWidth:   false
                maxUnitChars:      15
                Layout.fillWidth:  true
            }

            //
            // Fee
            //
            SFText {
                Layout.fillWidth:       true
                Layout.alignment:       Qt.AlignTop
                font.pixelSize:         14
                visible:                !control.createAssetSwap
                color:                  Style.content_disabled
                //% "Asset swap fee"
                text:                   qsTrId("assets-swap-confirm-dialog-fee-label")
                verticalAlignment:      Text.AlignTop
            }

            ColumnLayout {
                Layout.fillWidth:   true
                visible:            !control.createAssetSwap
                SFText {
                    id:              feeLabel
                    font.pixelSize:  14
                    color:           Style.content_main
                    text:            control.fee
                }
            }

            //
            // Password confirmation
            //
            SFText {
                id:                     requirePasswordLabel
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.topMargin:       16
                horizontalAlignment:    Text.AlignHCenter
                Layout.fillWidth:       true
                Layout.minimumHeight:   16
                font.pixelSize:         14
                color:                  Style.content_main
                //% "To approve the transaction please enter your password"
                text:                   qsTrId("send-confirmation-pwd-require-message")
            }

            PasswordInput {
                id:                     requirePasswordInput
                visible:                BeamGlobals.needPasswordToSpend()
                Layout.columnSpan:      2
                Layout.fillWidth:       true
                focus:                  true
                activeFocusOnTab:       true
                font.pixelSize:         14
                color:                  Style.content_main
                hasError:               requirePasswordError.text.length > 0
                onAccepted:             passworInputEnter()
                onTextChanged:          if (requirePasswordError.text.length > 0) requirePasswordError.text = ""
            }

            SFText {
                Layout.fillWidth:       true
                Layout.columnSpan:      2
                id:                     requirePasswordError
                visible:                BeamGlobals.needPasswordToSpend()
                color:                  Style.validator_error
                font.pixelSize:         14
            }
        }
    }
}
