import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import "../controls"

Item {
    id: control
    Layout.preferredWidth: openInExplorer.width + 10 + openInExplorerIcon.width
    height: 16
    // width: openInExplorer.width + 10 + openInExplorerIcon.width
    property var onTriggered: function() {
        console.log("triggered");
    }

    SFText {
        id: openInExplorer
        font.pixelSize: 14
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.rightMargin: 10
        color: Style.active
        //% "Open in Blockchain Explorer"
        text: qsTrId("open-in-explorer")
    }
    SvgImage {
        id: openInExplorerIcon
        anchors.top: parent.top
        anchors.right: parent.right
        source: "qrc:/assets/icon-external-link-green.svg"
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            onTriggered();
        }
        hoverEnabled: true
    }
}
