import QtQuick 2.15
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.15
import "."
import "../utils.js" as Utils

ConfirmationDialog {
    width: 440
    leftPadding: 50
    rightPadding: 50

    okButtonText: qsTrId("general-proceed")
    okButtonIconSource: "qrc:/assets/icon-done.svg"
    cancelButtonVisible: true
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"

    //% "DApps Privacy Warning"
    title: qsTrId("dapps-privacy-title")
    /*% "Beam Wallet requires a permission to launch distributed applications. This action may expose your info including but not limited to the IP address to 3rd-party web servers and/or contracts.

Allow to proceed? You can always update your choice in Settings later."*/
    text: qsTrId("dapps-privacy-warning")
}
