import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27

    property var appsList: undefined
    readonly property bool hasApps: !!appsList && appsList.length > 0
    property var onBack
    property var addPublisherByKey

    function showAddPublisherDialog() {
        addPublisherDialog.open()
    }

    //
    // Page Header (Back button + title + add publisher button)
    //
    RowLayout {
        id: header

        CustomButton {
            id:             backButton
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true

            font {
                styleName: "DemiBold"
                weight:    Font.DemiBold
            }

            //% "Back"
            text:        qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            visible:     true

            onClicked:   control.onBack()
        }

        SFText {
            Layout.fillWidth:     true
            color:                Style.content_main
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            font.weight:          Font.Bold
            font.capitalization:  Font.AllUppercase
            //% "Publishers that i follow"
            text: qsTrId("dapps-store-publishers-page-title")
        }

        CustomButton {
            Layout.alignment: Qt.AlignRight
            palette.button:   Qt.rgba(255, 255, 255, 0.1)
            //% "add publisher"
            text: qsTrId("dapps-store-add-publisher")
            // TODO: change icon
            icon.source:      "qrc:/assets/icon-dapps_store-publishers.svg"
            icon.color:       Style.white
            onClicked:        showAddPublisherDialog()
        }
    }

    //
    // Body: publishers list
    //
    CustomTableView {
        id: tableView
        Layout.fillHeight: true
        Layout.fillWidth:  true
        // TODO: implement
    }

    CustomDialog {
        id:      addPublisherDialog
        modal:   true
        x:       (parent.width - width) / 2
        y:       (parent.height - height) / 2
        parent:  Overlay.overlay

        onOpened: {
            forceActiveFocus()
        }

        onClosed: {
            publicKeyInput.text = ""
        }

        contentItem: ColumnLayout {
            spacing: 0

            // title
            SFText {
                Layout.fillWidth:    true
                Layout.topMargin:    40
                Layout.bottomMargin: 30
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      18
                color:               Style.content_main
                //% "Add publisher"
                text:                qsTrId("dapps-store-add-publisher-title")
            }

            // Note
            SFText {
                id:                  addressField
                Layout.bottomMargin: 30
                Layout.fillWidth:    true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      14
                color:               Style.content_main
                //% "To add a publisher you need to paste his Publisher Key in the field below"
                text:                qsTrId("dapps-store-add-publisher-note")
            }

            // input
            SFTextInput {
                id:                    publicKeyInput
                Layout.bottomMargin:   publicKeyError.visible ? 6 : 30
                Layout.leftMargin:     100
                Layout.rightMargin:    100
                Layout.fillWidth:      true
                Layout.preferredWidth: 560
                font.pixelSize:        14
                color:                 publicKeyError.visible ? Style.validator_error : Style.content_main
                backgroundColor:       publicKeyError.visible ? Style.validator_error : Style.content_main
                leftPadding:           15
                rightPadding:          15
                validator:             RegExpValidator { regExp: /[0-9a-fA-F]{66}/ }

                background: Rectangle {
                    color:   publicKeyError.visible ? Style.validator_error : Style.white
                    opacity: 0.1
                    radius:  10
                }
                onTextChanged: resetError()

                function resetError() {
                    publicKeyError.visible = false
                }
            }

            SFText {
                id:                  publicKeyError
                visible:             false
                Layout.bottomMargin: 20
                Layout.leftMargin:   100
                color:               Style.validator_error
                font.pixelSize:      12
                font.italic:         true
                //% "Publisher with that Key is not found"
                text:                qsTrId("dapps-store-add-publisher-error")
            }

            Row {
                id:                  buttonsLayout
                Layout.fillHeight:   true
                Layout.bottomMargin: 30
                Layout.alignment:    Qt.AlignHCenter
                spacing:             30
        
                CustomButton {
                    icon.source: "qrc:/assets/icon-cancel-16.svg"
                    //% "Close"
                    text:        qsTrId("general-close")
                    onClicked: {
                        addPublisherDialog.close()
                    }
                }
            
                PrimaryButton {
                    enabled:     !publicKeyError.visible && publicKeyInput.acceptableInput
                    icon.source: "qrc:/assets/icon-dapps_store-add-publisher-submit.svg"
                    icon.height: 10
                    icon.width:  12
                    //% "Submit"
                    text:        qsTrId("dapps-store-submit")
                    onClicked: {
                        var publisherName = addPublisherByKey(publicKeyInput.text)
                        if (publisherName) {
                            // TODO: Do we need to create a specific notification popup?

                            //% "%1 added to the list of publishers that you follow"
                            main.showSimplePopup(qsTrId("dapps-store-add-publisher-notification").arg(publisherName))
                            addPublisherDialog.close()
                        }
                        
                        publicKeyError.visible = true
                    }
                }
            }
        }
    }
}
