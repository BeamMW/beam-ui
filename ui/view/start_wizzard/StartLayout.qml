import QtQuick
import QtQuick.Controls
import Beam.Wallet 1.0
import QtQuick.Layouts
import QtQuick.Window
import "../utils.js" as Utils
import "../controls"
import "."

Rectangle {
    id: root

    color: Style.background_main
    default property alias content:     contentLayout.data
    property alias showNetworkSelector: networkSelector.visible

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
        id:             rootColumn
        anchors.fill:   parent
        spacing:        0

        LogoComponent {
            Layout.alignment:       Qt.AlignHCenter
            Layout.fillHeight:      true
            Layout.preferredHeight: 1
        }

        ColumnLayout {
            id:                     contentLayout
            Layout.fillWidth:       true
            Layout.fillHeight:      true
            Layout.alignment:       Qt.AlignHCenter
            Layout.preferredHeight: 1
        }
    }

    VersionFooter {
        id:                     version
        anchors.right:          parent.right
        anchors.top:            parent.top
        anchors.rightMargin:    40
        anchors.topMargin:      40
    }

    CustomComboBox {
        id:                 networkSelector
        anchors.left:       parent.left
        anchors.top:        parent.top
        anchors.leftMargin: 40
        anchors.topMargin:  40
        fontPixelSize:      14
        enableScroll:       false
        textRole:           "name"

        model:              viewModel.networks
        currentIndex:       viewModel.currentNetworkIndex
        onActivated: {
            viewModel.currentNetwork = currentText;
            Theme.update();
        }
    }
}