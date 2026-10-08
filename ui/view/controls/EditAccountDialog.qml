import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Beam.Wallet 1.0
import "."

CustomDialog {
    id: control

    width:   460
    parent:  Overlay.overlay
    x:       Math.round((parent.width - width) / 2)
    y:       Math.round((parent.height - height) / 2)
    modal:   true

    readonly property string newLabel: accountLabel.text.trim()
    readonly property bool labelExists: newLabel.length > 0 && newLabel != main.accountLabel && main.isAccountLabelUsed(newLabel)

    onOpened: {
        accountLabel.text = main.accountLabel
        pictures.currentIndex = main.accountPictureIndex
        accountLabel.forceActiveFocus()
    }

    contentItem: ColumnLayout {
        spacing: 0

        SFText {
            Layout.fillWidth:    true
            Layout.topMargin:    30
            horizontalAlignment: Text.AlignHCenter
            color:               Style.content_main
            font.pixelSize:      18
            font.weight:         Font.Bold
            //% "Edit account"
            text:                qsTrId("account-edit-title")
        }

        SFText {
            Layout.topMargin:   30
            Layout.leftMargin:  40
            Layout.rightMargin: 40
            text:               qsTrId("start-new-account-label")
            color:              Style.content_main
            font.pixelSize:     14
            font.styleName:     "Bold"; font.weight: Font.Bold
        }

        SFTextInput {
            id:                 accountLabel
            Layout.fillWidth:   true
            Layout.topMargin:   10
            Layout.leftMargin:  40
            Layout.rightMargin: 40
            font.pixelSize:     14
            color:              Style.content_main
        }

        SFText {
            Layout.leftMargin:  40
            Layout.rightMargin: 40
            visible:            control.labelExists
            text:               qsTrId("account-label-exists-error")
            color:              Style.validator_error
            font.pixelSize:     14
        }

        AccountPictureGrid {
            id:               pictures
            Layout.topMargin: 25
            Layout.alignment: Qt.AlignHCenter
            itemSize:         72
        }

        RowLayout {
            Layout.topMargin:    30
            Layout.bottomMargin: 30
            Layout.alignment:    Qt.AlignHCenter
            spacing:             20

            CustomButton {
                text:        qsTrId("general-cancel")
                icon.source: "qrc:/assets/icon-cancel-white.svg"
                onClicked:   control.close()
            }

            PrimaryButton {
                text:        qsTrId("edit-addr-save-button")
                icon.source: "qrc:/assets/icon-done.svg"
                enabled:     control.newLabel.length > 0 && !control.labelExists
                onClicked: {
                    main.updateAccount(control.newLabel, pictures.currentIndex)
                    control.close()
                }
            }
        }
    }
}
