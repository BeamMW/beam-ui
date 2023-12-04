import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "."

Item {
    implicitWidth:  logoutButton.implicitWidth
    implicitHeight: logoutButton.implicitHeight
    ContextMenu {
        id: accountContextMenu
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
            text:           qsTrId("messenger-title")
            icon.source:    "qrc:/assets/icon-beam_messenger.svg"
            onTriggered: {
                main.openMessenger();
            }
        }

        Action {
            text:           qsTrId("addresses-tittle")
            icon.source:    "qrc:/assets/icon-addresses.svg"
            onTriggered: {
                main.openAddresses();
            }
        }
        Action {
            text:           qsTrId("settings-title")
            icon.source:    "qrc:/assets/icon-settings.svg"
            onTriggered: {
                main.openSettings();
            }
        }
        Action {
            //% "Documentation"
            text:           qsTrId("help-title")
            icon.source:    "qrc:/assets/icon-help.svg"
            onTriggered: {
                main.openHelp();
            }
        }
        Action {
            //% "Logout"
            text: qsTrId("status-logout")
            icon.source: "qrc:/assets/icon-log-out.svg"
            onTriggered: {
                main.parent.setSource("qrc:/start.qml", {"isLogoutMode": true});
            }
        }
        MenuSeparator{
            leftPadding:            8
            rightPadding:           8
        }
        VersionFooter {
            horizontalAlignment:    Text.AlignRight
            rightPadding:           20
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
            accountContextMenu.popup(logoutButton, Qt.point(20, logoutButton.height + 6))
        }
    }
}