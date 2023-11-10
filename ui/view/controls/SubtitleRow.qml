import QtQuick 2.11
import QtQuick.Layouts 1.12
import "."

RowLayout {
    id: control
    spacing: 0

    property alias text: title.text
    property alias showBack: backBtn.visible
    property var onBack
    property var onRefresh: function () {}

    Layout.fillWidth:    true
    Layout.fillHeight:   false
    Layout.topMargin:    26
    Layout.bottomMargin: 30

    CustomButton {
        id: backBtn

        palette.button: "transparent"
        leftPadding:            0
        topPadding:             0
        bottomPadding:          0
        Layout.preferredHeight: title.height
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
        elide: Text.ElideRight
    }

    CustomToolButton {
        Layout.leftMargin:  3
        Layout.preferredHeight: title.height
        icon.source:        "qrc:/assets/icon-refresh.svg"
        visible:            main.devMode && control.text
        padding:            0
        onClicked:          control.onRefresh()
    }
}

