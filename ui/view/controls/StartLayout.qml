import QtQuick 2.12
import QtQuick.Controls 2.4
import Beam.Wallet 1.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import "../utils.js" as Utils
import "."

Rectangle
{
    id: root

    function getLogoTopGapSize(parentHeight) {
        return parentHeight * (Utils.isSqueezedHeight(parentHeight) ? 0.05 : 0.10)
    }

    color: Style.background_main
    default property alias content: contentLayout.children

    Image {
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent

        source: {
            console.log("Screen.devicePixelRatio: " + Screen.devicePixelRatio)
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

        Item {
            Layout.preferredHeight: root.getLogoTopGapSize(rootColumn.height)
        }

        LogoComponent {
            Layout.alignment: Qt.AlignHCenter
            isSqueezedHeight: Utils.isSqueezedHeight(rootColumn.height)
        }

        ColumnLayout {
            id:                 contentLayout
            Layout.fillWidth:   true
            Layout.fillHeight:  true
            Layout.alignment:   Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: Utils.isSqueezedHeight(rootColumn.height) ? 15 : 40
        }

        SFText {
            Layout.alignment:    Qt.AlignHCenter
            font.pixelSize:      12
            color:               Qt.rgba(255, 255, 255, 0.3)
            text:                [qsTrId("settings-version"), BeamGlobals.version()].join(' ')
        }

        Item {
            Layout.minimumHeight: 35
        }
    }
}