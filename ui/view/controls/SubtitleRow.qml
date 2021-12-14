import QtQuick 2.11
import QtQuick.Layouts 1.12
import "."

Item {
    id: control

    property alias text: title.text
    property alias showBack: backBtn.visible
    property var onBack
    property var onRefresh: function () {}

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
        visible:      true

        onClicked: control.onBack()
    }

    RowLayout {
        spacing: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        SFText {
            id: title
            color: Style.content_main

            font {
                styleName:      "DemiBold"
                weight:         Font.DemiBold
                pixelSize:      14
                letterSpacing:  4
                capitalization: Font.AllUppercase
            }
        }

        SvgImage {
            Layout.leftMargin: 3
            source: "qrc:/assets/icon-refresh.svg"
            sourceSize: Qt.size(13, 13)
            width: 13
            height: 13
            visible: main.devMode && control.text

            MouseArea {
                anchors.fill:    parent
                acceptedButtons: Qt.LeftButton
                cursorShape:     Qt.PointingHandCursor
                onClicked:       control.onRefresh()
            }
        }
    }
}
