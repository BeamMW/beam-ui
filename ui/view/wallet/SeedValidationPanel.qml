import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
            Layout.fillWidth:    !showFaucetPromo
            horizontalAlignment: Text.AlignHCenter
            height: 32
            font.pixelSize:      14
            color:               Style.content_main
            //% "Write down and validate your seed phrase so you can always recover your funds."
            text:                qsTrId("seed-validation-promo")
            wrapMode:            Text.WordWrap
        }

        LinkButton {
            Layout.topMargin:    -22
            Layout.fillWidth:    !showFaucetPromo
            //% "Secure your phrase"
            text:                qsTrId("seed-validation-link")
            onClicked: {
                seedValidationHelper.isSeedValidatiomMode = true;
                seedValidationHelper.isTriggeredFromSettings = false;
                main.parent.setSource("qrc:/start.qml");
            }
        }

        Item {
            Layout.preferredHeight: 16
            Layout.fillWidth:       true
            Layout.minimumWidth: control.showFaucetPromo ? 0 : 25
        }

        CustomToolButton {
            Layout.topMargin:   -63
            Layout.rightMargin: -17
            visible: control.canHideValidationPromo
            icon.source: "qrc:/assets/icon-cancel-white.svg"
            onClicked: {
                control.onShowSeedValidationPromoOff()
            }
        }
    }
}
