import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import "."
import "../utils.js" as Utils


RowLayout {
    default property alias content: contentLayout.data
    property alias text: title.text

    spacing:             0
    Layout.fillHeight:   false

    implicitWidth:       title.implicitWidth + buttons.implicitWidth
    implicitHeight:      title.implicitHeight + buttons.implicitHeight

    Rectangle {
        Layout.preferredWidth:  56
        Layout.preferredHeight: 56
        color:                  Utils.alpha(Style.background_main, 0.5)
        radius:                 28
        SvgImage {
            id: image
            anchors.centerIn:   parent
            source:             Style.navigation_logo
        }
    }

    SFText {
        id:                 title
        Layout.leftMargin:  16
        font.pixelSize:     36
        color:              Style.content_main
    }
    RowLayout {
        id:                 contentLayout
        Layout.fillWidth:   true
        Layout.alignment:   Qt.AlignVCenter
    }
    RowLayout {
        id:                 buttons
        Layout.fillWidth:   false
        Layout.fillHeight:  false
        Layout.leftMargin:  16
        spacing:            8
        Rectangle {
            Layout.preferredWidth:  32
            Layout.preferredHeight: 32
            color:                  Utils.alpha(Style.background_main, 0.5)
            radius:                 16

            CustomButton {
                id:                     notificationsButton
                width:                  32
                height:                 32
                display:                AbstractButton.IconOnly
                leftPadding:            6
                rightPadding:           6
                icon.source:            "qrc:/assets/icon-notifications.svg"
                icon.width:             20
                icon.height:            20
                icon.color:             "transparent"
                palette.button:         "transparent"//Utils.alpha(Style.background_main, 0.5)
                shadowColor:            Style.content_main
                onClicked: {
                    main.openNotifications()
                }
            }

            Item {
                x: 20
                y: 4
                width: 6
                height: 6
                visible: main.unreadNotifications > 0
                Rectangle {
                    id: counter
                    width: 6
                    height: 6
                    radius: width/2
                    color: Style.active
                    //SFText {
                    //    text: main.unreadNotifications
                    //    font.pixelSize: 6
                    //    anchors.centerIn: counter
                    //}
                }
                DropShadow {
                    anchors.fill: counter
                    radius: 5
                    samples: 9
                    source: counter
                    color: Style.active
                }
            }
        }
        LogoutButton{
            id: logoutButton
        }
    }
}