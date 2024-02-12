import QtQuick 2.12
import Beam.Wallet 1.0
import QtQuick.Layouts 1.15
import "."

SFText {
    id: versionText
    Layout.alignment:    Qt.AlignHCenter
    Layout.bottomMargin: 27
    font.pixelSize:      12
    color:               Qt.rgba(1, 1, 1, 0.3)
    text:                qsTrId("settings-version") + BeamGlobals.version()

    MouseArea {
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton
    cursorShape: Qt.PointingHandCursor
    onClicked: function() {
        BeamGlobals.copyToClipboard(versionText.text)
    }
}
}
