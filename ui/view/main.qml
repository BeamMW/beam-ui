import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.15
import QtQuick.Window 2.2
import "controls"
import Beam.Wallet 1.0
import "utils.js" as Utils

Rectangle {
    id: main

    property var    openedNotifications: []
    property var    notificationOffset: 0
    property alias  hasNewerVersion : notificationManager.hasNewerVersion
    readonly property bool devMode: viewModel.isDevMode
    readonly property string accountLabel: viewModel.accountLabel
    readonly property string accountPicture: viewModel.accountPicture
    readonly property alias unreadNotifications : viewModel.unreadNotifications
    property alias statusBar: statusBarInternal
    anchors.fill:   parent

    function increaseNotificationOffset(popup) {
        popup.verticalOffset = main.notificationOffset
        main.notificationOffset += popup.height + 10
        popup.nextVerticalOffset = main.notificationOffset
    }

    function decreaseNotificationOffset(popup) {
        main.notificationOffset -= (popup.nextVerticalOffset - popup.verticalOffset)
        if (main.notificationOffset < 0) main.notificationOffset = 0
    }

    function closeNotification(popup) {
        var closedNotificationIndex = openedNotifications.indexOf(popup)
        openedNotifications.splice(closedNotificationIndex, 1)
        
        for (var i = closedNotificationIndex; i < openedNotifications.length; ++i) {
            var oldVerticalOffset = openedNotifications[i].verticalOffset
            
            openedNotifications[i].verticalOffset  -= openedNotifications[i].nextVerticalOffset - openedNotifications[i].verticalOffset
            if (openedNotifications[i].verticalOffset < 0)  openedNotifications[i].verticalOffset = 0
                    
            openedNotifications[i].nextVerticalOffset = oldVerticalOffset
        }
    }

    function showPopup(popup) {
        increaseNotificationOffset(popup)
        openedNotifications.push(popup)

        popup.closed.connect(function () {
            if (main) {
                main.decreaseNotificationOffset(popup)
                main.closeNotification(popup)
            }
        })
        popup.open();
    }

    function showSimplePopup(message) {
        var popup = Qt.createComponent("controls/SimpleNotification.qml").createObject(main, {
            message
        })
        showPopup(popup)
    }

    function showAppTxPopup (comment, appname, appicon, txid, isLinkToWalletMainTxTable=false) {
        var popup = Qt.createComponent("controls/AppTxNotification.qml").createObject(main, {
            comment, appname, appicon, txid, isLinkToWalletMainTxTable
        })
        showPopup(popup)
    }

    function showUpdatePopup (newVersion, currentVersion, id) {
         var popup = Qt.createComponent("controls/UpdateNotification.qml").createObject(main, {
            title: ["New version v", newVersion, "is avalable"].join(" "),
            message: ["Your current version is v", currentVersion, ".Please update to get the most of your Beam wallet."].join(" "),
            acceptButtonText: "update now",
            onCancel: function () {
                notificationManager.onCancelPopup(id);
                popup.close();
            },
            onAccept: function () {
                Utils.navigateToDownloads();
            }
         });
         showPopup(popup)
    }

    function closeContractNotification(txId) {
        notificationManager.closeContractNotification(txId);
    }

    ConfirmationDialog {
        id: approveHWAction
        //% "Transaction"
        title:                  qsTrId("approve-on-hw-wallet-title")
        //% "Transaction is in process.\nConnect your Hardware Wallet to finalize the transaction."
        text:                   qsTrId("approve-on-hw-wallet-text")
        okButtonVisible:        false
        cancelButtonVisible:    false
        closePolicy:            Popup.NoAutoClose
    }

    MainViewModel {
        id: viewModel
        onClipboardChanged: function(message) {
            showSimplePopup(message)
        }
        onHwError: function(message) {
            if (message.length)
            {
                approveHWAction.open();
            }
            else
            {
                approveHWAction.close();
            }
        }
    }

    PushNotificationManager {
        id: notificationManager
        onShowUpdateNotification: function (newVersion, currentVersion, id) {
            showUpdatePopup (newVersion, currentVersion, id)
        }

        onShowContractNotification: function(txId, appName, comment, appicon) {
            showAppTxPopup(comment, appName, appicon, txId);
        }
    }

    ConfirmationDialog {
        id:                     closeDialog
        //% "Beam wallet close"
        title:                  qsTrId("app-close-title")
        //% "There are %1 active transactions that might fail if the wallet will go offline. Are you sure to close the wallet now?"
        text:                   qsTrId("app-close-text").arg(viewModel.unsafeTxCount)
        //% "yes"
        okButtonText:           qsTrId("atomic-swap-tx-yes-button")
        okButtonIconSource:     "qrc:/assets/icon-done.svg"
        okButtonColor:          Style.swapStateIndicator
        //% "no"
        cancelButtonText:       qsTrId("atomic-swap-no-button")
        cancelButtonIconSource: "qrc:/assets/icon-cancel-16.svg"
        
        onOpened: {
            closeDialog.visible = Qt.binding(function(){return viewModel.unsafeTxCount > 0;});
        }

        onClosed: {
            closeDialog.visible = false;
        }

        onAccepted: {
            Qt.quit();
        }
        modal: true
    }

    SeedValidationHelper { id: seedValidationHelper }

    function onClosing (close) {
        if (viewModel.unsafeTxCount > 0) {
            close.accepted = false;
            closeDialog.open();
        }
    }

    StatusbarViewModel {
        id: statusbarModel
    }

    property color topColor:            Style.background_main_top
    property color topGradientColor:    Qt.rgba(Style.background_main_top.r, Style.background_main_top.g, Style.background_main_top.b, 0)
    property var backgroundRect:        mainBackground
    property var backgroundLogo:        mainBackgroundLogo


    Rectangle {
        id:             mainBackground
        anchors.fill:   parent
        color:          Style.background_main

        Rectangle {
            anchors.left:   parent.left
            anchors.right:  parent.right
            height: 230
            gradient: Gradient {
                GradientStop { position: 0.0; color: main.topColor }
                GradientStop { position: 1.0; color: main.topGradientColor }
            }
        }

        BgLogo {
            id: mainBackgroundLogo
        }
    }

    Keys.onReleased: {
        viewModel.resetLockTimer();
    }

    function appsQml () {
        return BeamGlobals.isFork3() ? "applications/applications" : "applications/applications_nofork"
    }

    StatusBar {
        id:              statusBarInternal
        model:           statusbarModel
        anchors.right:   parent.right
        anchors.left:    parent.left
        anchors.top:     parent.top
        height:          24
        z:               33
    }

    ColumnLayout {
        anchors.topMargin:      30
        anchors.bottomMargin:   0
        anchors.rightMargin:    20
        anchors.leftMargin:     20
        anchors.fill:           parent
        spacing: 0

        Title {
            Layout.fillWidth:   true
            text:               contentStack.currentItem.title
            path:               getNavigationPath()
            content:            contentStack.currentItem.titleContent ? contentStack.currentItem.titleContent : null
            canNavigate:        contentStack.depth > 1
            onNavigate: function(index) {
                var item = contentStack.get(index);
                contentStack.pop(item);
            }
            
            function getNavigationPath() {
                var path = [];
                if (contentStack.depth <= 1)
                    return path;
                for (var i = 0; i < contentStack.depth-1; ++i) {
                    path.push(contentStack.get(i).title);
                }
                return path;
            }
        }

        StackView {
            id:                     contentStack
            Layout.topMargin:       30
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            focus:                  true
            initialItem:            Qt.createComponent("qrc:/applications/applications.qml")

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
                    contentStack.currentItem.defaultFocusItem.forceActiveFocus();
                }
            }
        }
    }

    function navigateTo(name, props) {
        var source = ["qrc:/", name, ".qml"].join('')
        console.log("Push: " + source)
        contentStack.push(Qt.createComponent(source), props)
    }

    function openMaxPrivacyCoins (assetId, unitName, lockedAmount) {
        var details = Qt.createComponent("controls/MaxPrivacyCoinsDialog.qml").createObject(main, {
            "unitName":     unitName,
            "lockedAmount": lockedAmount,
            "assetId":      assetId,
       });
       details.open()
    }

    function openHelp() {
        Utils.openExternalWithConfirmation("https://beam.mw/docs");
    }

    function openWallet() {
        navigateTo("wallet")
    }

    function goBack() {
        contentStack.pop()
    }
    
    function navigateSend(assetId) {
        openSendDialog("", assetId)
    }

    function navigateReceive(assetId) {
        openReceiveDialog("", assetId)
    }

    function openSendDialog(receiver, assetId) {
        var params = {
            "onAccepted":    goBack,
            "onClosed":      goBack,
            "receiverToken": receiver,
            "assetId":       assetId
        }
        if (assetId != undefined)
        {
            params["assetId"] = assetId >= 0 ? assetId : 0
        }
        navigateTo("send_regular", params)
    }

    function openReceiveDialog(token, assetId) {
        var params = {
            "onClosed": goBack,
            "token":    token,
            "assetId":  assetId
            };
        if (assetId != undefined)
        {
            params["assetId"] = assetId >= 0 ? assetId : 0
        }
        navigateTo("receive_regular", params)
    }

    function openSettings(section = "") {
        if (contentStack.currentItem instanceof Settings) {
            contentStack.currentItem.unfoldSection = section
            return
        }
        if (section == "") {
            navigateTo("Settings")
        } else {
            navigateTo("Settings", {"unfoldSection": section})
        }
    }

    function openNotifications() {
        contentStack.pop(contentStack.get(0));
        navigateTo("notifications");
    }

    function openSwapActiveTransactionsList() {
        navigateTo("atomic_swap", {"shouldShowActiveTransactions": true})
    }

    function openTransactionDetails(id) {
        openDAppTransactionDetails(id)
    }

    function openDAppTransactionDetails(txid) {
        contentStack.pop(contentStack.get(0));
        if (contentStack.currentItem.openAppTx) {
            return contentStack.currentItem.openAppTx(txid)
        }
        navigateTo(appsQml(), {"openedTxID": txid})
    }

    function openSwapTransactionDetails(id) {
        navigateTo("atomic_swap", {"openedTxID": id})
    }

    function openApplications () {
        navigateTo(appsQml())
    }

    function validationSeedBackToSettings() {
        navigateTo("Settings", { "settingsPrivacyFolded": false});
    }

    function openAssetSwaps() {
        navigateTo("assets_swap")
    }

    function openAtomicSwaps() {
        navigateTo("atomic_swap")
    }

    function openMessenger() {
        contentStack.pop(contentStack.get(0));
        navigateTo("beam_messenger")
    }

    function openAddresses() {
        contentStack.pop(contentStack.get(0));
        navigateTo("addresses")
    }

    property var trezor_popups : []

    Connections {
        target: viewModel
        function onGotoStartScreen () {
            main.parent.setSource("qrc:/start.qml", {"isLockedMode": true});
        }

        function onShowTrezorMessage () {
            var popup = Qt.createComponent("popup_message.qml").createObject(main)
            //% "Please, look at your Trezor device to complete actions..."
            popup.message = qsTrId("trezor-message")
            popup.open()
            trezor_popups.push(popup)
        }

        function onHideTrezorMessage () {
            console.log("onHideTrezorMessage")
            if (trezor_popups.length > 0) {
                var popup = trezor_popups.pop()
                popup.close()
            }
        }

        function onShowTrezorError (error) {
            console.log(error)
            var popup = Qt.createComponent("popup_message.qml").createObject(main)
            popup.message = error
            popup.open()
            trezor_popup.push(popup)
        }
    }

    Component.onCompleted: {
        if (seedValidationHelper.isTriggeredFromSettings)
            validationSeedBackToSettings();

        main.Window.window.closing.connect(onClosing);
    }

    Component.onDestruction: {
        main.Window.window.closing.disconnect(onClosing)
    }
}
