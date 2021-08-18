import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import "."

BaseNotification {
    id: popup

    property alias title: title.text
    property alias message: contentText.text
    property alias acceptButtonText: acceptButton.text
    property var onCancel: function () {}
    property var onAccept: function () {}

    width:  335
    height: 198

    contentItem: Item {
        CustomToolButton {
            x: 275
            y: -10
            icon.source: "qrc:/assets/icon-cancel-white.svg"
            onClicked: onCancel()
        }

        Column {
            SFText {
                id: title
                width: 255
                leftPadding: 10
                visible: text.length > 0;
                font.pixelSize: 18
                font.styleName: "Bold";
                font.weight: Font.Bold
                color: Style.content_main
                horizontalAlignment : Text.AlignLeft
                wrapMode: Text.Wrap
            }

            
            SFText {
                id: contentText
                width: 275
                topPadding: 10
                leftPadding: 10
                
                font.pixelSize: 14
                color: Style.content_main
                horizontalAlignment : Text.AlignLeft
                wrapMode: Text.Wrap
            }
        }
        
        CustomButton {
            id: acceptButton
            x: 0
            y: 120
            // width: 165
            height: 38
            palette.button: Style.background_second
            palette.buttonText : Style.content_main
            icon.source: "qrc:/assets/icon-repeat-white.svg"
            onClicked: onAccept()
        }
    }
}
