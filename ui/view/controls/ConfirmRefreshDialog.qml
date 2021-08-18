import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: control
    property bool canRefresh: true

    property var settingsViewModel: function() {
		return {
			refreshWallet: function() {
				console.log("settingsViewModel::refreshWallet undefined")
				return false
			}
		}
	}

    //% "Rescan"
    title: qsTrId("general-rescan")
    //% "Rescan"
    okButtonText: qsTrId("general-rescan")
    okButtonIconSource: "qrc:/assets/icon-repeat.svg"
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonVisible: true
    width:  460
    height: 243

    contentItem: Item {
        id: confirmationContent
        Column {
            anchors.fill: parent
            spacing: 30

            SFText {
                width: parent.width
                leftPadding: 20
                rightPadding: 20
                font.pixelSize: 14
                color: Style.content_main
                wrapMode: Text.Wrap
                horizontalAlignment : Text.AlignHCenter
                //: settings tab, confirm rescan dialog message
                //% "Rescan will sync transaction and UTXO data with the latest information on the blockchain. The process might take long time."
                text: qsTrId("settings-rescan-confirmation-message")
            }
            SFText {
                width: parent.width
                leftPadding: 20
                rightPadding: 20
                topPadding: -15
                font.pixelSize: 14
                color: Style.content_main
                wrapMode: Text.Wrap
                horizontalAlignment : Text.AlignHCenter
                //: settings tab, confirm rescan dialog additional message
                //% "Are you sure?"
                text: qsTrId("settings-rescan-confirmation-message-line-2")
            }
        }
    }

    onAccepted: {
        canRefresh = false;
        settingsViewModel.refreshWallet()
    }
}
