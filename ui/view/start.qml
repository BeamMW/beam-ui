import QtQuick 2
import QtQuick.Controls 1
import QtQuick.Controls 2
import QtQuick.Controls.Styles 1
import QtGraphicalEffects 1
import "controls"
import "start_wizzard"
import "utils.js" as Utils
import Beam.Wallet 1
import QtQuick.Layouts 1

Item
{
    id: root

    anchors.fill: parent
    property bool isLockedMode: false
    property bool isBadPortMode: false
    property bool isLogoutMode: false
    property int currentAccountIndex: 0

    StartViewModel { 
        id: viewModel 
        currentAccountIndex: root.currentAccountIndex
    }
    SeedValidationHelper { id: seedValidationHelper }

    function migrateWalletDB(path) {
        // copy wallet.db
        viewModel.migrateWalletDB(path);
        viewModel.isRecoveryMode = false;
        startWizzardView.push(openWalletPage, {
            "firstButtonVisible": true,
            //% "Back"
            "firstButtonText": qsTrId("general-back"), 
            "firstButtonIcon": "qrc:/assets/icon-back.svg",
            "firstButtonAction": function() {
                // remove wallet.db file
                viewModel.deleteCurrentWalletDB();
                startWizzardView.pop();
            }
        });
    }

    ConfirmationDialog {
        id: errorDlg

        //% "Ok"
        okButtonText:        qsTrId("general-ok")
        okButtonIconSource:  "qrc:/assets/icon-done.svg"
        cancelButtonVisible: false
        width: 460
        height: contentItem.implicitHeight + footer.implicitHeight + 60
        padding: 0

        property alias text: messageText.text


        contentItem: Column {
            width: parent.width
            height: messageText.implicitHeight + messageText.implicitHeight

            Item {
                height: 30
                width: parent.width
            }

            SFText {
                id: messageText
                padding: 30
                bottomPadding: 0
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment : Text.AlignHCenter
                width: parent.width
                height: 32
                text: ""
                color: Style.content_main
                font.pixelSize: 14
                wrapMode: Text.Wrap
            }
        }
    }

    function createWallet()
    {
        viewModel.createWallet(function (errMsg)
        {
            if (errMsg == "") { 
                startWizzardView.push("qrc:/loading.qml", {"isRecoveryMode" : true, "isCreating" : true, "cancelCallback": startWizzardView.pop});
            } else {
                errorDlg.text = errMsg;
                errorDlg.open();
            }
        });
    }

    StackView {
        id: startWizzardView
        anchors.fill: parent
        focus: true
        onCurrentItemChanged: {
            if (currentItem && currentItem.defaultFocusItem) {
                startWizzardView.currentItem.defaultFocusItem.forceActiveFocus();
            }
        }

        Component {
            id: walletStartPage
            WalletStartPage{}
        }

        Component {
            id: migrateWalletPage
            MigrateWalletPage{}
        }

        Component {
            id: selectWalletDBPage
            SelectWalletDBPage{}
        }

        Component {
            id: createNewWalletPage
            CreateNewWalletPage{}
        }

        Component {
            id: createTrezorWalletPage
            CreateTrezorWalletPage{}
        }

        Component {
            id: generateRecoveryPhrasePage 
            GenerateRecoveryPhrase {}
        }

        Component {
            id: checkRecoveryPhrasePage
            CheckRecoveryPhrase {}
        }

        Component {
            id: importTrezorOwnerKeyPage
            ImportTrezorOwnerKeyPage{}
        }

        Component {
            id: accountLabelPage
            AccountLabelPage{}
        }

        Component {
            id: restoreWalletPage
            RestoreWalletPage{}
        }

        Component {
            id: createPasswordPage
            CreatePasswordPage{}
        }

        Component {
            id: nodeSetupPage
            NodeSetupPage{}
        }

        Component {
            id: openWalletPage
            OpenWalletPage{}
        }

        function restoreProcessBadPortMode(isRecoveryMode) {
            startWizzardView.pop();
            startWizzardView.push(nodeSetupPage);
            root.isBadPortMode = true;
            viewModel.isRecoveryMode = isRecoveryMode;
        }

        Component.onCompleted: {
            if (seedValidationHelper.isSeedValidatiomMode) {
                startWizzardView.push(generateRecoveryPhrasePage);
            } else if (isBadPortMode) {
                startWizzardView.push(nodeSetupPage)
            }
            else if (isLockedMode) {
                startWizzardView.push(openWalletPage,
                    { 
                        "openWallet": function (pass, callback) {
                        if (viewModel.checkWalletPassword(pass)) {
                            callback("")
                        } else {
                            //% "Invalid password provided"
                            callback(qsTrId("general-pwd-invalid"))
                        }
                      },
                      "loadWallet": function () {
                        root.parent.setSource("qrc:/main.qml");
                      }
                    })
            } else if (viewModel.walletExists) {
                if (isLogoutMode) {
                    BeamGlobals.resetModel()
                }
                startWizzardView.push(openWalletPage);
            }
            else if (viewModel.isFoundExistingWalletDB()) {
                startWizzardView.push(migrateWalletPage);
            } else {
                startWizzardView.push(walletStartPage);
            }
        }
    }
}

