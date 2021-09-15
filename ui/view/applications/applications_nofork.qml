import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"

ColumnLayout {
    id: control
    Layout.fillWidth: true

    //
    // Page Header (Title + Status Bar)
    //
    Title {
        //% "DAPP Store"
        text: qsTrId("apps-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    SvgImage {
        Layout.topMargin: 80
        Layout.alignment: Qt.AlignHCenter
        width:   60
        height:  60
        opacity: 0.5
        source: "qrc:/assets/icon-applications.svg"
    }

    SFText {
        Layout.topMargin: 30
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize:   14
        color:            Style.content_main
        opacity:          0.5
        //% "DAPPs will be available in your wallet after the Hard Fork"
        text: qsTrId("apps-fork-warning")
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth:  true
    }
}
