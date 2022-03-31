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
    property var publisherInfo

    signal createPublisher(var info)
    signal changePublisherInfo(var info)

    function isChanged() {
        return publisherInfo.nickname !== nameInput.text ||
            publisherInfo.shortTitle !== shortTitleInput.text ||
            publisherInfo.aboutMe !== aboutMeInput.text ||
            publisherInfo.website !== websiteInput.text ||
            publisherInfo.twitter !== twitterInput.text ||
            publisherInfo.linkedin !== linkedinInput.text ||
            publisherInfo.instagram !== instagramInput.text ||
            publisherInfo.telegram !== telegramInput.text ||
            publisherInfo.discord !== discordInput.text;
    }

    function allFieldsIsValid() {
        return websiteInput.isValid &&
               twitterInput.isValid &&
               linkedinInput.isValid &&
               instagramInput.isValid &&
               telegramInput.isValid &&
               discordInput.isValid;
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
            visible: control.newPublisher
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
                    text: !!control.publisherInfo.nickname ? control.publisherInfo.nickname : ""
                    maximumLength: 30
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
                    text: !!control.publisherInfo.shortTitle ? control.publisherInfo.shortTitle : ""
                    maximumLength: 50
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
                    text: !!control.publisherInfo.aboutMe ? control.publisherInfo.aboutMe : ""
                    maximumLength: 150
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

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: websiteInput
                    width: 335
                    height: 45
                    placeholderText: "https://website.name/"
                    text: !!control.publisherInfo.website ? control.publisherInfo.website : ""
                    icon: "qrc:/assets/icon-dapps-store-website.svg"
                    maximumLength: 100
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^(?:http(s)?:\/\/)?[\w.-]+(?:\.[\w\.-]+)+[\w\-\._~:/?#[\]@!\$&'\(\)\*\+,;=.]+$/ }
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

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: twitterInput
                    width: 335
                    height: 45
                    placeholderText: "nickname"
                    text: !!control.publisherInfo.twitter ? control.publisherInfo.twitter : ""
                    icon: "qrc:/assets/icon-dapps-store-twitter.svg"
                    maximumLength: 50
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^[A-Za-z0-9_]+$/ }
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

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: linkedinInput
                    width: 335
                    height: 45
                    placeholderText: "in/nickname or company/name"
                    text: !!control.publisherInfo.linkedin ? control.publisherInfo.linkedin : ""
                    icon: "qrc:/assets/icon-dapps-store-linkedin.svg"
                    maximumLength: 50
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^in\/[A-Za-z0-9_]+|company\/[A-Za-z0-9_]+$/ }
                }
            }

            Column {
                spacing: 10

                SFText {
                    //% "Instagram"
                    text: qsTrId("dapps-store-instagram")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.weight: Font.Normal
                }

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: instagramInput
                    width: 335
                    height: 45
                    placeholderText: "nickname"
                    text: !!control.publisherInfo.instagram ? control.publisherInfo.instagram : ""
                    icon: "qrc:/assets/icon-dapps-store-instagram.svg"
                    maximumLength: 50
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^[A-Za-z0-9_]+$/ }
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

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: telegramInput
                    width: 335
                    height: 45
                    placeholderText: "nickname"
                    text: !!control.publisherInfo.telegram ? control.publisherInfo.telegram : ""
                    icon: "qrc:/assets/icon-dapps-store-telegram.svg"
                    maximumLength: 50
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^[A-Za-z0-9_]+$/ }
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

                SFTextInputEx {
                    property bool isValid: acceptableInput || text.length == 0
                    id: discordInput
                    width: 335
                    height: 45
                    placeholderText: "login"
                    text: !!control.publisherInfo.discord ? control.publisherInfo.discord : ""
                    icon: "qrc:/assets/icon-dapps-store-discord.svg"
                    maximumLength: 50
                    color: isValid ? Style.content_main : Style.validator_error
                    backgroundColor: isValid ? Style.content_main : Style.validator_error
                    validator: RegExpValidator { regExp: /^[A-Za-z0-9_]+$/ }
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
                    nameInput.text = !!control.publisherInfo.nickname ? control.publisherInfo.nickname : "";
                    shortTitleInput.text = !!control.publisherInfo.shortTitle ? control.publisherInfo.shortTitle : "";
                    aboutMeInput.text = !!control.publisherInfo.aboutMe ? control.publisherInfo.aboutMe : "";
                    websiteInput.text = !!control.publisherInfo.website ? control.publisherInfo.website : "";
                    twitterInput.text = !!control.publisherInfo.twitter ? control.publisherInfo.twitter : "";
                    linkedinInput.text = control.publisherInfo.linkedin ? control.publisherInfo.linkedin : "";
                    instagramInput.text = !!control.publisherInfo.instagram ? control.publisherInfo.instagram : "";
                    telegramInput.text = !!control.publisherInfo.telegram ? control.publisherInfo.telegram : "";
                    discordInput.text = !!control.publisherInfo.discord ? control.publisherInfo.discord : "";

                    control.close();
                }
            }

            CustomButton {
                palette.button: Style.active
                icon.source: "qrc:/assets/icon-next-blue.svg"

                
                text: control.newPublisher ?
                    //% "create account"
                    qsTrId("dapps-store-create-account")
                    :
                    //% "save changes"
                    qsTrId("dapps-store-save-changes")
                palette.buttonText: Style.content_opposite
                enabled: nameInput.text && isChanged() && allFieldsIsValid()
                onClicked: {
                    var info = {
                        nickname: nameInput.text,
                        shortTitle: shortTitleInput.text,
                        aboutMe: aboutMeInput.text,
                        website: websiteInput.text,
                        twitter: twitterInput.text,
                        linkedin: linkedinInput.text,
                        instagram: instagramInput.text,
                        telegram: telegramInput.text,
                        discord: discordInput.text
                    };
                    if (control.newPublisher) {
                        control.createPublisher(info);
                    } else {
                        control.changePublisherInfo(info);
                    }
                    control.close();
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}