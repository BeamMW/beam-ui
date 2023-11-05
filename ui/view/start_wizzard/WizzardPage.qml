import QtQuick 2.12
import QtQuick.Controls 2.4
import Beam.Wallet 1.0
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import "../utils.js" as Utils
import "../controls"

Rectangle {
    color: Style.background_main
    default property alias content: contentLayout.data
    property alias buttons: buttonsRow.data

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        anchors.topMargin: 50

        ColumnLayout {
            id: contentLayout
            Layout.fillWidth:   true
            Layout.alignment:   Qt.AlignHCenter
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
            Layout.minimumHeight: 50
        }

        Row {
            id: buttonsRow
            Layout.alignment: Qt.AlignHCenter
            spacing: 30
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
            Layout.minimumHeight: 67
            Layout.maximumHeight: 143
        }

        VersionFooter {}
    }
}