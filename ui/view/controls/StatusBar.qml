import QtQuick 2.11
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."

Item {
    id: rootControl
    y: 55

    property var model

    function getStatus() {
        if (model.isCoinClientFailed)
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
    property string error_msg: model.walletStatusErrorMsg
    property string error_msg_3rd_client: model.coinClientErrorMsg
    //% "online"
    property string statusOnline: qsTrId("status-online")
    //% "connected node supports online transactions only"
    property string statusOnlineRemote: qsTrId("status-online-remote")

    function setIndicator(indicator) {
        if (indicator !== rootControl.indicator) {
            rootControl.indicator.visible = false;
            rootControl.indicator = indicator;
            rootControl.indicator.visible = true;
        }
    }

    Item {
        id: online_indicator
        anchors.top: parent.top
        anchors.left: parent.left
        width: childrenRect.width

        property color color: Style.online
        property int radius: rootControl.indicator_radius

        Rectangle {
            id: online_rect
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.topMargin: 2

            width:      rootControl.indicator_radius * 2
            height:     rootControl.indicator_radius * 2
            radius:     rootControl.indicator_radius
            color:      parent.color
            visible:    !model.isConnectionTrusted || model.isCoinClientFailed
        }

        SvgImage {
            id:              onlineTrusted
            anchors.top:     parent.top
            anchors.left:    parent.left
            anchors.leftMargin: 0
            anchors.topMargin: 2
            width: 10
            height: 10
            sourceSize:     Qt.size(10, 10)
            source:         model.isExchangeRatesUpdated ? "qrc:/assets/icon-trusted-node-status.svg" : "qrc:/assets/icon-trusted-node-status-stale.svg"
            visible:        model.isConnectionTrusted && !model.isCoinClientFailed
        }

        DropShadow {
            anchors.fill: model.isConnectionTrusted && !model.isCoinClientFailed ? onlineTrusted : online_rect
            radius: 5
            samples: 9
            source: model.isConnectionTrusted && !model.isCoinClientFailed ? onlineTrusted : online_rect
            color: parent.color
        }
    }

    Item {
        id: update_indicator
        anchors.top: parent.top
        anchors.left: parent.left
        visible: false

        property color color: Style.online
        property int circle_line_width: 2
        property int animation_duration: 2000

        width: 2 * rootControl.indicator_radius + circle_line_width
        height: 2 * rootControl.indicator_radius + circle_line_width

        Canvas {
            id: canvas_
            anchors.fill: parent
            onPaint: {
                var context = getContext("2d");
                context.arc(width/2, height/2, width/2 - parent.circle_line_width, 0, 1.6 * Math.PI);
                context.strokeStyle = parent.color;
                context.lineWidth = parent.circle_line_width;
                context.stroke();
            }
        }

        RotationAnimator {
            target: update_indicator
            from: 0
            to: 360
            duration: update_indicator.animation_duration
            running: update_indicator.visible
            loops: Animation.Infinite
        }
    }

    SFText {
        id: status_text
        anchors.top: parent.top
        anchors.left: parent.indicator.right
        anchors.leftMargin: 5
        anchors.topMargin: -1
        color: Style.content_secondary
        font.pixelSize: 12
    }
    SFText {
        id: progressText
        anchors.top: parent.top
        anchors.left: status_text.right
        anchors.leftMargin: 5
        anchors.topMargin: -1
        color: Style.content_secondary
        font.pixelSize: 12
        text: "(" + model.nodeSyncProgress + "%)"
        visible: model.nodeSyncProgress > 0 && update_indicator.visible
    }

    CustomProgressBar {
        id: progress_bar
        anchors.top: update_indicator.bottom
        anchors.left: update_indicator.left
        anchors.topMargin: 6
        backgroundImplicitWidth: 200
        contentItemImplicitWidth: 200

        visible: model.nodeSyncProgress > 0 && update_indicator.visible
        value: model.nodeSyncProgress / 100
    }

    LinkButton {
        //% "Change settings"
        text: qsTrId("status-change-settings")
        visible: model.isCoinClientFailed || model.isFailedStatus || (model.isOnline && !model.isConnectionTrusted)
        anchors.top: parent.top
        anchors.left: status_text.right
        anchors.leftMargin: 20
        anchors.topMargin: -1
        fontSize: 12
        onClicked: {
            if (model.isCoinClientFailed || model.isFailedStatus)
                main.openSwapSettings(model.coinWithErrorLabel());
            else
                main.openSwapSettings("BEAM");
        }
    }

    states: [
        State {
            name: "connecting"
            PropertyChanges {
                target: status_text;
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
                target: status_text;
                text: statusOnline + (model.isConnectionTrusted ? "" : ": " + statusOnlineRemote) + model.branchName + 
                    (
                        model.isExchangeRatesUpdated? "" : (!model.isConnectionTrusted ? "\n" : " ") + model.exchangeStatus
                    )
            }
            PropertyChanges {
                target: online_indicator;
                color: model.isExchangeRatesUpdated ? Style.online : Style.validator_warning
            }
            StateChangeScript {
                name: "onlineScript"
                script: {
                    rootControl.setIndicator(online_indicator);
                }
            }
        },
        State {
            name: "updating"
            PropertyChanges {
                target: status_text;
                //% "updating"
                text: qsTrId("status-updating") + "..." + model.branchName
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
                target: status_text;
                text: rootControl.error_msg + model.branchName
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
            name: "error_3rd"
            PropertyChanges {
                target: status_text;
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
