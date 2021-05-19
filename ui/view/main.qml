import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import "controls"
import Beam.Wallet 1.0
import "utils.js" as Utils

Rectangle {
    id: main

    property var openedNotifications: 0
    property alias hasNewerVersion : updateInfoProvider.hasNewerVersion

    anchors.fill: parent

	MainViewModel {
        id: viewModel
    }

    PushNotificationManager {
        id: updateInfoProvider
        onShowUpdateNotification: function(newVersion, currentVersion, id) {
            var popup = Qt.createComponent("controls/NotificationPopup.qml").createObject(main);
            popup.title = "New version v " + newVersion + " is avalable";
            popup.message = "Your current version is v " + currentVersion + ". Please update to get the most of your Beam wallet.";
            popup.acceptButtonText = "update now";
            popup.onCancel = function () {
                updateInfoProvider.onCancelPopup(id);
                popup.close();
            }
            popup.onAccept = function () {
                Utils.navigateToDownloads();
            }
            main.openedNotifications++;
            popup.closed.connect(function() {
                main.openedNotifications--;
            })
            
            popup.verticalOffset = (main.openedNotifications - 1) * 200;
            popup.open();
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

    property var contentItems : [
		{name: "wallet"},
        {name: "atomic_swap"},
		{name: "addresses"},
        {name: "notifications"},
		{name: "utxo"},
		{name: "applications", qml: function () {
		    return BeamGlobals.isFork3() ? "applications" : "applications_nofork"
		}},
		{name: "settings"}
	]

    property int selectedItem

    Item {
        id: sidebar
        width: 70
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top

        Rectangle {
            anchors.fill: parent
            color: Style.navigation_background
            opacity: 0.1
            border.width: 0
        }

        Column {
            width: 0
            height: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 130

            Repeater{
                id: controls
                model: contentItems

                Item {
                    id: control
                    width: parent.width
                    height: 66
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
                        visible: contentItems[index].name == 'notifications' && viewModel.unreadNotifications > 0
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

                    Keys.onPressed: {
                        if ((event.key == Qt.Key_Return || event.key == Qt.Key_Enter || event.key == Qt.Key_Space) && selectedItem != index) 
                            updateItem(index);
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            control.focus = true
                            if (selectedItem != index)
                                updateItem(index)
                        }
						hoverEnabled: true
                    }
                }
            }
        }

        SvgImage {
            id: image
            y:  50
            anchors.horizontalCenter: parent.horizontalCenter
            source: Style.navigation_logo
            smooth: true
        }

        Item {
            property bool clicked: false
            id: whereToBuyControl
            width: parent.width
            anchors.bottom: parent.bottom
            height: 66
            activeFocusOnTab: true

            function clickHandler() {
                whereToBuyControl.clicked = true;
            }

            onClickedChanged: {
                if (clicked) {
                    Utils.openExternalWithConfirmation(
                        "https://www.beam.mw/#exchanges",
                        function () {
                            whereToBuyControl.clicked = false;
                        });
                }
            }

            SvgImage {
                x: 21
                y: 16
                width: 28
                height: 28
                source: whereToBuyControl.clicked
                    ? "qrc:/assets/icon-where-to-buy-beam-green.svg"
                    : "qrc:/assets/icon-where-to-buy-beam-gray.svg"
            }
            Item {
                Rectangle {
                    id: indicator
                    y: 6
                    width: 4
                    height: 48
                    color: whereToBuyControl.clicked ? Style.active : Style.passive
                }

                DropShadow {
                    anchors.fill: indicator
                    radius: 5
                    samples: 9
                    color: Style.active
                    source: indicator
                }

                visible: whereToBuyControl.activeFocus
            }
            Keys.onPressed: {
                if ((event.key == Qt.Key_Return || event.key == Qt.Key_Enter || event.key == Qt.Key_Space) && whereToBuyControl.activeFocus)
                    whereToBuyControl.clickHandler();
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    whereToBuyControl.clickHandler();
                }
                hoverEnabled: true
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
            selectedItem = index
            controls.itemAt(index).focus = true;

            var source = ["qrc:/", contentItems[index].qml ? contentItems[index].qml() : contentItems[index].name, ".qml"].join('')
            content.setSource(source, Object.assign({"openSend": false}, props))

            viewModel.update(index)
        }

        if (typeof(indexOrID) == "string") {
            for (var index = 0; index < contentItems.length; index++) {
                if (contentItems[index].name == indexOrID) {
                    return update(index);
                }
            }
        }

        // plain index passed
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

    function openSwapTransactionDetails(id) {
        updateItem("atomic_swap", {"openedTxID": id})
    }

    function openApplications () {
        updateItem("applications")
    }

    function resetLockTimer() {
        viewModel.resetLockTimer();
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
        updateItem("wallet")
        main.Window.window.closing.connect(onClosing)
    }

    Component.onDestruction: {
        main.Window.window.closing.disconnect(onClosing)
    }
}
