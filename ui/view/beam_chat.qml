import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "controls"
import "./utils.js" as Utils

ColumnLayout {
    id: thisView

    property var onClosed: undefined
    property alias receiverAddr: chatModel.peerID
    property string name: ""
    property bool canSendMessage: thisView.receiverAddr.length != 0
    property var sendMessage: function() {
        if (message.text.length != 0)
        {
            chatModel.sendMessage(message.text);
            message.text = "";
        }
    }

    Timer {
        id: scrollDownTimer
        interval: 10
        repeat:   false
        running:  false

        onTriggered: {
            scrollView.scrollTo(Qt.Vertical, 1);
        }
    }

    MessengerChat {
        id: chatModel
        onMessagesChanged: {
            scrollDownTimer.start();
        }
        onEndChat: {
            onClosed();
        }
    }

    MessengerAddReceiver {
        id: receiverAddrDialog
        onClosed: {
            if (receiverAddrDialog.peerID.length == 0) {
                thisView.onClosed();
            }
            else
            {
                thisView.receiverAddr = receiverAddrDialog.peerID;
                thisView.name = receiverAddrDialog.name;
            }
        }
    }

    TopGradient {
        mainRoot: main
        topColor: Style.accent_outgoing
    }

    //
    // Title row
    //
    SubtitleRow {
        Layout.fillWidth:    true
        Layout.topMargin:    100

        //% "anonimus"
        readonly property string kanon: qsTrId("chat-title-anon")

        //% "Chat with %1"
        text: qsTrId("chat-title").arg(thisView.name.length != 0 ? thisView.name : (thisView.receiverAddr.length != 0 ? thisView.receiverAddr : kanon))
        onBack: function () {
            onClosed()
        }
    }

    RowLayout {
        Layout.topMargin:    10
        Layout.bottomMargin: 15
        Layout.fillWidth: true
        SFText {
            Layout.fillWidth: true
            text: thisView.receiverAddr
            font.pixelSize: 14
            color: Style.content_secondary
            wrapMode: Text.Wrap
        }

        SvgImage {
            Layout.rightMargin: 12
            Layout.alignment: Qt.AlignRight
            source: "qrc:/assets/icon-delete.svg"
            sourceSize: Qt.size(16, 16)

            MouseArea {
                z: 42
                anchors.fill:            parent
                hoverEnabled:            true
                propagateComposedEvents: true
                cursorShape:             Qt.PointingHandCursor
                onClicked: {
                    console.log('delete chat');
                    chatModel.removeChat();
                }
            }
        }
    }

    ScrollView {
        id:                  scrollView
        Layout.fillWidth:    true
        Layout.fillHeight:   true
        Layout.bottomMargin: 10
        clip:                true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy:   ScrollBar.AsNeeded

        property ScrollBar hScrollBar: ScrollBar.horizontal
        property ScrollBar vScrollBar: ScrollBar.vertical

        function scrollTo(type, ratio) {
            var scrollFunc = function (bar, ratio) {
                bar.setPosition(ratio - bar.size)
            }
            switch(type) {
            case Qt.Horizontal:
                scrollFunc(scrollView.hScrollBar, ratio)
                break;
            case Qt.Vertical:
                scrollFunc(scrollView.vScrollBar, ratio)
                break;
            }
        }

        Column {
            width: parent.width
            Repeater {
                id: chatViewRepeater
                width: parent.width
                model: chatModel.messages

                delegate: RowLayout {
                    width: scrollView.width

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: !is_income
                    }

                    ColumnLayout {
                        Layout.alignment: is_income ? Qt.AlignLeft : Qt.AlignRight
                        spacing: 0
                        Layout.margins: 4

                        SFText {
                            Layout.margins: 7
                            text: time
                            font.pixelSize: 12
                            color: Style.content_secondary
                        }
                        SFText {
                            Layout.margins: 7
                            Layout.topMargin: 0
                            Layout.maximumWidth: scrollView.width - 70
                            text: message
                            wrapMode: Text.WrapAnywhere
                            font.pixelSize: 16
                            color: Style.content_main
                        }

                        Rectangle {
                            width: parent.width
                            height: parent.height
                            radius:       10
                            color:        is_income ? Style.accent_incoming : Style.accent_outgoing
                            opacity:      0.5
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: is_income
                    }
                }
            }
        }
    }

    RowLayout {
        spacing:             20
        Layout.topMargin:    30
        Layout.bottomMargin: 40

        ScrollView {
            clip: true
            Layout.fillWidth: true
            Layout.maximumHeight: 100
            SFTextArea {
                id: message
                Layout.fillWidth: true
                focus: true
                activeFocusOnTab: true
                font.pixelSize: 14
                wrapMode: TextInput.Wrap
                color: Style.content_main
                backgroundColor: Style.content_main
                placeholderText:  Qt.platform.os == "osx" ?
                    //% "[command + enter] to send"
                    qsTrId("chat-message-placeholder-osx")
                    //% "[ctrl + enter] to send"
                    : qsTrId("chat-message-placeholder")
                enabled: sendButton.enabled

                Keys.onPressed: (event)=>
                {
                    if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
                    {
                        if (event.modifiers & Qt.ControlModifier)
                        {
                            thisView.sendMessage();
                            return false;
                        }
                    }
                }
            }

        }

        CustomButton {
            id: sendButton
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            height: 32
            palette.button: Style.accent_outgoing
            palette.buttonText: Style.content_opposite
            icon.source: "qrc:/assets/icon-send-blue.svg"
            //% "Send"
            text: qsTrId("general-send")
            font.pixelSize: 12
            onClicked: {
                thisView.sendMessage();
            }
            enabled: chatModel.canSend
        }
    }

    Component.onCompleted: {
        if (thisView.receiverAddr.length == 0) {
            receiverAddrDialog.open()
        }
    }
}
