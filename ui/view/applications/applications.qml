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

    property string   errorMessage: ""
    property var      appsList: undefined
    property var      unsupportedCnt: 0
    property var      activeApp: undefined
    property var      appToOpen: undefined
    property string   openedTxID: ""
    property bool     showBack: true
    readonly property bool hasApps: !!appsList && appsList.length > 0

    function openAppTx (txid) {
        openedTxID = txid
        txTable.showTxDetails(txid)
    }

    ApplicationsViewModel {
        id: viewModel
    }

    //
    // Page Header (Title + Status Bar)
    //
    Title {
        //% "DAPP Store"
        text: qsTrId("apps-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    // Subtitle row is invisible only when we display appslit
    // In all other cases we display it
    //   - if app, with back button & app title
    //   - if no app (loading) without back button & title.
    //     This is to avoid 'Loading (appname)...' message jumping vertically
    SubtitleRow {
        id: backRow
        Layout.fillWidth:    true
        Layout.topMargin:    50
        Layout.bottomMargin: 20

        visible:  !appsListView.visible
        showBack: control.showBack && !!text
        text:     ((control.activeApp || {}).name || "")

        onBack: function () {
            main.openApplications()
        }

        onRefresh: function () {
            webView.reload()
        }
    }

    //
    // This object is visible to web. We create such proxy
    // to ensure that nothing (methods, events, props &c)
    // is leaked to the web from real API
    //
    QtObject {
        id: webapiBEAM
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
        id: apiChannel
        registeredObjects: [webapiBEAM]
    }

    WebAPICreator {
        id: webapiCreator
        property var releaseApi

        onApiCreated: function (api) {
            control.errorMessage = ""
            webLayout.visible = false

            webView.profile.cachePath = viewModel.getAppCachePath(control.activeApp["appid"])
            webView.profile.persistentStoragePath = viewModel.getAppStoragePath(control.activeApp["appid"])
            webView.url = control.activeApp.url

            var onCallWalletApi = function (request) {
                api.callWalletApi(request)
            }

            var onCallWalletApiResult = function (result) {
                webapiBEAM.api.callWalletApiResult(result)
                try
                {
                    var json = JSON.parse(result)
                    var txid = ((json || {}).result || {}).txid
                    if (txid) txTable.showAppTxNotifcation(txid, control.activeApp.icon)
                }
                catch (e) {
                    BeamGlobals.logInfo(["callWalletApiResult json parse fail:", e].join(": "))
                }
            }

            var onApproveSend = function(request, info, amounts) {
                info = JSON.parse(info)
                amounts = JSON.parse(amounts)
                var dialog = Qt.createComponent("send_confirm.qml")
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
                const dialog = Qt.createComponent("send_confirm.qml")
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
        visible: !appsListView.visible && !webLayout.visible

        ColumnLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height / 2 - this.height / 2 - 40
            spacing: 40

            SFText {
                Layout.alignment: Qt.AlignHCenter
                color: control.errorMessage.length ? Style.validator_error : Style.content_main
                opacity: 0.5

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
                source: "qrc:/assets/dapp-loading.svg"
                sourceSize: Qt.size(245, 140)

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

    ColumnLayout {
        id: webLayout

        Layout.fillHeight:   true
        Layout.fillWidth:    true
        Layout.bottomMargin: 10
        visible: false

        WebEngineView {
            id: webView

            Layout.fillWidth:    true
            Layout.fillHeight:   true
            Layout.bottomMargin: 10

            webChannel: apiChannel
            visible: true
            backgroundColor: "transparent"

            profile: WebEngineProfile {
                httpCacheType:           WebEngineProfile.DiskHttpCache
                persistentCookiesPolicy: WebEngineProfile.AllowPersistentCookies
                offTheRecord:            false
                spellCheckEnabled:       false
                httpUserAgent:           viewModel.userAgent
                httpCacheMaximumSize:    0
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

            onLoadingChanged: {
                // do not change this to declarative style, it flickers somewhy, probably because of delays
                if (control.activeApp && !this.loading) {
                    viewModel.onCompleted(webView)

                    if(loadRequest.status === WebEngineLoadRequest.LoadFailedStatus) {
                        // code in this 'if' will cause next 'if' to be called
                        control.errorMessage = ["Failed to load:", JSON.stringify(loadRequest, null, 4)].join('\n')
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
    }

    SFText {
        id: errCntMessage
        Layout.alignment: Qt.AlignRight
        Layout.topMargin: 5
        color: Style.validator_error
        visible: control.hasApps && !control.activeApp && unsupportedCnt > 0
        font.italic: true
        //% "%n DApp(s) is not available"
        text: qsTrId("apps-err-cnt", unsupportedCnt)
    }

    AppsList {
        id: appsListView
        Layout.topMargin:  40 - (unsupportedCnt ? errCntMessage.height + 5 + parent.spacing : 0)
        Layout.fillHeight: true
        Layout.fillWidth:  true
        Layout.bottomMargin: 10
        visible: control.hasApps && !control.activeApp
        appsList: control.appsList
    }

    FoldablePanel {
        title:               qsTrId("wallet-transactions-title")
        folded:              !control.openedTxID
        titleOpacity:        0.5
        Layout.fillWidth:    true
        Layout.bottomMargin: 10
        contentItemHeight:   control.height * 0.32
        bottomPadding:       folded ? 20 : 5
        foldsUp:             false
        visible:             appsListView.visible || webLayout.visible
        bkColor:             Style.background_appstx

        content: TxTable {
            id:    txTable
            owner: control
            emptyMessageMargin: 60
            headerShaderVisible: false
            dappFilter: (control.activeApp || {}).appid || "all"
        }

        //% "(%1 active)"
        titleTip: txTable.activeTxCnt ? qsTrId("apps-inprogress-tip").arg(txTable.activeTxCnt) : ""
    }

    function appendLocalApps (arr) {
        return viewModel.localApps.concat(arr || [])
    }

    function loadAppsList () {
        if (viewModel.appsUrl.length) {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function()
            {
                //% "Failed to load applications list, %1"
                var errTemplate = qsTrId("apps-load-error")
                if(xhr.readyState === XMLHttpRequest.DONE)
                {
                    if (xhr.status === 200)
                    {
                        var list = JSON.parse(xhr.responseText)
                        control.appsList = checkSupport(appendLocalApps(list))

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
                    else
                    {
                        control.appsList = checkSupport([])
                        var errMsg = errTemplate.arg(["code", xhr.status].join(" "))
                        control.errorMessage = errMsg
                    }
                }
            }
            xhr.open('GET', viewModel.appsUrl, true)
            xhr.send('')
        }

        control.appsList = checkSupport(appendLocalApps(undefined))
    }

    function checkSupport (apps) {
        unsupportedCnt = 0
        for (var app of apps) {
            app.supported = appSupported(app)
            if (!app.supported) ++unsupportedCnt
        }
        return apps
    }

    SettingsViewModel {
        id: settings
    }

    OpenApplicationsDialog {
        id: appsDialog
        onRejected: function () {
            settings.dappsAllowed = false
            main.openWallet()
        }
        onAccepted: function () {
            settings.dappsAllowed = true
            loadAppsList()
        }
    }

    Component.onCompleted: {
        if (settings.dappsAllowed)
        {
            loadAppsList();
        }
        else
        {
            appsDialog.open();
        }
    }
}
