import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "."

ConfirmationDialog {
    id:                       changeSettingsDialog
    okButtonColor:            Style.active
    okButtonText:             qsTrId("general-change-settings")
    okButtonIconSource:       "qrc:/assets/icon-settings-blue.svg"
    cancelButtonIconSource:   "qrc:/assets/icon-cancel-white.svg"
    closePolicy:              Popup.NoAutoClose
    defaultFocusItem:         okButton
    property alias text:      message.text

    contentItem: ColumnLayout {
        id: confirmationContent
        spacing: 30

        SFText {
            Layout.fillWidth:     true
            topPadding:           20
            font.pixelSize:       18
            color:                Style.content_main
            horizontalAlignment:  Text.AlignHCenter
            //% "Swap"
            text:                 qsTrId("general-swap")
        }

        SFText {
            id:                   message
            Layout.fillWidth:     true
            leftPadding:          20
            rightPadding:         20
            bottomPadding:        30
            font.pixelSize:       14
            color:                Style.content_main
            wrapMode:             Text.Wrap
            horizontalAlignment:  Text.AlignHCenter
        }
    }
}
