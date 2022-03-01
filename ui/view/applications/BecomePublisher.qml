import QtQuick          2.15
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import Beam.Wallet      1.0
import "../controls"

CustomDialog {
    id:      control
    width:   761
    height:  769
    x:       Math.round((parent.width - width) / 2)
    y:       Math.round((parent.height - height) / 2)
    parent:  Overlay.overlay
    modal:   true

    property bool newPublisher: true

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
            text: control.newPublisher ?
                //% "Become a publisher"
                qsTrId("dapps-store-become-publisher")
                :
                //% "Edit publisher info"
                qsTrId("dapps-store-edit-publisher-info")
        }

        SFText {
            Layout.fillWidth:     true
            color:                Style.white
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            font.weight:          Font.Normal
            //% "To become a publisher you need to set up a username. ID number and personal Publisher Key will\n be given to you automatically. Registration will allow you to publish, update and delete DApps."
            text: qsTrId("dapps-store-become-publisher-text")
        }

        GridLayout {
            width: parent.width
            columns: 2
            columnSpacing: 31
            rowSpacing: 20

            Column {
                spacing: 10
                Layout.alignment: Qt.AlignTop

                Row {
                    SFText {
                        //% "Nickname"
                        text: qsTrId("dapps-store-nickname")
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
                    id: nameInput
                    width: 335
                    height: 45
                    color: Style.content_main
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Short title"
                    text: qsTrId("dapps-store-short-title")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: shortTitleInput
                    width: 335
                    height: 45
                    color: Style.content_main
                }

                SFText {
                    //% "50 characters max"
                    text: qsTrId("dapps-store-50-character-max")
                    color: Style.content_main
                    font.pixelSize: 12
                    font.weight: Font.Normal
                }
            }

            Column {
                spacing: 10
                Layout.columnSpan: 2

                SFText {
                    //% "About me"
                    text: qsTrId("dapps-store-about-me")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: aboutMeInput
                    width: 701
                    height: 45
                    color: Style.content_main
                }

                SFText {
                    //% "150 characters max"
                    text: qsTrId("dapps-store-150-character-max")
                    color: Style.content_main
                    font.pixelSize: 12
                    font.weight: Font.Normal
                }
            }

            SFText {
                //% "Social networks"
                text: qsTrId("dapps-store-social-networks")
                color: Style.content_main
                font.pixelSize: 14
                font.weight: Font.Bold
                Layout.columnSpan: 2
            }

            Column {
                spacing: 10

                SFText {
                    //% "Website"
                    text: qsTrId("dapps-store-website")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: websiteInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "https://website.name/"
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Twitter"
                    text: qsTrId("dapps-store-twitter")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: twitterInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "@nickname"
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "LinkedIn"
                    text: qsTrId("dapps-store-linkedin")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: linkedinInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "@nickname"
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Instagramm"
                    text: qsTrId("dapps-store-instagram")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: instagrammInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "@nickname"
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Telegram"
                    text: qsTrId("dapps-store-telegram")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: telegramInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "@nickname"
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Discord"
                    text: qsTrId("dapps-store-discord")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInput {
                    id: discordInput
                    width: 335
                    height: 45
                    color: Style.content_main
                    placeholderText: "login#0000"
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            Item {
                Layout.fillWidth: true
            }

            CustomButton {
                icon.source: "qrc:/assets/icon-cancel-white.svg"
                text: qsTrId("general-cancel")
                onClicked: {
                    control.close();
                }
            }

            CustomButton {
                palette.button: Style.active
                icon.source: "qrc:/assets/icon-dapps-store-create-account.svg"

                
                text: control.newPublisher ?
                    //% "create account"
                    qsTrId("dapps-store-create-account")
                    :
                    //% "save changes"
                    qsTrId("dapps-store-save-changes")
                palette.buttonText: Style.content_opposite
                enabled: control.newPublisher ? nameInput.text : false
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}