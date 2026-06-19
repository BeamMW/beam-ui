import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Beam.Wallet 1.0
import "controls"
import "utils.js" as Utils

ColumnLayout {
    id: messengerRoot
    spacing:           0

    property var stackView : StackView.view

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

    //% "Beam Messenger"
    property string title:  qsTrId("messenger-title")
    property var titleContent: RowLayout {
        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }
        RowLayout {
            spacing:          20

            PrimaryButton {
                id:                     newChatButton
                Layout.preferredHeight: 32
                                //% "New chat"
                text:         qsTrId("messenger-new-chat")
                icon.source:  "qrc:/assets/icon-messenger-new-chat.svg"
                font.pixelSize: 12
                onClicked:    {
                    messengerRoot.openChat();
                }
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

        ScrollBar.vertical: CustomScrollBar {}

        delegate: Item {
            implicitHeight: 72
            implicitWidth:  chatsList.width - 10
            Rectangle {
                anchors.fill: parent
                radius:       10
                color:        Style.active
                opacity:      hoverArea.containsMouse ? 0.15 : 0.1
            }

            // Accent strip on the left edge, clipped to the pill's rounded
            // shape so its ends follow the corner curve instead of overhanging.
            Item {
                anchors.fill: parent
                visible:      haveUnread

                Item {
                    id: accentSource
                    anchors.fill: parent
                    visible:      false
                    Rectangle {
                        anchors.left:   parent.left
                        anchors.top:    parent.top
                        anchors.bottom: parent.bottom
                        width:          5
                        color:          Style.active
                    }
                }

                Rectangle {
                    id: pillMask
                    anchors.fill: parent
                    radius:       10
                    visible:      false
                }

                OpacityMask {
                    anchors.fill: parent
                    source:       accentSource
                    maskSource:   pillMask
                }
            }

            ColumnLayout {
                anchors.fill:     parent
                anchors.leftMargin: 25
                anchors.topMargin:  15
                spacing: 7

                SFText {
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
                    text: cid
                    font.pixelSize: 14
                    color: Style.content_secondary
                    wrapMode: Text.Wrap
                }

                Item { Layout.fillHeight: true }
            }

            MouseArea {
                id:                      hoverArea
                anchors.fill:            parent
                hoverEnabled:            true
                propagateComposedEvents: true
                cursorShape:             Qt.PointingHandCursor
                onClicked: {
                    messengerRoot.openChat(cid, name);
                }
            }
        }
    }
}
