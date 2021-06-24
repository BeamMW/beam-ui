import QtQuick          2.11
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "controls"
import "wallet"
import "./utils.js" as Utils

ColumnLayout {
    id: control
    Layout.fillWidth: true

    property var     appsList: undefined
    property bool    hasApps:  !!appsList && appsList.length > 0
    property string  errorMessage: ""
    property var     activeApp: undefined

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

    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    50
        Layout.bottomMargin: 20

        text: (control.activeApp || {}).name || ""
        visible: !!control.activeApp

        onBack: function () {
            main.openApplications()
        }
    }

    QtObject {
        id: webapiBEAM
        WebChannel.id: "BEAM"

        property var style: Style
        property var api
    }

    WebAPICreator {
        id: webapiCreator

        onApproveContractInfo: function(request, info, amounts) {
            const dialog = Qt.createComponent("send_confirm.qml")
            const instance = dialog.createObject(control,
                {
                    //% "Contract transaction"
                    typeText:       qsTrId("general-contract-transaction"),
                    amounts:        amounts,
                    rateUnit:       info["rateUnit"],
                    fee:            info["fee"],
                    feeRate:        info["feeRate"],
                    comment:        info["comment"],
                    appMode:        true,
                    isOnline:       false,
                    showPrefix:     true,
                    assetsProvider: webapiCreator
                })

            instance.Component.onDestruction.connect(function () {
                 if (instance.result == Dialog.Accepted) {
                    webapiCreator.contractInfoApproved(request)
                    return
                }
                webapiCreator.contractInfoRejected(request)
                return
            })

            instance.open()
        }

        onApproveSend: function(request, info) {
            var dialog   = Qt.createComponent("send_confirm.qml")
            var instance = dialog.createObject(control,
                {
                    addressText:    info["token"],
                    typeText:       info["tokenType"],
                    isOnline:       info["isOnline"],
                    amounts: [{
                        assetID:  info["assetID"],
                        amount:   info["amount"],
                        spend:    true
                    }],
                    rateUnit:       info["rateUnit"],
                    fee:            info["fee"],
                    feeRate:        info["feeRate"],
                    comment:        info["comment"],
                    appMode:        true,
                    showPrefix:     true,
                    assetsProvider: webapiCreator
                })

            instance.Component.onDestruction.connect(function () {
                 if (instance.result == Dialog.Accepted) {
                    webapiCreator.sendApproved(request)
                    return
                }
                webapiCreator.sendRejected(request)
                return
            })

            instance.open()
        }
    }

    WebChannel {
        id: apiChannel
        registeredObjects: [webapiBEAM]
    }

    function launchApp(app) {
        control.activeApp = app

        try
        {
            var apiVersion = app.api_version || "current";

            webapiCreator.onApiCreated.connect(function(api, appid) {
                control.errorMessage = ""
                webapiBEAM.api = api
                webLayout.visible = false

                webView.profile.cachePath = viewModel.getAppCachePath(appid)
                webView.profile.persistentStoragePath = viewModel.getAppStoragePath(appid)
                webView.url = app.url
                appctTable.dappFilter = appid
            })

            webapiCreator.createApi(apiVersion, app.name, app.url)
            return
        }
        catch (err)
        {
            control.errorMessage = err.toString()
            return
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

            onLoadingChanged: {
                // do not change this to declarative style, it flickers somewhy, probably because of delays
                if (control.activeApp && !this.loading) {
                    viewModel.onCompleted(webView)

                    if(loadRequest.status === WebEngineLoadRequest.LoadFailedStatus) {
                        // code in this 'if' will cause next 'if' to be called
                        control.errorMessage = loadRequest.errorString
                        return
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
                    if (req.isContentEditable) return
                    if (req.selectedText) return
                }
                req.accepted = true
            }
        }

        FoldablePanel {
            title:             qsTrId("wallet-transactions-title")
            folded:            true
            titleOpacity:      0.5
            Layout.fillWidth:  true
            contentItemHeight: appsView.height * 0.4
            bottomPadding:     folded ? 20 : 5

            content: TxTable {
                id: appctTable
                emptyMessageMargin: 60
                headerShaderVisible: false
                dappFilter: "all"
            }
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth:  true
        visible: !appsView.visible && !webLayout.visible

        SFText {
            anchors.horizontalCenter: parent.horizontalCenter
            y:       parent.height / 2 - this.height / 2 - 40
            color:   control.errorMessage.length ? Style.validator_error : Style.content_main
            opacity: control.hasApps ? 0.5 : 1

            font {
                styleName: "DemiBold"
                weight:    Font.DemiBold
                pixelSize: 18
            }

            text: {
                if (control.errorMessage.length) {
                    return control.errorMessage
                }

                if (!control.hasApps) {
                    //% "There are no applications at the moment"
                    return qsTrId("apps-nothing")
                }

                if (control.activeApp) {
                    //% "Loading '%1'..."
                    return qsTrId("apps-loading-app").arg(control.activeApp.name)
                }

                //% "Loading..."
                return qsTrId("apps-loading")
            }
        }
    }

    ColumnLayout {
        id: appsView
        Layout.topMargin:  50
        Layout.fillHeight: true
        Layout.fillWidth:  true
        Layout.bottomMargin: 10
        visible: control.hasApps && !control.activeApp
        spacing: 20

        // Actuall apps list
        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth:  true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            clip: true

            ColumnLayout
            {
                width: parent.width
                spacing: 15

                Repeater {
                    model: control.appsList
                    delegate: Item {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 100

                        Rectangle {
                            anchors.fill: parent
                            radius:       10
                            color:        Style.active
                            opacity:      hoverArea.containsMouse ? 0.15 : 0.1
                        }

                        RowLayout {
                            anchors.fill: parent

                            Rectangle {
                                Layout.leftMargin: 30
                                width:  60
                                height: 60
                                radius: 30
                                color:  Style.background_main

                                SvgImage {
                                    id: defaultIcon
                                    source: hoverArea.containsMouse ? "qrc:/assets/icon-defapp-active.svg" : "qrc:/assets/icon-defapp.svg"
                                    sourceSize: Qt.size(30, 30)
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    visible: !customIcon.visible
                                }

                                SvgImage {
                                    id: customIcon
                                    source: modelData.icon ? modelData.icon : "qrc:/assets/icon-defapp.svg"
                                    sourceSize: Qt.size(30, 30)
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    smooth: true
                                    visible: !!modelData.icon && progress == 1.0
                                }
                            }

                            Column {
                                Layout.leftMargin: 20
                                spacing: 10

                                SFText {
                                    text: modelData.name
                                    font {
                                        styleName:  "DemiBold"
                                        weight:     Font.DemiBold
                                        pixelSize:  18
                                    }
                                    color: Style.content_main
                                }

                                SFText {
                                    text: Utils.limitText(modelData.description, 80)
                                    font.pixelSize:  14
                                    elide: Text.ElideRight
                                    color: Style.content_main
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            CustomButton {
                                id: launchBtn
                                Layout.rightMargin: 20
                                height: 40
                                palette.button: Style.background_second
                                palette.buttonText : Style.content_main
                                icon.source: "qrc:/assets/icon-run.svg"
                                icon.height: 16
                                visible: webapiCreator.apiSupported(model.api_version || "current")
                                //% "launch"
                                text: qsTrId("apps-run")

                                MouseArea {
                                    anchors.fill:     parent
                                    acceptedButtons:  Qt.LeftButton
                                    hoverEnabled:     true
                                    propagateComposedEvents: true
                                    preventStealing:  true
                                    onClicked:        launchApp(modelData)
                                }
                            }

                            SFText {
                                Layout.rightMargin: 25
                                //% "Update Beam Wallet to launch"
                                text: qsTrId("apps-update-wallet")
                                color: Style.validator_error
                                visible: !launchBtn.visible
                            }
                        }

                        MouseArea {
                            id:               hoverArea
                            anchors.fill:     parent
                            // acceptedButtons:  Qt.LeftButton
                            hoverEnabled:     true
                            propagateComposedEvents: true
                            preventStealing: true
                        }
                    }
                }
            }
        }

        FoldablePanel {
            title:             qsTrId("wallet-transactions-title")
            folded:            true
            titleOpacity:      0.5
            Layout.fillWidth:  true
            contentItemHeight: appsView.height * 0.4
            bottomPadding:     folded ? 20 : 5

            content: TxTable {
                id: allctTable
                emptyMessageMargin: 60
                headerShaderVisible: false
                dappFilter: "all"
            }
        }
    }

    function appendDevApp (arr) {
        if (viewModel.devAppName) {
            arr = arr || []
            arr.unshift({
                //% "This is your dev application"
                "description":  qsTrId("apps-devapp"),
                "name":         viewModel.devAppName,
                "url":          viewModel.devAppUrl,
                "api_version":  viewModel.devAppApiVer
            })
            return arr
        }
        return arr
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
                        control.appsList = appendDevApp(list)
                    }
                    else
                    {
                        var errMsg = errTemplate.arg(["code", xhr.status].join(" "))
                        control.errorMessage = errMsg
                    }
                }
            }
            xhr.open('GET', viewModel.appsUrl, true)
            xhr.send('')
        }
        control.appsList = appendDevApp(undefined)
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
        if (settings.dappsAllowed) {
            loadAppsList()
        } else {
            appsDialog.open()
        }
    }
}
