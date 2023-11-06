import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

StartLayout {
    id:    startLayout
    property Item defaultFocusItem: openPassword

    // default methods for open wallet, can be changed for unlock wallet
    property var openWallet: function (pass, callback) {
        viewModel.openWallet(pass, callback);
    }
    property var loadWallet: function () {
        startWizzardView.push("qrc:/loading.qml", {"isRecoveryMode" : false, "isCreating" : false, "cancelCallback": startWizzardView.pop});
    }
                
    property var checkCapsLockOnActivation: function () {
        viewModel.checkCapsLock();
        // OSX hack, to handle capslock shutdonw
        if (Qt.platform.os == "osx" && viewModel.isCapsLockOn) {
            var timer = Qt.createQmlObject('import QtQml 2.11; Timer {}', openWalletPage, "osxCapsTimer");
            timer.interval = 500;
            timer.repeat = true;
            timer.triggered.connect(viewModel.checkCapsLock);
            timer.start();
        }
    }
    Component.onCompleted: root.parent.activated.connect(checkCapsLockOnActivation)
    Component.onDestruction: root.parent.activated.disconnect(checkCapsLockOnActivation)

    function clearPassword() {
        openPassword.clear()
        openPasswordError.text = ""
    }

    Keys.onPressed: {
        // Linux hack, X11 return caps state with delay
        if (Qt.platform.os == "linux") {
            var timer = Qt.createQmlObject('import QtQml 2.11; Timer {}', openWalletPage, "linuxCapsTimer");
            timer.interval = 500;
            timer.repeat = false;
            timer.triggered.connect(viewModel.checkCapsLock);
            timer.start();
        } else {
            viewModel.checkCapsLock();
        }
    }
    Keys.onReleased: {
        // OSX hack, to handle capslock shutdonw
        if (Qt.platform.os == "osx") {
            viewModel.checkCapsLock();
        }
    }

    ColumnLayout {
        id: openColumn
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 0

        AccountSetup {
            Layout.alignment:       Qt.AlignHCenter
            Layout.minimumWidth:    400
            Layout.maximumWidth:    400
            Layout.bottomMargin:    startLayout.isSqueezedHeight  ? 10 : 20
        }

        Connections {
            target: viewModel
            function onCurrentAccountChanged() {
                clearPassword()
            }
            function onCurrentNetworkChanged() {
                clearPassword()
            }
        }

        ColumnLayout {
            Layout.maximumWidth: 400
            Layout.minimumWidth: 400
            Layout.preferredHeight: 79
            Layout.alignment: Qt.AlignHCenter

            SFText {
                //% "Account password"
                text: qsTrId("start-account-password-label")
                color: Style.content_main
                font.pixelSize: 14
                font.styleName: "Bold"; font.weight: Font.Bold
            }

            SFTextInput {
                id: openPassword
                Layout.fillWidth: true
                focus: true
                activeFocusOnTab: true
                font.pixelSize: 14
                color: openPasswordError.text.length ? Style.validator_error : Style.content_main
                echoMode: TextInput.Password
                onAccepted: btnCurrentWallet.clicked()
                onTextChanged: if (openPassword.text.length > 0) openPasswordError.text = ""
                enabled: viewModel.isOnlyOneInstanceStarted
                backgroundColor: openPasswordError.text.length ? Style.validator_error : Style.content_main
            }

            SFText {
                id: openPasswordError
                text: viewModel.isOnlyOneInstanceStarted
                    ? ""
                    //% "The wallet is already started. Close all running wallets and start again."
                    : qsTrId("start-second-copy-error")
                color: Style.validator_error
                font.pixelSize: 14
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: startLayout.isSqueezedHeight  ? 8 : 14
            Layout.preferredHeight: 38
            spacing:          15
                                
            function tryOpenWallet() {
                if(openPassword.text.length == 0)
                {
                    //% "Please, enter password"
                    openPasswordError.text = qsTrId("general-pwd-empty-error");
                    openPassword.focus = true;
                }
                else
                {
                    openWallet(openPassword.text, function (errmsg) {
                        if(errmsg.length)
                        {
                            openPasswordError.text = errmsg
                            openPassword.selectAll()
                            openPassword.focus = true
                        }
                        else
                        {
                            loadWallet();
                        }
                    })
                }
            }

            PrimaryButton {
                anchors.verticalCenter: parent.verticalCenter
                id: btnCurrentWallet
                enabled: viewModel.isOnlyOneInstanceStarted && (!viewModel.useHWWallet || viewModel.isTrezorConnected)
                text: (viewModel.useHWWallet == false)
                    ?
                    //% "Show my wallet"
                    qsTrId("open-show-wallet-button")
                    :
                    //% "Show my wallet with Trezor"
                    qsTrId("open-show-wallet-button-hw")
                icon.source: "qrc:/assets/icon-wallet-small.svg"
                onClicked: {
                    parent.tryOpenWallet();
                }
            }
        }

        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 36
            Layout.topMargin: startLayout.isSqueezedHeight  ? 9 : 15
            Layout.bottomMargin: 9
            Rectangle {
                id: capsWarning
                anchors.centerIn: parent
                color: Style.caps_warning
                width: 152
                height: 36
                radius: 6
                opacity: 0.2
                visible: viewModel.isCapsLockOn    
            }
            SFText {
                anchors.centerIn: capsWarning
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                //% "Caps lock is on!"
                text: qsTrId("start-open-caps-warning")
                color: Style.content_main
                font.pixelSize: 14
                visible: viewModel.isCapsLockOn 
            }
        }

        LinkButton {
            Layout.alignment: Qt.AlignHCenter
            //% "Restore wallet or create a new one"
            text: qsTrId("general-restore-or-create-wallet")
            fontSize: 14
            visible: viewModel.isOnlyOneInstanceStarted
            onClicked: {
                confirmChangeWalletDialog.open();
            }
        }

        Item {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
        }

        ConfirmationDialog {
            id: confirmChangeWalletDialog
            //% "Proceed"
            okButtonText: qsTrId("general-proceed")
            okButtonIconSource: "qrc:/assets/icon-done.svg"
            cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
            cancelButtonVisible: true
            width: 460
            height: 195
            contentItem: Column {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 20

                SFText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Qt.AlignHCenter
                    //% "Restore wallet or create a new one"
                    text: qsTrId("general-restore-or-create-wallet")
                    color: Style.content_main
                    font.pixelSize: 18
                    font.styleName: "Bold"
                    font.weight: Font.Bold
                }

                SFText {
                    horizontalAlignment : Text.AlignHCenter
                    width: parent.width
                    //% "If you'll restore a wallet all transaction history and addresses will be lost."
                    text: qsTrId("start-open-change-wallet-message")
                    color: Style.content_main
                    font.pixelSize: 14
                    wrapMode: Text.Wrap
                }
            }
            onAccepted: {
                viewModel.isRecoveryMode = false;
                startWizzardView.push(walletStartPage);
            }
        }
    }
}