import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import "."

BaseNotification {
    id: control

    property alias  comment: commentCtrl.text
    property alias  appname: titleCtrl.text
    property string appicon
    property string txid

    width:         360
    lifetime:      6000
    topPadding:    5//10
    bottomPadding: 5//10
    rightPadding:  5//10
    leftPadding:   20

    contentItem: RowLayout {
        spacing: 0

        Rectangle {
            Layout.alignment: Qt.AlignVCenter
            width:  60
            height: 60
            radius: 30
            color:  Style.background_main

            SvgImage {
                id: defaultIcon
                source: "qrc:/assets/icon-defapp.svg"
                sourceSize: Qt.size(30, 30)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !customIcon.visible
            }

            SvgImage {
                id: customIcon
                source: control.appicon || "qrc:/assets/icon-defapp.svg"
                sourceSize: Qt.size(30, 30)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !!control.appicon && progress == 1.0
            }
        }

        ColumnLayout {
            Layout.leftMargin:   20
            Layout.topMargin:    15
            Layout.bottomMargin: 15
            Layout.fillWidth:    true
            spacing: 5

            SFText {
                Layout.fillWidth:  true
                id: commentCtrl
                color: Style.content_main
                elide: Text.ElideRight
                font {
                    pixelSize: 15
                    styleName: "DemiBold"
                    weight:    Font.DemiBold
                }
            }

            SFText {
                Layout.fillWidth:  true
                id: titleCtrl
                color: Style.content_secondary
                font.pixelSize: 14
                elide: Text.ElideRight
            }

            SFText {
                //% "Show details"
                text: [Style.linkStyle, "<a href='#'>", qsTrId("general-show-tx-details"), "</a>"].join("")
                linkEnabled: true
                textFormat: Text.RichText
                onLinkActivated: function () {
                    main.openDAppTransactionDetails(control.txid)
                    control.close()
                }
            }
        }

        CustomToolButton {
            Layout.leftMargin: 10
            Layout.alignment: Qt.AlignTop
            icon.source: "qrc:/assets/icon-cancel-white.svg"
            onClicked: function () {
               control.close()
            }
        }
    }
}
