import QtQuick 2.11
import QtQuick.Controls 2.3
import "."
import Beam.Wallet 1.0

CustomDialog {
    id: control

    property var settingsViewModel: function() {
        var checkWalletPassword = function() {
            console.log("settingsViewModel::checkWalletPassword undefined");
        }
        var changeWalletPassword = function() {
            console.log("settingsViewModel::changeWalletPassword undefined");
        }
    }

    modal: true

    width: 520
    height: 420
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    contentItem: Column {
        anchors.fill: parent
        anchors.margins: 30

        spacing: 30

        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            //% "Change wallet password"
            text: qsTrId("general-change-pwd")
            color: Style.content_main
            font.pixelSize: 24
            font.styleName: "Bold"; font.weight: Font.Bold
        }

        Column
        {
            width: parent.width

            SFText {
                //% "Enter old password"
                text: qsTrId("change-pwd-old-pwd-label")
                color: Style.content_main
                font.pixelSize: 12
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id: oldPass

                width: parent.width

                font.pixelSize: 12
                color: Style.content_main
                echoMode: TextInput.Password
            }    		
        }

        Column
        {
            width: parent.width

            SFText {
                //% "Enter new password"
                text: qsTrId("change-pwd-new-pwd-label")
                color: Style.content_main
                font.pixelSize: 12
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id: newPass

                width: parent.width

                font.pixelSize: 12
                color: Style.content_main
                echoMode: TextInput.Password
            }    		
        }

        Column
        {
            width: parent.width

            SFText {
                //% "Confirm new password"
                text: qsTrId("change-pwd-confirm-pwd-label")
                color: Style.content_main
                font.pixelSize: 12
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id: confirmPass

                width: parent.width

                font.pixelSize: 12
                color: Style.content_main
                echoMode: TextInput.Password
            }

        }

        Column  {
            width: parent.width
            height: error.height

            SFText {
                id: error
                color: Style.validator_error
                font.pixelSize: 12
            }			
        }    	

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 30

            CustomButton {
                //% "Cancel"
                text: qsTrId("general-cancel")
                onClicked: control.close()
                icon.source: "qrc:/assets/icon-cancel-white.svg"
            }

            PrimaryButton {
                //% "Change password"
                text: qsTrId("change-pwd-ok")
                icon.source: "qrc:/assets/icon-done.svg"
                onClicked: {
                    if(oldPass.text.length == 0)
                    {
                        //% "Please, enter old password"
                        error.text = qsTrId("change-pwd-old-empty");
                    }
                    else if(newPass.text.length == 0)
                    {
                        //% "Please, enter new password"
                        error.text = qsTrId("change-pwd-new-empty");
                    }
                    else if(confirmPass.text.length == 0)
                    {
                        //% "Please, confirm new password"
                        error.text = qsTrId("change-pwd-confirm-empty");
                    }
                    else if(!settingsViewModel.checkWalletPassword(oldPass.text))
                    {
                        //% "The old password you have entered is incorrect"
                        error.text = qsTrId("change-pwd-old-fail");
                    }
                    else if(newPass.text == oldPass.text)
                    {
                        //% "New password cannot be the same as old"
                        error.text = qsTrId("change-pwd-new-same-as-old");
                    }
                    else if(newPass.text != confirmPass.text)
                    {
                        //% "New password doesn't match the confirm password"
                        error.text = qsTrId("change-pwd-confirm-fail");
                    }
                    else
                    {
                        settingsViewModel.changeWalletPassword(newPass.text)
                        control.close()
                    }
                }
            }
        }
    }

    onOpened: {
        oldPass.forceActiveFocus(Qt.TabFocusReason);
        oldPass.text = newPass.text = confirmPass.text = error.text = ""
    }		
}