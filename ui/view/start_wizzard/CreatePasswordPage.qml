import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

Rectangle {
    color: Style.background_main

    property Item defaultFocusItem: password
    property var onEnterPassword: function() {
        if(password.text.length == 0)
        {
            //% "Please, enter password"
            passwordError.text = qsTrId("general-pwd-empty-error");
        }
        else if(password.text != confirmPassword.text)
        {
            //% "Passwords do not match"
            passwordError.text = qsTrId("start-create-pwd-not-match-error");
            confirmPassword.hasError = true
        }
        else
        {
            viewModel.setPassword(password.text);
            if (viewModel.isRecoveryMode) {
                viewModel.setupLocalNode(parseInt(viewModel.defaultPortToListen()), viewModel.chooseRandomNode());
                createWallet();
            } else {
                startWizzardView.push(nodeSetupPage);
            }
        }
    }

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        anchors.topMargin: 50
        Column {
            spacing: 30
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredWidth: 730
            SFText {
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
                text: viewModel.isRecoveryMode
                    //% "Create new password"
                    ? qsTrId("start-recovery-title")
                    //% "Create password"
                    : qsTrId("start-create-password")
                color: Style.content_main
                font.pixelSize: 36
            }
            SFText {
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Qt.AlignHCenter
                text: viewModel.isRecoveryMode
                    //% "Create new password to access your wallet"
                    ? qsTrId("start-recovery-pwd-message")
                    //% "Create password to access your wallet"
                    : qsTrId("start-create-pwd-message")
                color: Style.content_main
                wrapMode: Text.WordWrap
                font.pixelSize: 14
            }
        }
                    
        Column {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 400
            Layout.topMargin: 50
            spacing: 30

            Column {
                width: parent.width
                spacing: 10

                SFText {
                    //% "Account password"
                    text: qsTrId("start-pwd-label")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.styleName: "Bold"; font.weight: Font.Bold
                }

                PasswordBoxInput {
                    id:password
                    width: parent.width
                    font.pixelSize: 16
                    showEye: true
                    onTextChanged: {
                        if (confirmPassword.text.length == password.text.length) {
                            passwordError.text = "";
                            confirmPassword.hasError = false;
                        }
                    }
                    onAccepted: {
                        confirmPassword.forceActiveFocus();
                    }
                }
            }

            Column {
                width: parent.width
                spacing: 10

                SFText {
                    //% "Password confirmation"
                    text: qsTrId("start-create-pwd-confirm-label")
                    color: Style.content_main
                    font.pixelSize: 14
                    font.styleName: "Bold"; font.weight: Font.Bold
                }

                PasswordBoxInput {
                    id: confirmPassword
                    width: parent.width
                    font.pixelSize: 16
                    showEye: true
                    onTextChanged: {
                        if (confirmPassword.text.length == password.text.length) {
                            passwordError.text = "";
                            this.hasError = false;
                        }
                    }
                    onAccepted: {
                        onEnterPassword();
                    } 
                }

                SFText {
                    id: passwordError
                    color: Style.validator_error
                    font.pixelSize: 14
                    font.italic: true
                    height: 16
                    width: parent.width
                }
            }

            Column {
                width: parent.width
                spacing: 10

                RowLayout{
                    id: strengthChecker
                    property var strengthTests: 
                    [
                        //: set passwort, difficulty message, very weak
                        //% "Very weak password"
                        {exp: new RegExp("(?=.{1,})")                                                               , color: Style.validator_error, msg: qsTrId("start-pwd-difficulty-very-weak")},
                        //: set passwort, difficulty message, weak
                        //% "Weak password"
                        {exp: new RegExp("((?=.{6,})(?=.*[0-9]))|((?=.{6,})(?=.*[A-Z]))|((?=.{6,})(?=.*[a-z]))")    , color: Style.validator_error, msg: qsTrId("start-pwd-difficulty-weak")},
                        //: set passwort, difficulty message, medium
                        //% "Medium strength password"
                        {exp: new RegExp("((?=.{6,})(?=.*[A-Z])(?=.*[a-z]))|((?=.{6,})(?=.*[0-9])(?=.*[a-z]))")     , color: Style.validator_warning, msg: qsTrId("start-pwd-difficulty-medium")},
                        //: set passwort, difficulty message, medium
                        //% "Medium strength password"
                        {exp: new RegExp("(?=.{8,})(?=.*[0-9])(?=.*[A-Z])(?=.*[a-z])")                              , color: Style.validator_warning, msg: qsTrId("start-pwd-difficulty-medium")},
                        //: set passwort, difficulty message, strong
                        //% "Strong password"
                        {exp: new RegExp("(?=.{10,})(?=.*[0-9])(?=.*[A-Z])(?=.*[a-z])")                             , color: Style.active, msg: qsTrId("start-pwd-difficulty-strong")},
                        //: set passwort, difficulty message, very strong
                        //% "Very strong password"
                        {exp: new RegExp("(?=.{10,})(?=.*[!@#\$%\^&\*])(?=.*[0-9])(?=.*[A-Z])(?=.*[a-z])")          , color: Style.active, msg: qsTrId("start-pwd-difficulty-very-strong")},
                    ]

                    function passwordStrength(pass)
                    {
                        for(var i = strengthTests.length - 1; i >= 0; i--)
                            if(strengthTests[i].exp.test(pass))
                                return i + 1;
                               
                        return 0;
                    }

                    property var strength: passwordStrength(password.text)
                    width: parent.width
                    spacing: 8

                    Repeater{
                        model: parent.strengthTests.length

                        Rectangle {
                            Layout.fillWidth: true
                            height: 4
                            border.width: index < parent.strength ? 0 : 1
                            border.color: Style.background_second
                            radius: 10
                            color: index < parent.strength ? parent.strengthTests[parent.strength-1].color : Style.background_main
                        }
                    }
                }

                SFText {
                    text: strengthChecker.strength > 0 ? strengthChecker.strengthTests[strengthChecker.strength-1].msg : ""
                    color: Style.content_main
                    font.pixelSize: 14
                    height: 16
                    width: parent.width
                }

                SFText {
/*% "Strong password needs to meet the following requirements:
•  the length must be at least 10 characters
•  must contain at least one lowercase letter
•  must contain at least one uppercase letter
•  must contain at least one number"
*/
                    text: qsTrId("start-create-pwd-strength-message")
                    color: Style.content_secondary
                    font.pixelSize: 14
                    height: 80
                    width: parent.width
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 50
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
                    
            spacing: 30

            CustomButton {
                //% "Back"
                text: qsTrId("general-back")
                icon.source: "qrc:/assets/icon-back.svg"
                onClicked: startWizzardView.pop();
            }
            PrimaryButton {
                            
                text: viewModel.isRecoveryMode
                    //% "Open my wallet"
                    ? qsTrId("general-open-wallet")
                    //% "Next"
                    : qsTrId("general-next")
                icon.source : viewModel.isRecoveryMode
                    ? "qrc:/assets/icon-wallet-small.svg"
                    : "qrc:/assets/icon-next-blue.svg"
                enabled: password.text.length > 0 && confirmPassword.text.length == password.text.length && passwordError.text == ""
                onClicked: {
                    onEnterPassword();
                }
            }
        }
                     
        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 67
            Layout.maximumHeight: 143
        }

        VersionFooter {}
    }
}