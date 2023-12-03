import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "."

Item {
    id: rootControl

    property var model

    function getStatus() {
        if (model.isFailedHww)
            return "error_hww";
        if (model.isCoinClientFailed || model.ipfsError)
            return "error_3rd";
        if (model.isFailedStatus)
            return "error";
        else if (model.isSyncInProgress)
            return "updating";
        else if (model.isOnline)
            return "online";
        else
            return "connecting";
    }
    
    property string status: getStatus()
    state: getStatus()

    property int indicator_radius: 5
    property Item indicator: online_indicator
    property string walletError: model.walletError
    property string localNodeError: model.localNodeError
    property string error_msg_3rd_client: model.coinClientErrorMsg || model.ipfsError
    //% "online"
    property string statusOnline: qsTrId("status-online")
    //% "connected node supports online transactions only"
    property string statusOnlineRemote: qsTrId("status-online-remote")
    property var indicatorX: 60
    property var indicatorY: 32
    property real nodeSyncProgress: model.nodeSyncProgress

    function setIndicator(indicator) {
        if (indicator !== rootControl.indicator) {
            rootControl.indicator.visible = false;
            rootControl.indicator = indicator;
            rootControl.indicator.visible = true;
        }
    }

    Item {
        id: online_indicator
        x: rootControl.indicatorX
        y: rootControl.indicatorY
        width:  childrenRect.width
        height: childrenRect.height

        property color color: Style.online
        property int radius: rootControl.indicator_radius

        Column {
            id: indicators
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 0
                topMargin: 2
            }

            SvgImage {
                id:          onlineTrusted
                width:       10
                height:      10
                sourceSize:  Qt.size(10, 10)
                source:      model.isExchangeRatesUpdated && !model.ipfsError ? "qrc:/assets/icon-trusted-node-status.svg" : "qrc:/assets/icon-trusted-node-status-stale.svg"
                visible:     model.isConnectionTrusted && !model.isCoinClientFailed && !model.ipfsError
            }

            Rectangle {
                id:       online_rect
                width:    rootControl.indicator_radius * 2
                height:   rootControl.indicator_radius * 2
                radius:   rootControl.indicator_radius
                color:    online_indicator.color
                visible:  !onlineTrusted.visible
            }
        }

        DropShadow {
            radius: 5
            samples: 9
            anchors.fill: indicators
            source: indicators
            color: online_indicator.color
        }
    }

    UpdateIndicator {
        id:         update_indicator
        x:          rootControl.indicatorX
        y:          rootControl.indicatorY
        visible:    false
        radius:     rootControl.indicator_radius
    }
    Rectangle {
        id:                     rowBackground
        anchors.leftMargin:     0
        anchors.left:           parent.left
        anchors.right:          parent.right
        height:                 24
        color:                  "transparent"
        visible:                !onlineTrusted.visible || !model.isExchangeRatesUpdated || model.isFailedHww
        property color gradientColor: online_indicator.color

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end:   Qt.point(rowBackground.width, 0)

            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0)}
                GradientStop { position: 0.5; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0.3)}
                GradientStop { position: 1.0; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0)}
            }
        }

        ColumnLayout {
            spacing: 0
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth:   true
                Layout.rightMargin: 20
                spacing:            8

                Item {
                    Layout.fillWidth:   true
                    Layout.fillHeight:  true
                }

                SFText {
                    id:                 statusText
                    color:              Style.content_main
                    font.pixelSize:     12
                    elide:              Text.ElideRight
                }

                SFText {
                    id:                 progressText
                    color:              Style.content_main
                    font.pixelSize:     12
                    text:               "(" + model.nodeSyncProgress.toFixed(2) + "%)"
                    visible:            model.nodeSyncProgress > 0 && update_indicator.visible
                }

                LinkButton {
                    id: settingsBtn
                    //% "Change settings"
                    text: qsTrId("status-change-settings")
                    visible: model.ipfsError || 
                             model.isCoinClientFailed || 
                             model.isFailedStatus || 
                             (model.isOnline && !model.isConnectionTrusted && !model.isSyncInProgress)
                    fontSize: 12

                    onClicked: function () {
                        if (model.isCoinClientFailed || model.isFailedStatus) {
                            main.openSettings(model.coinWithErrorLabel())
                            return
                        }

                        if (model.ipfsError) {
                            main.openSettings("IPFS_NODE")
                            return
                        }

                        main.openSettings("BEAM_NODE")
                        return
                    }
                }

                //Item {
                //    Layout.fillWidth:   true
                //    Layout.fillHeight:  true
                //}
            }

            CustomProgressBar {
                id: progress_bar
                backgroundImplicitWidth: parent.width
                contentItemImplicitWidth: parent.width

                visible: model.nodeSyncProgress > 0 && update_indicator.visible
                value: model.nodeSyncProgress / 100
            }
        }
    }

    states: [
        State {
            name: "connecting"
            PropertyChanges {
                target: statusText;
                //% "connecting"
                text: qsTrId("status-connecting") + model.branchName
            }
            StateChangeScript {
                name: "connectingScript"
                script: {
                    rootControl.setIndicator(update_indicator);
                }
            }
        },
        State {
            name: "online"
            PropertyChanges {
                target: statusText;
                text: statusOnline + (model.isConnectionTrusted || !model.isExchangeRatesUpdated ? "" : ": " + statusOnlineRemote) + model.branchName + 
                    (
                        model.isExchangeRatesUpdated ? "" : " " + model.exchangeStatus
                    )
            }
            StateChangeScript {
                name: "onlineScript"
                script: {
                    online_indicator.color = model.isCoinClientFailed || model.ipfsError ? Style.accent_fail : (model.isExchangeRatesUpdated ? Style.online : Style.validator_warning);
                    rootControl.setIndicator(online_indicator);
                }
            }
        },
        State {
            name: "updating"
            PropertyChanges {
                target: statusText;
                //% "synchronizing blockchain..."
                text: qsTrId("status-updating")
            }
            StateChangeScript {
                name: "updatingScript"
                script: {
                    rootControl.setIndicator(update_indicator);
                }
            }
        },
        State {
            name: "error"
            PropertyChanges {
                target: statusText;
                text: rootControl.walletError + model.branchName
            }
            StateChangeScript {
                name: "errorScript"
                script: {
                    online_indicator.color = Style.accent_fail;
                    rootControl.setIndicator(online_indicator);
                }
            }
        },
        State {
            name: "error_hww"
            PropertyChanges {
                target: statusText;
                text: model.hwwError;
                color: Style.accent_fail;
                font.pixelSize: 20
            }
        },
        State {
            name: "error_3rd"
            PropertyChanges {
                target: statusText;
                text: rootControl.error_msg_3rd_client + model.branchName
            }
            StateChangeScript {
                name: "errorScript"
                script: {
                    online_indicator.color = Style.accent_fail;
                    rootControl.setIndicator(online_indicator);
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "online"
            to: "updating"
            SequentialAnimation {
                PauseAnimation { duration: 1000 }
                ScriptAction { scriptName: "updatingScript" }
            }
        },
        Transition {
            from: "error"
            to: "online"
            SequentialAnimation {
                PauseAnimation { duration: 500 }
                ScriptAction { scriptName: "onlineScript" }
            }
        }
    ]
}
