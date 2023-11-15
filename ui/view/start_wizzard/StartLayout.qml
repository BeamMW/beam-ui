import QtQuick 2.12
import QtQuick.Controls 2.4
import Beam.Wallet 1.0
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import "../utils.js" as Utils
import "../controls"
import "."

Rectangle
{
    id: root

    readonly property bool isSqueezedHeight : Utils.isSqueezedHeight(root.height)

    color: Style.background_main
    default property alias content: contentLayout.data

    Image {
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent

        source: {
            if (Screen.devicePixelRatio  > 2)
                "qrc:/assets/bg-pic@3x.png"
            else if (Screen.devicePixelRatio  > 1)
                "qrc:/assets/bg-pic@2x.png"
            else
                "qrc:/assets/bg-pic.png"
        }
    }

    BgLogo {
    }

    ColumnLayout {
        id: rootColumn
        anchors.fill: parent
        spacing: 0

        LogoComponent {
            Layout.topMargin:   root.isSqueezedHeight ? 13 : 83
            Layout.alignment:   Qt.AlignHCenter
            isSqueezedHeight:   root.isSqueezedHeight
        }

        ColumnLayout {
            id:                 contentLayout
            Layout.fillWidth:   true
            Layout.fillHeight:  true
            Layout.alignment:   Qt.AlignHCenter
        }
    }

    VersionFooter {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 40
    }
}