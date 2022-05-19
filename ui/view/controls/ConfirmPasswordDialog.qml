	import QtQuick 2.11
	import QtQuick.Controls 2.3
	import QtQuick.Layouts 1.12
	import Beam.Wallet 1.0
	import "."

	CustomDialog {
	property var settingsViewModel: function() {
		return {
			checkWalletPassword: function() {
				console.log("settingsViewModel::checkWalletPassword undefined")
				return false
			}
		}
	}

	property string dialogTitle: "title"
	property string dialogMessage: "message"
	property alias okButtonText: okButton.text
	property alias okButtonIcon: okButton.icon.source
	property alias cancelButtonText: cancelButton.text
	property alias cancelButtonIcon: cancelButton.icon.source
	property alias pwd: pwd.text
	property bool showError: false
	property var onDialogAccepted: function() {
		console.log("Accepted");
	}
	property var onDialogRejected: function() {
		console.log("Rejected");
	}
	property var onPwdEntered: function(password) {
		if (settingsViewModel.checkWalletPassword(password)) {
			accept();
		} else {
			showError = true;
			pwd.selectAll();
			pwd.focus = true;
		}
	}

	modal: true

	x: (parent.width - width) / 2
	y: (parent.height - height) / 2
	visible:        false
	parent:         Overlay.overlay
	padding:        30

	contentItem: ColumnLayout {
		spacing: 30

		SFText {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			text: dialogTitle
			color: Style.content_main
			horizontalAlignment: Text.AlignHCenter
			font.pixelSize: 18
			font.styleName: "Bold"; font.weight: Font.Bold
		}

		ColumnLayout {
			Layout.fillWidth:		true
			SFText {
				Layout.fillWidth:		true
				Layout.alignment:		Qt.AlignHCenter
				text:					dialogMessage
				color:					Style.content_main
				font.pixelSize:			14
				wrapMode:				Text.Wrap
			}

			SFTextInput {
				id: pwd
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				font.pixelSize: 14
				rightPadding:   0
				color: showError ? Style.validator_error : Style.content_main
				backgroundColor: showError ? Style.validator_error : Style.content_main
				echoMode: TextInput.Password
				onTextEdited: {
					showError = false;
				}
				Keys.onEnterPressed: {
					onPwdEntered(text);
				}
				Keys.onReturnPressed: {
					onPwdEntered(text);
				}
			}

			Item {
				Layout.preferredHeight: 16
				Layout.topMargin: -5
				SFText {
					Layout.fillWidth: true
					Layout.alignment: Qt.AlignHCenter
					color: Style.validator_error
					font.pixelSize: 12
					//% "Please, enter password"
					text: qsTrId("general-pwd-empty-error")
					visible: showError && !pwd.text.length
				}
				SFText {
					Layout.fillWidth: true
					Layout.alignment: Qt.AlignHCenter
					color: Style.validator_error
					font.pixelSize: 12
					//% "Invalid password provided"
					text: qsTrId("general-pwd-invalid")
					visible: showError && pwd.text.length
				}
			}
		}			 	

		RowLayout {
			spacing: 20
			Layout.topMargin: -10
			Layout.alignment: Qt.AlignHCenter

			CustomButton {
				id: cancelButton
				//% "Cancel"
				text: qsTrId("general-cancel")
				icon.source: "qrc:/assets/icon-cancel-white.svg"
				onClicked: reject()
			}

			PrimaryButton {
				id: okButton
				//: confirm password dialog, ok button
				//% "Proceed"
				text: qsTrId("general-proceed")
				enabled: !showError
				icon.source: "qrc:/assets/icon-done.svg"
				onClicked: {
					onPwdEntered(pwd.text);
				}
			}
		}
	}

	onOpened: {
		pwd.text = "";
		showError = false;
		pwd.forceActiveFocus(Qt.TabFocusReason);
	}

	onAccepted: {
		if (typeof onDialogRejected == "function") {
			onDialogAccepted();
		}
	}
	onRejected: {
		if (typeof onDialogRejected == "function") {
			onDialogRejected();
		}
	}	
	}
