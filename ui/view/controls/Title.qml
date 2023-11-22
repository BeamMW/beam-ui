import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 1
import QtGraphicalEffects 1
import "."
import "../utils.js" as Utils


RowLayout {
    default property alias content: contentLayout.data
    property alias text: title.text

    spacing:             0
    Layout.fillHeight:   false

    implicitWidth:       title.implicitWidth + buttons.implicitWidth
    implicitHeight:      title.implicitHeight + buttons.implicitHeight

    SFText {
        id:                 title
        Layout.leftMargin:  10
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
        Item {
            Layout.preferredWidth:  32
            Layout.preferredHeight: 32
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
                palette.button:         Style.background_main//Utils.alpha(Style.background_main, 0.5)
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