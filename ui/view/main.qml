import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import "controls"
import Beam.Wallet 1.0
import "utils.js" as Utils

Rectangle {
    id: main

    property var    openedNotifications: 0
    property var    notificationOffset: 0
    property alias  hasNewerVersion : updateInfoProvider.hasNewerVersion
    readonly property bool devMode: viewModel.isDevMode
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

    function showPopup(popup) {
        increaseNotificationOffset(popup)
        popup.closed.connect(function () {
            if (main) {
                main.decreaseNotificationOffset(popup)
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

    function showAppTxPopup (comment, appname, appicon, txid) {
        var popup = Qt.createComponent("controls/AppTxNotification.qml").createObject(main, {
            comment, appname, appicon, txid
        })
        showPopup(popup)
    }

    function showUpdatePopup (newVersion, currentVersion, id) {
         var popup = Qt.createComponent("controls/UpdateNotification.qml").createObject(main, {
            title: ["New version v", newVersion, "is avalable"].join(" "),
            message: ["Your current version is v", currentVersion, ".Please update to get the most of your Beam wallet."].join(" "),
            acceptButtonText: "update now",
            onCancel: function () {
                updateInfoProvider.onCancelPopup(id);
                popup.close();
            },
            onAccept: function () {
                Utils.navigateToDownloads();
            }
         });
         showPopup(popup)
    }

	MainViewModel {
        id: viewModel
        onClipboardChanged: function(message) {
            showSimplePopup(message)
        }
    }

    PushNotificationManager {
        id: updateInfoProvider
        onShowUpdateNotification: function (newVersion, currentVersion, id) {
            showUpdatePopup (newVersion, currentVersion, id)
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

    property color topColor: Style.background_main_top
    property color topGradientColor: Qt.rgba(Style.background_main_top.r, Style.background_main_top.g, Style.background_main_top.b, 0)


    StatusbarViewModel {
        id: statusbarModel
    }

    property var backgroundRect: mainBackground
    property var backgroundLogo: mainBackgroundLogo

    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: Style.background_main

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 230
            gradient: Gradient {
                GradientStop { position: 0.0; color: main.topColor }
                GradientStop { position: 1.0; color: main.topGradientColor }
            }
        }

        BgLogo {
            id: mainBackgroundLogo
            anchors.leftMargin: sidebar.width
        }
    }

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        propagateComposedEvents: true
        onMouseXChanged: resetLockTimer()
        onPressedChanged: resetLockTimer()
    }

    Keys.onReleased: {
        resetLockTimer()
    }

    function appsQml () {
        return BeamGlobals.isFork3() ? "applications" : "applications_nofork"
    }

    function appArgs (name, appid, showBack) {
        return {
            "appToOpen": { name, appid},
            showBack
        }
    }

    property var contentItems : [
        {name: "wallet"},
        {name: "atomic_swap"},
        {name: "applications", qml: appsQml},
        {name: "daocore", qml: appsQml, args: () => appArgs("BeamX DAO", viewModel.daoCoreAppID, false)},
        {name: "dex"},
        {name: "addresses"},
        {name: "notifications"},
        {name: "settings"}
    ]

    property int selectedItem: -1

    Item {
        id:              sidebar
        width:           70
        anchors.bottom:  parent.bottom
        anchors.left:    parent.left
        anchors.top:     parent.top

        Rectangle {
            anchors.fill: parent
            opacity: 0.1
            border.width: 0
            color: Style.navigation_background
        }

        SvgImage {
            id: image
            y:  50
            anchors.horizontalCenter: parent.horizontalCenter
            source: Style.navigation_logo
            smooth: true
        }

        ColumnLayout {
            anchors.left:       parent.left
            anchors.right:      parent.right
            anchors.top:        parent.top
            anchors.bottom:     parent.bottom
            anchors.topMargin:  130
            spacing:            0

            Repeater {
                id: controls
                model: contentItems

                ColumnLayout {
                    Layout.fillWidth:  true
                    Layout.fillHeight: modelData.name =='addresses'

                    Item {
                        Layout.fillHeight: true
                        visible: modelData.name == 'addresses'
                    }

                    Item {
                        id: control
                        Layout.fillWidth: true
                        Layout.preferredHeight: 66
                        Layout.alignment: Qt.AlignBottom
                        activeFocusOnTab: true

                        SvgImage {
                            id: icon
                            x: 21
                            y: 16
                            width: 28
                            height: 28
                            source: "qrc:/assets/icon-" + modelData.name + (selectedItem == index ? "-active" : "") + ".svg"
                        }

                        Item {
                            Rectangle {
                                id: indicator
                                y: 6
                                width: 4
                                height: 48
                                color: selectedItem == index ? Style.active : Style.passive
                            }

                            DropShadow {
                                anchors.fill: indicator
                                radius: 5
                                samples: 9
                                color: Style.active
                                source: indicator
                            }

                            visible: selectedItem == index
                        }

                        Item {
                            visible: modelData.name == 'notifications' && viewModel.unreadNotifications > 0
                            Rectangle {
                                id: counter
                                x: 42
                                y: 9
                                width: 16
                                height: 16
                                radius: width/2
                                color: Style.active

                                SFText {
                                    height: 14
                                    text: viewModel.unreadNotifications
                                    font.pixelSize: 12
                                    anchors.centerIn: counter
                                }
                            }
                            DropShadow {
                                anchors.fill: counter
                                radius: 5
                                samples: 9
                                source: counter
                                color: Style.active
                            }
                        }

                        Rectangle {
                            x: 10
                            width: parent.width - 20
                            height: 2
                            color: Style.background_button
                            visible: modelData.name == 'dex'
                        }

                        Keys.onPressed: {
                            if ((event.key == Qt.Key_Return || event.key == Qt.Key_Enter || event.key == Qt.Key_Space))
                            if (selectedItem != index) {
                                updateItem(index);
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            onClicked: {
                                control.focus = true
                                if (selectedItem != index) {
                                    updateItem(index);
                                }
                            }
                            hoverEnabled: true
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: content
        anchors.topMargin: 45
        anchors.bottomMargin: 0
        anchors.rightMargin: 20
        anchors.leftMargin: 90
        anchors.fill: parent
        focus: true
    }

    function updateItem(indexOrID, props)
    {
        var update = function(index) {
            var sameTab = selectedItem == index;

            selectedItem = index
            controls.itemAt(index).focus = true;

            var item   = contentItems[index]
            var source = ["qrc:/", item.qml ? item.qml() : item.name, ".qml"].join('')
            var args   = item.args ? item.args() : {}

            content.setSource(source, Object.assign(args, props))
            viewModel.update(index)
        }

        if (typeof(indexOrID) == "string") {
            for (var index = 0; index < contentItems.length; index++) {
                if (contentItems[index].name == indexOrID) {
                    indexOrID = index
                }
            }
        }

        // here we always have a number
        update(indexOrID)
    }

    function openMaxPrivacyCoins (assetId, unitName, lockedAmount) {
        var details = Qt.createComponent("controls/MaxPrivacyCoinsDialog.qml").createObject(main, {
            "unitName":     unitName,
            "lockedAmount": lockedAmount,
            "assetId":      assetId,
       });
       details.open()
    }

    function openWallet () {
        updateItem("wallet")
    }
    function openSendDialog(receiver) {
        updateItem("wallet", {"openSend": true, "token" : receiver})
    }

    function openReceiveDialog(token) {
        updateItem("wallet", {"openReceive": true, "token" : token})
    }

    function openSwapSettings(coinID) {
        updateItem("settings", {swapMode: typeof(coinID) == "string" ? coinID : "ALL"})
    }

    function openSwapActiveTransactionsList() {
        updateItem("atomic_swap", {"shouldShowActiveTransactions": true})
    }

    function openTransactionDetails(id) {
        updateItem("wallet", {"openedTxID": id})
    }

    function openDAppTransactionDetails(txid) {
        if (content.item.openAppTx) {
            return content.item.openAppTx(txid)
        }
        updateItem("applications", {"openedTxID": txid})
    }

    function openSwapTransactionDetails(id) {
        updateItem("atomic_swap", {"openedTxID": id})
    }

    function openApplications () {
        updateItem("applications")
    }

    function resetLockTimer() {
        viewModel.resetLockTimer();
    }

    function openFaucet () {
        var args = appArgs("BEAM Faucet", viewModel.faucetAppID, false);
        updateItem("applications", args);
    }

    function validationSeedBackToSettings() {
        updateItem("settings", { "settingsPrivacyFolded": false});
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
        else
            openWallet();
        main.Window.window.closing.connect(onClosing);
    }

    Component.onDestruction: {
        main.Window.window.closing.disconnect(onClosing)
    }
}
