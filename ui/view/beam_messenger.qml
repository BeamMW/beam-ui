import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "controls"
import "utils.js" as Utils

Item {
    id: assetsSwapRoot
    Layout.fillWidth: true
    Layout.fillHeight: true

    Title {
        //% "Beam Messenger"
        text: qsTrId("messenger-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
        z: 33
    }

    MessengerChatList {
        id: chatList
    }

    property var openChat : function(cid, name) {
        var chatId = "";
        if (cid && cid.length) {
            chatId = cid;
        }
        var chatName = "";
        if (name && name.length) {
            chatName = name;
        }
        chatList.blockSignals();
        stackView.push(Qt.createComponent("beam_chat.qml"), {
            "onClosed": function() {
                    stackView.pop();
                    chatList.unblockSignals();
                },
            "receiverAddr": chatId,
            "name": chatName
            });
    }

    Component {
        id: chatListLayout

        ColumnLayout {
            Layout.fillWidth:  true
            Layout.fillHeight: true
            spacing:           0

            RowLayout {
                spacing:          20
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.topMargin: 50

                PrimaryButton {
                    id:           newChatButton
                                  //% "New chat"
                    text:         qsTrId("messenger-new-chat")
                    icon.source:  "qrc:/assets/icon-messenger-new-chat.svg"
                    allLowercase: false
                    onClicked:    {
                        assetsSwapRoot.openChat();
                    }
                }
            }

            ListView {
                id: chatsList
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: 30
                Layout.bottomMargin: 15
                spacing: 10

                model: chatList.chats
                clip: true

                ScrollBar.vertical: ScrollBar {}

                delegate: Item {
                    implicitHeight: 72
                    implicitWidth:  chatsList.width - 10
                    Rectangle {
                        anchors.fill: parent
                        radius:       10
                        color:        Style.active
                        opacity:      hoverArea.containsMouse ? 0.15 : 0.1
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 7

                        SFText {
                            Layout.topMargin: 15
                            Layout.leftMargin: 25
                            text: name
                            font {
                                styleName:  "DemiBold"
                                weight:     Font.DemiBold
                                pixelSize:  16
                            }
                            color: Style.content_main
                            wrapMode: Text.Wrap
                        }

                        SFText {
                            Layout.leftMargin: 25
                            text: cid
                            font.pixelSize: 14
                            color: Style.content_secondary
                            wrapMode: Text.Wrap
                        }
                    }

                    MouseArea {
                        id:                      hoverArea
                        anchors.fill:            parent
                        hoverEnabled:            true
                        propagateComposedEvents: true
                        cursorShape:             Qt.PointingHandCursor
                        onClicked: {
                            assetsSwapRoot.openChat(cid, name);
                        }
                    }
                }
            }
        }
    }

    StackView {
        id:           stackView
        anchors.fill: parent
        initialItem:  chatListLayout

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
                stackView.currentItem.defaultFocusItem.forceActiveFocus();
            }
        }
    }
}
