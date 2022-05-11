import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "../controls"

Panel {
    id: control

    property bool canHideValidationPromo: true
    property real itemHeight: 75
    property var  onShowSeedValidationPromoOff: function() {}
    property bool showFaucetPromo: false

    width: parent.width
    height: control.itemHeight
    backgroundColor: control.canHideValidationPromo ?
        Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1) :
        Qt.rgba(Style.active.r, Style.active.g, Style.active.b, 0.2)

    content: RowLayout {
        SFText {
            Layout.topMargin:    -22
            Layout.fillWidth:    !(showFaucetPromo || control.assetsCount > 1)
            horizontalAlignment: Text.AlignHCenter
            height: 32
            font.pixelSize:      14
            color:               Style.content_main
            //% "Write down and validate your seed phrase so you can always recover your funds."
            text:                qsTrId("seed-validation-promo")
            wrapMode:            Text.WordWrap
        }

        SFText {
            Layout.topMargin:    -22
            Layout.fillWidth:    !(showFaucetPromo || control.assetsCount > 1)
            Layout.leftMargin:   10
            font.pixelSize:      14
            color:               Style.active
            //% "Secure your phrase"
            text:                qsTrId("seed-validation-link")
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    seedValidationHelper.isSeedValidatiomMode = true;
                    seedValidationHelper.isTriggeredFromSettings = false;
                    main.parent.setSource("qrc:/start.qml");
                }
                hoverEnabled: true
            }
        }

        Item {
            height: 16
            Layout.fillWidth: true
            Layout.minimumWidth: control.showFaucetPromo ? 0 : 25
        }

        Item {
            width:  16
            height: 16
            Layout.topMargin:   -55
            Layout.rightMargin: -9
            visible: control.canHideValidationPromo
            SvgImage {
                anchors.left: parent.left
                anchors.top: parent.top
                source: "qrc:/assets/icon-cancel-white.svg"
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.PointingHandCursor
                onClicked: function () {
                    control.onShowSeedValidationPromoOff()
                }
                hoverEnabled: true
            }
        }
    }
}
