import QtQuick 2.11
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

Item {
    id: rootControl
    x: -20
    y: -45

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
    property var indicatorX: -20
    property var indicatorY: 50

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
        x: rootControl.indicatorX
        y: rootControl.indicatorY
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
    
    Rectangle {
        id: rowBackground
        width: main.width - 70
        height: 24
        color: "transparent"
        visible: !onlineTrusted.visible
        property color gradientColor: online_indicator.color

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(rowBackground.width, 0)
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0.7) }
                GradientStop { position: 0.1; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0.7) }
                GradientStop { position: 1.0; color: Qt.rgba(rowBackground.gradientColor.r, rowBackground.gradientColor.g, rowBackground.gradientColor.b, 0) }
            }
        }

        ColumnLayout {
            spacing: 0
            RowLayout
            {
                id: statusRow
                Layout.topMargin: 3
                SFText {
                    id: status_text
                    color: Style.content_main
                    font.pixelSize: 16
                }
                SFText {
                    id: progressText
                    color: Style.content_main
                    font.pixelSize: 16
                    text: "(" + model.nodeSyncProgress + "%)"
                    visible: model.nodeSyncProgress > 0 && update_indicator.visible
                }

                LinkButton {
                    //% "Change settings"
                    text: qsTrId("status-change-settings")
                    visible: model.isCoinClientFailed || model.isFailedStatus || (model.isOnline && !model.isConnectionTrusted)
                    fontSize: 16
                    onClicked: {
                        if (model.isCoinClientFailed || model.isFailedStatus)
                            main.openSwapSettings(model.coinWithErrorLabel());
                        else
                            main.openSwapSettings("BEAM");
                    }
                }
            }

            CustomProgressBar {
                id: progress_bar
                backgroundImplicitWidth: 200
                contentItemImplicitWidth: 200

                visible: model.nodeSyncProgress > 0 && update_indicator.visible
                value: model.nodeSyncProgress / 100
            }
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
