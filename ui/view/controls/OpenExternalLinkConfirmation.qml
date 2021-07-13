import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."
import "../utils.js" as Utils

ConfirmationDialog {
    property string externalUrl
    property var onOkClicked: function () {}
    property var onCancelClicked: function () {}
    //% "proceed"
    okButtonText: qsTrId("open-external-open")
    okButtonIconSource: "qrc:/assets/icon-external-link-black.svg"
    cancelButtonVisible: true
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    width: 460
    leftPadding: 50
    rightPadding: 50

    //% "Open external link"
    title: qsTrId("open-external-title")

    /*% "Beam Wallet app requires permission to open external link in the browser. This action will expose your IP to the web server. To avoid it, choose \"Cancel\". You can chage your choice in app setting anytime."*/
    text: qsTrId("open-external-message")

    onAccepted: {
        onOkClicked();
        Utils.openUrl(externalUrl);
    }

    onRejected: {
        onCancelClicked();
    }
}