import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import Beam.Wallet 1.0
import "."

BaseNotification {
    id: control

    property alias  comment: commentCtrl.text
    property alias  appname: titleCtrl.text
    property string appicon
    property string txid
    property bool   isLinkToWalletMainTxTable: false
    property int    secondsFromBirth:          1
    property double progress:                  0.0

    Timer {
        id: closeTimer
        interval: 3000
        repeat: false
        running: false
        onTriggered: {
            control.close();
        }
    }

    AppNotificationHelper {
        id: notificationHelper
        txId: control.txid
        onTxFinished: {
            control.progress = 1.0;
            closeTimer.start();
        }
    }

    width:         360
    lifetime:      0//6000
    padding:       0

    contentItem: ColumnLayout{
        spacing: 0
        RowLayout {
            Layout.fillWidth:    true
            Layout.topMargin: 5
            Layout.rightMargin: 5
            Layout.leftMargin: 5
            Layout.bottomMargin: -14
            spacing: 0

            Item {
                width: 15
                height: 1
            }

            Rectangle {
                Layout.alignment: Qt.AlignVCenter
                width:  60
                height: 60
                radius: 30
                color:  Style.background_main

                SvgImage {
                    id: defaultIcon
                    source: "qrc:/assets/icon-defapp.svg"
                    sourceSize: Qt.size(30, 30)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: !customIcon.visible
                }

                SvgImage {
                    id: customIcon
                    source: control.appicon || "qrc:/assets/icon-defapp.svg"
                    sourceSize: Qt.size(30, 30)
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: !!control.appicon && progress == 1.0
                }
            }

            ColumnLayout {
                Layout.leftMargin:   20
                Layout.topMargin:    15
                Layout.bottomMargin: 15
                Layout.fillWidth:    true
                spacing: 5

                SFText {
                    Layout.fillWidth:  true
                    id: commentCtrl
                    color: Style.content_main
                    elide: Text.ElideRight
                    font {
                        pixelSize: 15
                        styleName: "DemiBold"
                        weight:    Font.DemiBold
                    }
                }

                SFText {
                    Layout.fillWidth:  true
                    id: titleCtrl
                    color: Style.content_secondary
                    font.pixelSize: 14
                    elide: Text.ElideRight
                }

                SFText {
                    //% "Show details"
                    text: [Style.linkStyle, "<a href='#'>", qsTrId("general-show-tx-details"), "</a>"].join("")
                    linkEnabled: true
                    textFormat: Text.RichText
                    onLinkActivated: function () {
                        if (isLinkToWalletMainTxTable) {
                            main.openTransactionDetails(control.txid)
                        } else {
                            main.openDAppTransactionDetails(control.txid)
                        }
                    }
                }
            }

            CustomToolButton {
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignTop
                icon.source: "qrc:/assets/icon-cancel-white.svg"
                onClicked: function () {
                    control.close()
                }
            }
        }

        Timer {
            interval: 1000
            repeat:   true
            running:  true

            onTriggered: {
                var pr = control.secondsFromBirth / notificationHelper.estimateBlockTime;
                if (control.progress < 1.0)
                    control.progress = pr > 0.97 ? 0.97 : pr;
                control.secondsFromBirth++;
            }
        }

        Rectangle {
            id: rect
            Layout.alignment: Qt.AlignBottom | Qt.AlignVCenter
            Layout.fillWidth: true
            height: 20
            radius: 10
            color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1)
            z: 101
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Rectangle {
                    anchors.centerIn: parent
                    width: rect.width
                    height: rect.height
                    radius: 10
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                height: 6
                width: parent.width * control.progress
                radius: 3
                color: Style.active
                z: 102
            }
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 14
                color: Style.background_popup
                z: 103
            }
        }
    }

    onClosed: {
        main.closeContractNotification(txid);
    }
}
