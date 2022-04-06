import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"
import "../wallet"
import "../utils.js" as Utils
import "."

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27
    spacing:          0

    property string   errorMessage:   ""
    property var      appsList:       undefined
    property var      unsupportedCnt: 0
    property var      activeApp:      undefined
    property var      appToOpen:      undefined
    property string   openedTxID:     ""
    property bool     showBack:       true
    readonly property bool hasApps:   !!appsList && appsList.length > 0

    function openAppTx (txid) {
        openedTxID = txid
        showTxDetails(txid)
    }

    function uninstallApp (app) {
        if (viewModel.uninstallLocalApp(app.appid)) {
            //% "'%1' DApp is successfully uninstalled."
            uninstallOK.text = qsTrId("apps-uninstall-success").arg(app.name)
            uninstallOK.open()
        } else {
            //% "Failed to uninstall '%1' DApp."
            uninstallFail.text = qsTrId("apps-uninstall-fail").arg(app.name)
            uninstallFail.open()
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
            becomePublisherDialog.open();
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
            //% "'%1' is successfully installed."
            installOK.text = qsTrId("apps-install-success").arg(appName)
            installOK.open()
        }

        onAppInstallFail: function (appName) {
            //% "Failed to install DApp:\n%1"
            installFail.text = qsTrId("apps-install-fail").arg(appName)
            installFail.open()
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
            //% "Oops... Failed to create Publisher. Please check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-create-publisher");
            dappStoreFail.open();
        }

        onPublisherEditFail: function() {
            //% "Oops... Failed to edit Publisher. Please check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-edit-publisher");
            dappStoreFail.open();
        }

        onAppPublishFail: function() {
            //% "Oops... Failed to publish DApp. Please check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-publish-dapp");
            dappStoreFail.open();
        }

        onAppRemoveFail: function() {
            //% "Oops... Failed to remove DApp. Please check the log and try again."
            dappStoreFail.text = qsTrId("app-failed-remove-dapp");
            dappStoreFail.open();
        }
    }

    //
    // Page Header (Title + Status Bar)
    //
    Title {
                                                           //% "My DApp Store"
        text: control.activeApp ? control.activeApp.name : qsTrId("apps-title")

        MouseArea {
            visible:         !!control.activeApp
            anchors.fill:    parent
            acceptedButtons: Qt.LeftButton
            cursorShape:     Qt.PointingHandCursor

            onClicked: function () {
                reloadWebEngineView()
            }
        }

        MouseArea {
            visible:         !control.activeApp
            enabled:         stackView.depth > 1
            anchors.fill:    parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled:    true
            cursorShape:     enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked:       stackView.pop()
        }
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    SettingsViewModel {
        id: settings
    }

    Component {
        id: dappsMainLayout

        ColumnLayout {
            id:                dappsLayout
            Layout.fillWidth:  true
            Layout.fillHeight: true
            spacing:           0

            DnDdappInstallDialog {
                id: dndDialog
                onGetFileName: function(fname) {
                    appsListView.installFromFile(fname);
                }
            }

            Item {
                visible:          appsListView.visible
                Layout.fillWidth: true
                opacity:          txPanel.folded ? 1.0 : 0.25
                height:           47

                RowLayout {
                    spacing:           20
                    anchors.right:     parent.right
                    anchors.topMargin: 10

                    CustomButton {
                        id:                     showPublishers
                        height:                 36
                        Layout.preferredHeight: 36
                        width:                  36
                        radius:                 10
                        display:                AbstractButton.IconOnly
                        leftPadding:            6
                        rightPadding:           6
                        palette.button:         Qt.rgba(255, 255, 255, 0.1)
                        icon.source:            "qrc:/assets/icon-dapps_store-publishers.svg"
                        icon.width:             24
                        icon.height:            24
                        onClicked:              navigatePublishersList()
                    }

                    PrimaryButton {
                        id:           publisherDetails
                                      //% "become a publisher"
                        text:         viewModel.isPublisher ? viewModel.publisherInfo.name : qsTrId("apps-become-a-publisher")
                        icon.source:  "qrc:/assets/icon-dapps_store-become-a-publisher.svg"
                        allLowercase: false
                        onClicked:    navigatePublisherDetails()
                    }
                }

                MouseArea {
                    anchors.fill:    parent

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
                visible:           !appsListView.visible && !webLayout.visible

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
                Layout.bottomMargin: txPanel.folded ? 10 : 0
                Layout.topMargin:    20
                visible:             false
                opacity:             txPanel.folded ? 1.0 : 0.25
                clip:                true

                WebEngineScript {
                    id:        userScript
                    sourceUrl: "qrc:/web_view_watcher.js"
                }

                WebEngineView {
                    id:              webView
                    anchors.fill:    parent
                    webChannel:      apiChannel
                    visible:         true
                    backgroundColor: "transparent"
                    userScripts:     [userScript]

                    onJavaScriptConsoleMessage: function (level, message, line, sourceId) {
                        if (message == "no_sleep") main.resetLockTimer();
                    }

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
                                createApi(control.activeApp)
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

            SFText {
                id:                  errCntMessage
                Layout.alignment:    Qt.AlignRight
                Layout.topMargin:    5
                Layout.bottomMargin: 10
                color:               Style.validator_error
                visible:             control.hasApps && !control.activeApp && unsupportedCnt > 0
                font.italic:         true
                                     //% "%n DApp(s) is not available"
                text:                qsTrId("apps-err-cnt", unsupportedCnt)
            }

            AppsList {
                id:                  appsListView
                Layout.topMargin:    unsupportedCnt ? 0 : 20
                Layout.fillHeight:   true
                Layout.fillWidth:    true
                Layout.bottomMargin: txPanel.folded ? 10 : 0
                opacity:             txPanel.folded ? 1.0 : 0.25
                visible:             control.hasApps && !control.activeApp
                appsList:            control.appsList
                isIPFSAvailable:     viewModel.isIPFSAvailable

                onOpenDnd: function () {
                    dndDialog.open();
                }

                onLaunch: function (app) {
                    launchApp(app)
                }

                onInstall: function (app) {
                    viewModel.installApp(app.guid)
                }

                onUpdate: function (app) {
                    viewModel.updateDApp(app.guid)
                }

                onInstallFromFile: function (fname) {
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

                onUninstall: function (app) {
                    control.uninstallApp(app)
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

            AppInfoPanel {
                id:                  txPanel
                folded:              !control.openedTxID
                state:               control.openedTxID ? "transactions" : "balance"
                Layout.fillWidth:    true
                Layout.bottomMargin: 10
                contentItemHeight:   control.height * 0.36
                bottomPadding:       folded ? 20 : 5
                foldsUp:             false
                visible:             appsListView.visible || webLayout.visible
                bkColor:             Style.background_appstx
                dappName:            (control.activeApp || {}).name || ""
                dappFilter:          (control.activeApp || {}).appid || "all"
                tableOwner:          control
            }

            function loadAppsList () {
                control.appsList = checkSupport(viewModel.apps)

                if (control.appToOpen) {
                    for (let app of control.appsList)
                    {
                        if (webapiCreator.generateAppID(app.name, app.url) == appToOpen.appid) {
                            if (appSupported(app)) {
                                launchApp(app)
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
                    app.supported = appSupported(app)
                    if (!app.supported) ++unsupportedCnt
                }
                return apps
            }

            Component.onCompleted: {
                viewModel.appsChanged.connect(loadAppsList)
                control.reloadWebEngineView.connect(webView.reload)
                control.showTxDetails.connect(txPanel.showTxDetails)

                if (!settings.dappsAllowed) {
                    appsDialog.open();
                } else {
                    viewModel.init(!!appToOpen);
                }
            }

            Component.onDestruction: {
                viewModel.appsChanged.disconnect(loadAppsList)
                control.reloadWebEngineView.disconnect(webView.reload)
                control.showTxDetails.disconnect(txPanel.showTxDetails)
            }
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

    OpenApplicationsDialog {
        id: appsDialog

        onRejected: function () {
            settings.dappsAllowed = false
            main.openWallet()
        }
        onAccepted: function () {
            settings.dappsAllowed = true
            viewModel.init(!!appToOpen);
        }
    }

    ConfirmationDialog {
        id: installOK
                             //% "Install DApp"
        title:               qsTrId("app-install-title")
                             //% "Ok"
        okButtonText:        qsTrId("general-ok")
        cancelButtonVisible: false
    }

    ConfirmationDialog {
        id:                      installFail
                                 //% "Install DApp"
        title:                   qsTrId("app-install-title")
                                 //% "Ok"
        okButtonText:            qsTrId("general-ok")
        okButton.palette.button: Style.accent_fail
        cancelButtonVisible:     false
    }

    ConfirmationDialog {
        id:                  uninstallOK
                             //% "Uninstall DApp"
        title:               qsTrId("app-uninstall-title")
                             //% "Ok"
        okButtonText:        qsTrId("general-ok")
        cancelButtonVisible: false
    }

    ConfirmationDialog {
        id:                      uninstallFail
                                 //% "Uninstall DApp"
        title:                   qsTrId("app-uninstall-title")
                                 //% "Ok"
        okButtonText:            qsTrId("general-ok")
        okButton.palette.button: Style.accent_fail
        cancelButtonVisible:     false
    }

    ConfirmationDialog {
        id:                      dappStoreFail
                                 //% "Dapp Store"
        title:                   qsTrId("dapp-store-fail-title")
                                 //% "Ok"
        okButtonText:            qsTrId("general-ok")
        okButton.palette.button: Style.accent_fail
        cancelButtonVisible:     false
                                 //% "Failed to create publisher"
        //text:                    qsTrId("app-failed-create-publisher")
    }

    BecomePublisher {
        id:            becomePublisherDialog

        newPublisher:  !viewModel.isPublisher
        publisherInfo: viewModel.publisherInfo

        onCreatePublisher: function(info) {
            viewModel.createPublisher(info);
        }

        onChangePublisherInfo: function(info) {
            viewModel.changePublisherInfo(info);
        }
    }

    StackView {
        id:                stackView
        Layout.fillWidth:  true
        Layout.fillHeight: true
        initialItem:       dappsMainLayout

        pushEnter: Transition {
            enabled: false
        }
        pushExit: Transition {
            enabled: false
        }
        popEnter: Transition {
            enabled: false
        }
        popExit: Transition {
            enabled: false
        }
        onCurrentItemChanged: {
            if (currentItem && currentItem.defaultFocusItem) {
                stackView.currentItem.defaultFocusItem.forceActiveFocus();
            }
        }
    }
}
