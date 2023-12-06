import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import "."
import "../utils.js" as Utils

RowLayout {
    id: control
    default property alias  content:        contentLayout.data
    property alias          text:           title.text
    property var            path:           []
    property alias          canNavigate:    navigationBar.visible
    signal                  navigate(int item)

    spacing:             0
    Layout.fillHeight:   false

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
    RowLayout {
        Layout.alignment:   Qt.AlignVCenter
        Layout.leftMargin:  16
        spacing:            8
        RowLayout {
            id:                 navigationBar
            Layout.alignment:   Qt.AlignVCenter

            Repeater {
                Layout.alignment:          Qt.AlignVCenter
                model: path
                visible: path.length > 0

                RowLayout {
                    id:         navItem
                    spacing:    8
                    Item {
                        Layout.alignment:          Qt.AlignVCenter
                        width:  navText.width
                        height: navText.height
                        SFText {
                            id: navText
                            text: modelData
                            font.pixelSize:     24
                            color:              Style.content_secondary
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape:  Qt.PointingHandCursor 
                            onClicked: function () {
                                control.navigate(index)
                            }
                        }
                    }
                    SvgImage {
                        Layout.alignment:          Qt.AlignVCenter
                        Layout.preferredWidth:     24
                        Layout.preferredHeight:    24
                        source: "qrc:/assets/icon-chevron-right.svg"
                    }
                }
            }
        }

        SFText {
            Layout.alignment:   Qt.AlignVCenter
            id:                 title
            font.pixelSize:     30
            color:              Style.content_main
        }
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