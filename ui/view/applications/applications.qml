import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.15
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"
import "../wallet"
import "../utils.js" as Utils
import "."

ColumnLayout {
    id: control
    spacing:          0

    property var      stackView:      StackView.view
    property string   errorMessage:   ""
    property var      appsList:       undefined
    property var      unsupportedCnt: 0
    property var      appToOpen:      undefined
    property string   openedTxID:     ""
    property bool     showBack:       true
    readonly property bool hasApps:   !!appsList && appsList.length > 0

    function openAppTx (txid) {
        openedTxID = txid
        showTxDetails(txid)
        openedTxID = "";
    }

    function uninstallApp (app) {
        if (viewModel.uninstallLocalApp(app.appid)) {
            //% "Everything worked out.\n'%1' DApp is uninstalled."
            dappStoreOk.text = qsTrId("app-uninstall-success").arg(app.name)
            dappStoreOk.open()
        } else {
            //% "Something went wrong.\nUninstall failed. Please try again later."
            dappStoreFail.text = qsTrId("app-uninstall-fail")
            dappStoreFail.open()
        }
    }

    function navigatePublishersList() {
        var params = {
            "appsViewModel":     viewModel,
            "onBack":            stackView.pop,
        }
        stackView.push(Qt.createComponent("PublishersList.qml"), params)
    }

    function navigatePublisherDetails() {
        if (viewModel.isPublisher) {
            var params = {
                "viewModel": viewModel,
                "onBack":    stackView.pop,
                "uninstall": uninstallApp,
            }
            stackView.push(Qt.createComponent("PublisherDetails.qml"), params)
        }
        else {
            const dialog = Qt.createComponent("qrc:/applications/BecomePublisher.qml")
            const instance = dialog.createObject(control,
                {
                    newPublisher:  !viewModel.isPublisher,
                    publisherInfo: viewModel.publisherInfo
                });

            instance.onCreatePublisher.connect(function(info) {
                viewModel.changePublisherInfo(info, true);
            });

            instance.onChangePublisherInfo.connect(function(info) {
                viewModel.changePublisherInfo(info, false);
            });

            instance.open();
        }
    }

    function installFromFile (fname) {
        if (!fname) {
            //% "Select application to install"
            fname = viewModel.chooseFile(qsTrId("applications-install-title"))
            if (!fname) return
        }

        var appName = viewModel.installFromFile(fname)
        if (appName.length) {
            dndDialog.isOk = true;
            dndDialog.appName = appName;
        } else {
            dndDialog.isFail = true;
        }
    }

    signal showTxDetails(string txid)

    ApplicationsViewModel {
        id: viewModel

        onShaderTxData: function (action, data, txComment, fee, feeRate, rateUnit, isEnough) {
            const dialog = Qt.createComponent("qrc:/send_confirm.qml")
            const instance = dialog.createObject(control,
                {
                    rateUnit:       rateUnit,
                    fee:            fee,
                    feeRate:        feeRate,
                    comment:        txComment,
                    appMode:        true,
                    isOnline:       false,
                    showPrefix:     true,
                    hasAmounts:     false,
                    isEnough:       isEnough
                })

            instance.Component.onDestruction.connect(function () {
                if (instance.result == Dialog.Accepted) {
                    viewModel.contractInfoApproved(action, data)
                    return
                }
                viewModel.contractInfoRejected()
                return
            })

            instance.open()
        }

        onAppInstallOK: function (appName) {
            //% "Congratulations!\n'%1' DApp is successfully installed."
            dappStoreOk.text = qsTrId("app-install-success").arg(appName)
            dappStoreOk.open()
        }

        onAppInstallFail: function (appName) {
            //% "Sorry, the installation failed.\nPlease, check the file and try again."
            dappStoreFail.text = qsTrId("app-install-fail")
            dappStoreFail.open()
        }

        onAppInstallTimeoutFail: function (appName) {
            //% "Sorry, the installation failed.\nPlease, restart the wallet and try again."
            dappStoreFail.text = qsTrId("app-install-timeout-fail")
            dappStoreFail.open()
        }

        onAppUpdateFail: function (appName) {
            //% "Sorry, the update failed.\nPlease, check the file and try again."
            dappStoreFail.text = qsTrId("app-update-fail")
            dappStoreFail.open()
        }

        onAppUpdateTimeoutFail: function (appName) {
            //% "Sorry, the update failed.\nPlease, restart the wallet and try again."
            dappStoreFail.text = qsTrId("app-update-timeout-fail")
            dappStoreFail.open()
        }

        onShowTxIsSent: function() {
            transactionIsSent.open();
        }

        onHideTxIsSent: function() {
            transactionIsSent.close();
        }

        onShowYouArePublisher: function() {
            youArePublisher.open();
        }

        onPublisherCreateFail: function() {
            //% "Oops... Failed to create Publisher.\nPlease check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-create-publisher");
            dappStoreFail.open();
        }

        onPublisherEditFail: function() {
            //% "Oops... Failed to edit Publisher.\nPlease check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-edit-publisher");
            dappStoreFail.open();
        }

        onAppPublishFail: function() {
            //% "Oops... Failed to publish DApp.\nPlease check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-publish-dapp");
            dappStoreFail.open();
        }

        onAppRemoveFail: function() {
            //% "Oops... Failed to remove DApp.\nPlease check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-remove-dapp");
            dappStoreFail.open();
        }
    }

    //
    // Page Header (Title + Status Bar)
    //
                                //% "Wallet"
    property string title:      qsTrId("wallet-title")
    property var titleContent:  RowLayout {
        spacing: 20
        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }

        CustomButton {
            id: sendButton
            Layout.preferredHeight: 32
            palette.button: Style.accent_outgoing
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-send-blue.svg"
            //% "Send"
            text: qsTrId("general-send")
            font.pixelSize: 12
            onClicked: {
                main.navigateSend(assetsList.selectedId);
            }
        }

        CustomButton {
            Layout.preferredHeight: 32
            palette.button: Style.accent_incoming
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-receive-blue.svg"
            //% "Receive"
            text: qsTrId("wallet-receive-button")
            font.pixelSize: 12
            onClicked: {
                main.navigateReceive(assetsList.selectedId);
            }
        }

        ContextMenu {
            id: swapContextMenu
            Action {
                text:           qsTrId("atomic-swap-title")
                icon.source:    "qrc:/assets/icon-atomic_swap.svg"
                onTriggered: {
                    main.openAtomicSwaps();
                }
            }
            Action {
                text:           qsTrId("assets-swap-title")
                icon.source:    "qrc:/assets/icon-assets_swap.svg"
                onTriggered: {
                    main.openAssetSwaps();
                }
            }
        }

        CustomButton {
            id:                     swapButton
            Layout.preferredHeight: 32
            palette.button:         Style.active
            palette.buttonText:     Style.content_opposite
            icon.source:            "qrc:/assets/icon-swap-blue.svg"
            //% "Swap"
            text: qsTrId("wallet-swap-button")
            font.pixelSize:         12
            onClicked: {
                swapContextMenu.popup(swapButton, Qt.point(0, swapButton.height + 6))
            }
        }
    }

    SettingsViewModel {
        id: settings
    }

    ColumnLayout {
        id:                appsRoot
        Layout.fillWidth:  true
        Layout.fillHeight: true
        spacing:           0

        state: "applications"
        states: [
            State {
                name: "applications"
                PropertyChanges { target: appsTab; state: "active" }
                PropertyChanges { target: dappsLayout; visible: true }
            },
            State {
                name: "transactions"
                PropertyChanges { target: txTab; state: "active" }
                PropertyChanges { target: txTable; visible: true }
            }
        ]

        DnDdappInstallDialog {
            id: dndDialog
            onGetFileName: function(fname) {
                installFromFile(fname);
            }
        }

        AssetsPanel {
            id:                 assetsList
            Layout.fillWidth:   true
        }

        RowLayout {
            Layout.topMargin:   40
            spacing:            25

            TabButton {
                id: appsTab
                //% "Applications"
                label:              qsTrId("apps-title")
                Layout.alignment:   Qt.AlignVCenter
                onClicked:          appsRoot.state = "applications"
            }

            TabButton {
                id: txTab
                label:              qsTrId("wallet-transactions-title")
                Layout.alignment:   Qt.AlignVCenter
                onClicked:          appsRoot.state = "transactions"
            }
        }

        TxTable {
            id:    txTable
            owner: control

            Layout.topMargin:  12
            Layout.fillWidth:  true
            Layout.fillHeight: true
            visible:           false
            Component.onCompleted: {
                control.showTxDetails.connect(txTable.showTxDetails)
            }

            Component.onDestruction: {
                control.showTxDetails.disconnect(txTable.showTxDetails)
            }
        }

        ColumnLayout {
            id:                dappsLayout
            Layout.topMargin:  12
            Layout.fillWidth:  true
            Layout.fillHeight: true
            spacing:           0
            visible:           false
            Item {
                visible:          appsListView.visible
                Layout.fillWidth: true
                Layout.preferredHeight: 32

                RowLayout {
                    spacing:           20
                    anchors.right:     parent.right

                    ContextMenu {
                        id: appActionsMenu

                        Action {
                            //% "Install DApp"
                            text:           qsTrId("dnd-app-install-title")
                            icon.source:    "qrc:/assets/icon-add.svg"
                            onTriggered:    dndDialog.open()
                        }
                        Action {
                            //% "Publishers"
                            text:           qsTrId("dapps-store-publishers-page-main-title")
                            icon.source:    "qrc:/assets/icon-dapps_store-publishers.svg"
                            onTriggered:    navigatePublishersList()
                        }
                        Action {
                            //% "become a publisher"
                            text:           viewModel.isPublisher ? viewModel.publisherInfo.name : qsTrId("apps-become-a-publisher")
                            icon.source:    "qrc:/assets/icon-dapps_store-become-a-publisher.svg"
                            onTriggered:    navigatePublisherDetails()
                        }
                    }

                    CustomToolButton {
                        id:                       appActionButton
                        padding:                  0
                        icon.color:               Style.content_main
                        icon.source:              "qrc:/assets/icon-settings.svg"
                        onClicked:                appActionsMenu.open()
                    }
                }
            }

            WebAPICreator {
                id: webapiCreator
            }
            function appSupported(app) {
                return webapiCreator.apiSupported(app.api_version || "current") ||
                       webapiCreator.apiSupported(app.min_api_version || "")
            }

            function launchApp(app) {
                stackView.push(Qt.createComponent("AppView.qml"), {"appToOpen": {"name":app.name, "appid":app.appid}});
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth:  true
                visible:           !appsListView.visible

                ColumnLayout {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y:                        parent.height / 2 - this.height / 2 - 40
                    spacing:                  40

                    SFText {
                        Layout.alignment: Qt.AlignHCenter
                        color:            control.errorMessage.length ? Style.validator_error : Style.content_main
                        opacity:          0.5

                        font {
                            italic:    true
                            pixelSize: 16
                        }

                        text: {
                            if (control.errorMessage.length) {
                                return control.errorMessage
                            }

                            //if (control.activeApp || control.appToOpen) {
                            //    //% "Please wait, %1 is loading"
                            //    return qsTrId("apps-loading-app").arg(
                            //        (control.activeApp || control.appToOpen).name
                            //    )
                            //}

                            if (!control.appsList) {
                                //% "Loading..."
                                return qsTrId("apps-loading")
                            }

                            //% "There are no applications at the moment"
                            return qsTrId("apps-nothing")
                        }
                    }

                    SvgImage {
                        Layout.alignment: Qt.AlignHCenter
                        source:           "qrc:/assets/dapp-loading.svg"
                        sourceSize:       Qt.size(245, 140)

                        visible: {
                            if (control.errorMessage.length) {
                                return false
                            }

                            //if (control.activeApp || control.appToOpen) {
                            //    return true
                            //}

                            return false
                        }
                    }
                }
            }

            //SFText {
            //    id:                  errCntMessage
            //    Layout.alignment:    Qt.AlignRight
            //    Layout.topMargin:    5
            //    Layout.bottomMargin: 10
            //    color:               Style.validator_error
            //    visible:             control.hasApps && unsupportedCnt > 0
            //    font.italic:         true
            //                         //% "%n DApp(s) is not available"
            //    text:                qsTrId("apps-err-cnt", unsupportedCnt)
            //}

            AppsList {
                id:                  appsListView
                //Layout.topMargin:    unsupportedCnt ? 0 : 20
                Layout.fillHeight:   true
                Layout.fillWidth:    true
                visible:             control.hasApps
                appsList:            control.appsList
                isIPFSAvailable:     viewModel.isIPFSAvailable

                onLaunch: function (app) {
                    dappsLayout.launchApp(app)
                }

                onInstall: function (app, launchOnSuccess) {
                    if (launchOnSuccess) {
                        control.appToOpen = app
                    }
                    viewModel.installApp(app.guid)
                }

                onUpdate: function (app) {
                    viewModel.updateDApp(app.guid)
                }

                onUninstall: function (app) {
                    control.uninstallApp(app)
                }
            }

            function loadAppsList () {
                control.appsList = checkSupport(viewModel.apps)
                
                if (control.appToOpen) {
                    for (let app of control.appsList) {
                        if (app.guid == appToOpen.guid) {
                            if (dappsLayout.appSupported(app)) {
                                dappsLayout.launchApp(app)
                            } else {
                                //% "Update Wallet to launch %1 application"
                                BeamGlobals.showMessage(qsTrId("apps-update-message").arg(app.name))
                            }
                        }
                    }
                    control.appToOpen = undefined
                }
            }

            function checkSupport (apps) {
                unsupportedCnt = 0
                for (var app of apps) {
                    app.supported = dappsLayout.appSupported(app)
                    if (!app.supported) ++unsupportedCnt
                }
                return apps
            }

            Component.onCompleted: {
                viewModel.appsChanged.connect(loadAppsList)
                viewModel.stopProgress.connect(appsListView.stopProgress);

                viewModel.init(!!appToOpen);
            }

            Component.onDestruction: {
                viewModel.appsChanged.disconnect(loadAppsList)
                viewModel.stopProgress.disconnect(appsListView.stopProgress);
            }
        }
    }

    TransactionIsSent {
        id:           transactionIsSent
        newPublisher: !viewModel.isPublisher
    }

    YouArePublisher {
        id:           youArePublisher
        nickname:     !!viewModel.publisherInfo ? viewModel.publisherInfo.name : ""
        publisherKey: !!viewModel.publisherInfo ? viewModel.publisherInfo.pubkey : ""

        onGoToMyAccount: {
            youArePublisher.close()
            control.navigatePublisherDetails()
        }
    }

    ConfirmationDialog {
        id:                  dappStoreOk
        title:               qsTrId("dapp-store-dialog-title")
                             //% "Ok"
        okButtonText:        qsTrId("general-ok")
        cancelButtonVisible: false
    }

    ConfirmationDialog {
        id:                      dappStoreFail
                                 //% "Dapp Store"
        title:                   qsTrId("dapp-store-dialog-title")
                                 //% "Ok"
        okButtonText:            qsTrId("general-ok")
        okButton.palette.button: Style.accent_fail
        cancelButtonVisible:     false
    }
}
