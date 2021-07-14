import QtQuick 2.11
import "."

Item {
    id: control

    property alias text: title.text
    property var onBack

    CustomButton {
        id: backBtn

        anchors.verticalCenter: parent.verticalCenter
        anchors.left:   parent.left

        palette.button: "transparent"
        leftPadding:    0
        showHandCursor: true

        font {
            styleName: "DemiBold"
            weight:    Font.DemiBold
        }

        //% "Back"
        text:         qsTrId("general-back")
        icon.source:  "qrc:/assets/icon-back.svg"
        visible:      !!control.text

        onClicked: control.onBack()
    }

    SFText {
        id: title
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        color: Style.content_main

        font {
            styleName:      "DemiBold"
            weight:         Font.DemiBold
            pixelSize:      14
            letterSpacing:  4
            capitalization: Font.AllUppercase
        }
    }
}
