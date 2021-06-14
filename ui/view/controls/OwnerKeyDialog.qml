import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

ConfirmationDialog {
    id: thisDialog
    property string pwd: ""

    property var settingsViewModel: function() {
		return {
			getOwnerKey: function() {
				console.log("settingsViewModel::getOwnerKey undefined")
				return false
			}
		}
	}

    //: settings tab, show owner key dialog title
    //% "Owner key"
    title: qsTrId("settings-show-owner-key-title")
    okButtonText: qsTrId("general-copy")
    okButtonIconSource: "qrc:/assets/icon-copy-blue.svg"
    cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    cancelButtonText: qsTrId("general-close")
    cancelButtonVisible: true
    width: 460

    contentItem: Item {
        ColumnLayout {
            anchors.fill: parent
            spacing: 20
            SFLabel {
                id: ownerKeyValue
                Layout.fillWidth: true
                leftPadding: 20
                rightPadding: 20
                topPadding: 15
                font.pixelSize: 14
                color: Style.content_secondary
                wrapMode: Text.WrapAnywhere
                horizontalAlignment : Text.AlignHCenter
                text: ""
                copyMenuEnabled: true
                onCopyText: BeamGlobals.copyToClipboard(text)
            }
            SFText {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom
                width: parent.width
                leftPadding: 20
                rightPadding: 20
                bottomPadding: 30
                font.pixelSize: 14
                font.italic:    true
                color: Style.content_main
                wrapMode: Text.Wrap
                horizontalAlignment : Text.AlignHCenter
                //: settings tab, show owner key message
/*% "Please notice, that knowing your owner key allows to
know all your funds (UTXO). Make sure that you
deploy the key at the node you trust completely."*/
                text: qsTrId("settings-show-owner-key-message")
            }
        }
    }

    onAccepted: {
        BeamGlobals.copyToClipboard(ownerKeyValue.text);
    }

    onOpened: {
        ownerKeyValue.text = settingsViewModel.getOwnerKey(thisDialog.pwd);
    }
}