import QtQuick          2.15
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import Beam.Wallet      1.0
import "../controls"

CustomDialog {
    id:          control
    width:       600
    height:      300
    x:           Math.round((parent.width - width) / 2)
    y:           Math.round((parent.height - height) / 2)
    parent:      Overlay.overlay
    modal:       true
    focus:       true

    property alias name: model.name
    property alias peerID: model.peerID

    MessengerAddressAdd {
        id: model
    }

    contentItem: ColumnLayout {
        spacing: 0
        anchors.fill:    parent
        anchors.margins: 30

        // title
        SFText {
            Layout.fillWidth:     true
            color:                Style.white
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       18
            font.weight:          Font.Bold
            //% "Add receiver address"
            text: qsTrId("messenger-add-receiver-address-dialog-title")
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 31
            rowSpacing: 20

            Row {
                SFText {
                    //% "Address"
                    text: qsTrId("messenger-add-receiver-address-address")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }
                SFText {
                    text: "*"
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                    verticalAlignment: TextInput.AlignTop
                }
            }

            SFTextInput {
                Layout.fillWidth: true
                id: addressInput
                width: 335
                height: 45
                color:            model.error ? Style.validator_error : Style.content_main
                backgroundColor:  model.error ? Style.validator_error : Style.content_main
                text:             model.address
                focus:            true
                validator:        RegExpValidator { regExp: /[0-9a-zA-Z]{1,}/ }
                Binding {
                    target:   model
                    property: "address"
                    value:    addressInput.text
                }
            }

            Row {
                SFText {
                    //% "Name"
                    text: qsTrId("messenger-add-receiver-address-name")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }
            }

            SFTextInput {
                Layout.fillWidth: true
                id: nameInput
                width: 335
                height: 45
                color: Style.content_main
                text: model.name
                Binding {
                    target:   model
                    property: "name"
                    value:    nameInput.text
                }
            }
        }

        CustomButton {
            Layout.alignment: Qt.AlignHCenter
            palette.button: Style.active
            icon.source: "qrc:/assets/icon-done.svg"
            //% "add receiver address"
            text: qsTrId("messenger-add-receiver-address-save")
            palette.buttonText: Style.content_opposite
            enabled: addressInput.text.length != 0 && !model.error
            onClicked: {
                model.saveAddress();
                control.close();
            }
        }
    }
}
