import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtWebEngine
import QtWebChannel
import Beam.Wallet      1.0
import "../controls"

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27

    //
    // Page Header (Title + Status Bar)
    //

    //% "DApp Store"
    property string title: qsTrId("apps-title-no-fork")

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
