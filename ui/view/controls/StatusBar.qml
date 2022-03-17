import QtQuick 2.11
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

Item {
    id: rootControl
    x: -20
    y: -30

    property var model

    function getStatus() {
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
    property string error_msg_3rd_client: model.coinClientErrorMsg || model.ipfsError
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
        visible: !onlineTrusted.visible || !model.isExchangeRatesUpdated
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
                width: rowBackground.width
                spacing: 7

                Item {
                    Layout.fillWidth: true
                }

                SFText {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.maximumWidth: parent.width - parent.spacing * 2 - 14
                                         - (progressText.visible ? progressText.width + parent.spacing : 0)
                                         - (settingsBtn.visible ? settingsBtn.width + parent.spacing : 0)
                    id: status_text
                    color: Style.content_main
                    font.pixelSize: 15
                    elide: Text.ElideRight
                }

                SFText {
                    id: progressText
                    color: Style.content_main
                    font.pixelSize: 15
                    text: "(" + model.nodeSyncProgress + "%)"
                    visible: model.nodeSyncProgress > 0 && update_indicator.visible
                }

                LinkButton {
                    id: settingsBtn
                    //% "Change settings"
                    text: qsTrId("status-change-settings")
                    visible: model.ipfsError || model.isCoinClientFailed || model.isFailedStatus || (model.isOnline && !model.isConnectionTrusted)
                    fontSize: 15

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

                Item {
                    Layout.fillWidth: true
                }
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
