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
    property var      activeApp:      undefined
    property var      appToOpen:      undefined
    property string   openedTxID:     ""

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

    signal reloadWebEngineView()
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

    property string title:      control.appToOpen.name

    SettingsViewModel {
        id: settings
    }

    ColumnLayout {
        id:                dappsLayout
        Layout.fillWidth:  true
        Layout.fillHeight: true
        spacing:           0

        //
        // This object is visible to web. We create such proxy
        // to ensure that nothing (methods, events, props &c)
        // is leaked to the web from real API
        //
        QtObject {
            id:            webapiBEAM
            WebChannel.id: "BEAM"

            property var style: Style
            property var api: QtObject
            {
                function callWalletApi (request)
                {
                    callWalletApiCall(request)
                }

                signal callWalletApiResult (string result)
                signal callWalletApiCall   (string request)
            }
        }

        WebChannel {
            id:                apiChannel
            registeredObjects: [webapiBEAM]
        }

        WebAPICreator {
            id: webapiCreator
            property var releaseApi

            onApiCreated: function (api) {
                control.errorMessage = ""

                webView.profile.cachePath = viewModel.getAppCachePath(control.activeApp["appid"])
                webView.profile.persistentStoragePath = viewModel.getAppStoragePath(control.activeApp["appid"])
                webView.url = control.activeApp.url

                var onCallWalletApi = function (request) {
                    api.callWalletApi(request)
                }

                var onCallWalletApiResult = function (result) {
                    webapiBEAM.api.callWalletApiResult(result)
                }

                var onApproveSend = function(request, info, amounts) {
                    info = JSON.parse(info)
                    amounts = JSON.parse(amounts)
                    var dialog = Qt.createComponent("qrc:/send_confirm.qml")
                    var instance = dialog.createObject(control,
                        {
                            amounts:        amounts,
                            addressText:    info["token"],
                            typeText:       info["tokenType"],
                            isOnline:       info["isOnline"],
                            rateUnit:       info["rateUnit"],
                            fee:            info["fee"],
                            feeRate:        info["feeRate"],
                            comment:        info["comment"],
                            appMode:        true,
                            appName:        activeApp.name,
                            showPrefix:     true,
                            assetsProvider: api,
                            isEnough:       info.isEnough
                        })

                    instance.Component.onDestruction.connect(function () {
                         if (instance.result == Dialog.Accepted) {
                            api.sendApproved(request)
                            return
                        }
                        api.sendRejected(request)
                        return
                    })

                    instance.open()
                }

                var onApproveContractInfo = function(request, info, amounts) {
                    info = JSON.parse(info)
                    amounts = JSON.parse(amounts)
                    const dialog = Qt.createComponent("qrc:/send_confirm.qml")
                    const instance = dialog.createObject(control,
                        {
                            amounts:        amounts,
                            rateUnit:       info["rateUnit"],
                            fee:            info["fee"],
                            feeRate:        info["feeRate"],
                            comment:        info["comment"],
                            isSpend:        info["isSpend"],
                            appMode:        true,
                            appName:        activeApp.name,
                            isOnline:       false,
                            showPrefix:     true,
                            assetsProvider: api,
                            isEnough:       info.isEnough
                        })

                    instance.Component.onDestruction.connect(function () {
                         if (instance.result == Dialog.Accepted) {
                            api.contractInfoApproved(request)
                            return
                        }
                        api.contractInfoRejected(request)
                        return
                    })

                    instance.open()
                }

                webapiBEAM.api.callWalletApiCall.connect(onCallWalletApi)
                api.callWalletApiResult.connect(onCallWalletApiResult)
                api.approveSend.connect(onApproveSend)
                api.approveContractInfo.connect(onApproveContractInfo)

                releaseApi = function () {
                    webapiBEAM.api.callWalletApiCall.disconnect(onCallWalletApi)
                    api.callWalletApiResult.disconnect(onCallWalletApiResult)
                    api.approveSend.disconnect(onApproveSend)
                    api.approveContractInfo.disconnect(onApproveContractInfo)
                    webapiCreator.destroyApi()
                    webapiCreator.releaseApi = undefined
                }
            }
        }

        function appSupported(app) {
            return webapiCreator.apiSupported(app.api_version || "current") ||
                   webapiCreator.apiSupported(app.min_api_version || "")
        }

        function createApi(app) {
            try
            {
               // temporary hack
               viewModel.prepareToLaunchApp()

               var verWant = app.api_version || "current"
               var verMin  = app.min_api_version || ""
               webapiCreator.createApi(verWant, verMin, app.name, app.url)
            }
            catch (err)
            {
               control.errorMessage = err.toString()
            }
        }

        function launchApp(app) {
            app["appid"] = webapiCreator.generateAppID(app.name, app.url)
            control.activeApp = app

            if (app.local) {
                viewModel.launchAppServer()
            }

            createApi(app)
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
            visible:           !webLayout.visible

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
                        if (control.activeApp || control.appToOpen) {
                            //% "Please wait, %1 is loading"
                            return qsTrId("apps-loading-app").arg(
                                (control.activeApp || control.appToOpen).name
                            )
                        }

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

                        if (control.activeApp || control.appToOpen) {
                            return true
                        }

                        return false
                    }
                }
            }
        }

        Item {
            id:                  webLayout

            Layout.fillHeight:   true
            Layout.fillWidth:    true
            Layout.bottomMargin: 90
            Layout.topMargin:    20
            visible:             false
            opacity:             txPanel.folded ? 1.0 : 0.25
            clip:                true

            WebEngineView {
                id:              webView
                anchors.fill:    parent
                webChannel:      apiChannel
                visible:         true
                backgroundColor: "transparent"

                profile: WebEngineProfile {
                    httpCacheType:           WebEngineProfile.DiskHttpCache
                    persistentCookiesPolicy: WebEngineProfile.AllowPersistentCookies
                    offTheRecord:            false
                    spellCheckEnabled:       false
                    httpUserAgent:           viewModel.userAgent
                    httpCacheMaximumSize:    536870912 // 5GB
                }

                settings {
                    javascriptCanOpenWindows: false
                }

                onNavigationRequested: function (ev) {
                    if (ev.navigationType == WebEngineNavigationRequest.ReloadNavigation) {

                        if (webapiCreator.releaseApi) {
                            webapiCreator.releaseApi()
                        }

                        if (control.activeApp) {
                            dappsLayout.createApi(control.activeApp)
                        }
                    }
                }

                onNewViewRequested: function (ev) {
                    var url = ev.requestedUrl.toString()
                    Utils.openExternalWithConfirmation(url)
                }

                onLoadingChanged: {
                    // do not change this to declarative style, it flickers somewhy, probably because of delays
                    if (control.activeApp && !this.loading) {
                        viewModel.onCompleted(webView)

                        if(loadRequest.status === WebEngineLoadRequest.LoadFailedStatus) {
                            // code in this 'if' will cause next 'if' to be called
                            control.errorMessage = ["Failed to load", JSON.stringify(loadRequest, null, 4)].join('\n')
                            // no return
                        }

                        if (control.errorMessage.length) {
                            webLayout.visible = false
                            return
                        }

                        webLayout.visible = true
                    }
                }

                onContextMenuRequested: function (req) {
                    if (req.mediaType == ContextMenuRequest.MediaTypeNone && !req.linkText) {
                        if (req.isContentEditable) {
                            req.accepted = true
                            return
                        }
                        if (req.selectedText) return
                    }
                    req.accepted = true
                }
            }

            MouseArea {
                anchors.fill: parent
                visible:      !txPanel.folded
                hoverEnabled: true

                onClicked: function (ev) {
                    txPanel.folded = true
                    ev.accepted = true
                }

                onWheel: function (ev) {
                    ev.accepted = true
                }
            }
        }

        Item {
            width:  dappsLayout.width
            height: dappsLayout.height
            z: 42
            AppInfoPanel {
                id:                  txPanel
                folded:              !control.openedTxID
                state:               control.openedTxID ? "transactions" : "balance"
                width:               parent.width
                anchors.bottom:      parent.bottom
                anchors.bottomMargin: 10
                contentItemHeight:   parent.height * (txPanel.maximized ? 0.79 : 0.36)
                foldsUp:             false
                visible:             webLayout.visible
                bkColor:             Style.background_appstx
                dappName:            (control.activeApp || {}).name || ""
                dappFilter:          (control.activeApp || {}).appid || "all"
                tableOwner:          control
            }
        }

        function loadAppsList () {
            control.appsList = viewModel.apps;
            if (control.appToOpen) {
                for (let i = 0; i < control.appsList.rowCount(); ++i) {
                    let app = control.appsList.get(i);
                    if (webapiCreator.generateAppID(app.name, app.url) == appToOpen.appid) {
                        if (dappsLayout.appSupported(app)) {
                            dappsLayout.launchApp(app)
                        } else {
                            //% "Update Wallet to launch %1 application"
                            BeamGlobals.showMessage(qsTrId("apps-update-message").arg(app.name))
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            viewModel.appsChanged.connect(loadAppsList)
            control.reloadWebEngineView.connect(webView.reload)
            control.showTxDetails.connect(txPanel.showTxDetails)

            viewModel.init(!!appToOpen);
        }

        Component.onDestruction: {
            viewModel.appsChanged.disconnect(loadAppsList)
            control.reloadWebEngineView.disconnect(webView.reload)
            control.showTxDetails.disconnect(txPanel.showTxDetails)
        }
    }

    TransactionIsSent {
        id:           transactionIsSent
        newPublisher: !viewModel.isPublisher
    }

    YouArePublisher {
        id:           youArePublisher
        nickname:     viewModel.publisherInfo.name
        publisherKey: viewModel.publisherInfo.pubkey

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
