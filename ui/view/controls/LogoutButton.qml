import QtQuick 2.11
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

Item {
    implicitWidth:  logoutButton.implicitWidth
    implicitHeight: logoutButton.implicitHeight
    ContextMenu {
        id: accountContextMenu
        VersionFooter {
            horizontalAlignment:    Text.AlignRight
            rightPadding:           20
        }
        MenuSeparator{
            leftPadding:            8
            rightPadding:           8
        }
        SFLabel {
            leftPadding:            20
            rightPadding:           20
            font.pixelSize:         15
            elide:                  Text.ElideMiddle
            text:                   main.accountLabel
            color:                  Style.content_main
            font.styleName: "Bold"; font.weight: Font.Bold
        }
        MenuSeparator{
            leftPadding:            8
            rightPadding:           8
        }
        Action {
            text:           qsTrId("settings-title")
            icon.source:    "qrc:/assets/icon-settings.svg"
            onTriggered: {
                main.openSettings();
            }
        }
        Action {
            //% "Logout"
            text: qsTrId("status-logout")
            icon.source: "qrc:/assets/icon-back.svg"
            onTriggered: {
                main.parent.setSource("qrc:/start.qml", {"isLogoutMode": true});
            }
        }
    }
    CustomToolButton {
        id:                     logoutButton
        Layout.rightMargin:     20
        leftPadding:            4
        rightPadding:           0
        verticalPadding:        0
        icon.source:            main.accountPicture
        icon.width:             32
        icon.height:            32
        icon.color:             "transparent"
        onClicked: {
            accountContextMenu.popup(logoutButton, Qt.point(20, logoutButton.height + 7))
        }
    }
}