import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Beam.Wallet      1.0
import "../controls"

CustomDialog {
    id:          control
    modal:       true
    x:           (parent.width - width) / 2
    y:           (parent.height - height) / 2
    parent:      Overlay.overlay
    width:       761
    closePolicy: Popup.NoAutoClose

    property bool newPublisher: true

    contentItem: ColumnLayout {
        spacing: 0

        // Title
        SFText {
            Layout.topMargin: 40
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize:   18
            font.weight:      Font.Bold
            color:            Style.content_main
            //% "The transaction is sent"
            text:             qsTrId("dapps-store-transacton-is-sent")
        }

        // Note
        SFText {
            Layout.topMargin: 30
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize:   14
            color:            Style.content_main
            text: control.newPublisher ?
                //% "Creation of the personal publisher account takes time.\n You can continue as soon as transaction is completed."
                qsTrId("dapps-store-creation-of-account")
                :
                //% "Changes take time. You can continue as soon as transaction is completed."
                qsTrId("dapps-store-changes-takes-time")
        }

        SvgImage {
            Layout.topMargin: 38
            Layout.bottomMargin: 30
            Layout.alignment: Qt.AlignHCenter
            source:           "qrc:/assets/icon-dapps-store-transaction-is-sent.svg"
            sourceSize:       Qt.size(82, 113)
        }
    }
}