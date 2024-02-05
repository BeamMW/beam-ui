import QtQuick 2.12
import Beam.Wallet 1.0
import QtQuick.Layouts 1.15
import "."

SFText {
    Layout.alignment:    Qt.AlignHCenter
    Layout.bottomMargin: 27
    font.pixelSize:      12
    color:               Qt.rgba(1, 1, 1, 0.3)
    text:                qsTrId("settings-version") + BeamGlobals.version()
    onClicked: function() {
        BeamGlobals.copyToClipboard(qsTrId("settings-version") + BeamGlobals.version())
    }
}