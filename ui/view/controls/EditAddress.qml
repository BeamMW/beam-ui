import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.11
import Beam.Wallet 1.0
import "../utils.js" as Utils
import "."

Dialog {
	id:      control
	modal:   true
	x:       (parent.width - width) / 2
	y:       (parent.height - height) / 2
	padding: 30

    property var  viewModel
    property var  addressItem

    property var     token:         addressItem.address
    property bool    isToken:       !BeamGlobals.isAddress(token)
    property string  comment:       addressItem.name
    property var     expiration:    addressItem.expirationDate
    property bool    expired:       expiration < new Date(Date.now())
    property bool    neverExpires:  expiration == new Date(-1)
    property bool    commentValid:  comment == "" || comment == addressItem.name || viewModel.commentValid(comment)
    property bool    extended:      false

    background: Rectangle {
		radius: 10
        color:  Style.background_popup
    }

    contentItem: Item { ColumnLayout {
        spacing: 0

        SFText {
			Layout.alignment: Qt.AlignHCenter
			//% "Edit address"
			text: qsTrId("edit-addr-title")
			color: Style.content_main
			font.pixelSize: 18
			font.weight:    Font.Bold
		}

    	SFText {
    		Layout.topMargin: 20
    		//% "Address"
	    	text: qsTrId("edit-addr-addr")
		    color: Style.content_main
			font.pixelSize: 14
			font.weight: Font.Bold
		}

		ScrollView {
            Layout.maximumHeight:         200
            Layout.topMargin:             10
            Layout.preferredWidth:        control.isToken ? 578 : 510
            clip:                         true
            ScrollBar.horizontal.policy:  ScrollBar.AlwaysOff
            ScrollBar.vertical.policy:    ScrollBar.AsNeeded

            SFLabel {
                id:                       addressID
                width:                    control.isToken ? 578 : 510
                copyMenuEnabled:          true
                wrapMode:                 Text.Wrap
                font.pixelSize:           14
                color:                    Style.content_main
                text:                     control.token

                onCopyText: function () {
                    BeamGlobals.copyToClipboard(text)
                }
            }
        }

        SFText {
            //% "Expires on"
            text: qsTrId("edit-addr-expires-label")

            Layout.topMargin: 25
            color:            Style.content_main
            font.pixelSize:   14
            font.styleName:   "Bold"
            font.weight:      Font.Bold
        }

        SFText {
            Layout.topMargin: 10

            text: control.expired ?
                //% "This address is already expired"
                qsTrId("edit-addr-expired") :
                //% "This address never expires"
                Utils.formatDateTime(control.expiration, BeamGlobals.getLocaleName(), qsTrId("edit-addr-never-expires"))

            color: Style.content_main
            font.pixelSize: 14
        }

        Row {
            spacing: 15
            Layout.topMargin: 10

            LinkButton {
                visible:  !control.expired
                fontSize: 13

                //% "Expire now"
                text: qsTrId("edit-addr-expire-now")
                enabled: !viewModel.isAddressBusy(control.token)

                onClicked: {
                    var newExpiration = new Date(Date.now() - 1000)
                    control.expiration = newExpiration
                }
            }

            LinkButton {
                fontSize: 13
                visible: !control.neverExpires && (control.expired || !control.extended)

                text: control.expired ?
                    //% "Activate"
                    qsTrId("edit-addr-activate") :
                    //% "Extend"
                    qsTrId("edit-addr-extend")

                onClicked: {
                    var newExpiration = new Date()
                    newExpiration.setDate(newExpiration.getDate() + 61)
                    control.expiration = newExpiration
                    control.extended = true
                }
            }
        }

        SFText {
            //% "There is an active transaction for this address, therefore it cannot be expired."
            text: qsTrId("edit-addr-no-expire")
            color:            Style.content_secondary
            Layout.topMargin: 7
            font.pixelSize:   13
            font.italic:      true
            visible:          viewModel.isAddressBusy(control.token)
        }

        SFText {
            //% "Comment"
            text: qsTrId("general-comment")

            Layout.topMargin: 25
            color:            Style.content_main
            font.pixelSize:   14
            font.styleName:   "Bold";
            font.weight:      Font.Bold
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: 5
            spacing: 0

            SFTextInput {
                id: addressName
                Layout.fillWidth: true

                font.pixelSize:  14
                font.italic :    !control.commentValid
                backgroundColor: control.commentValid ? Style.content_main : Style.validator_error
                color:           control.commentValid ? Style.content_main : Style.validator_error
                text:            control.comment
            }

            Item {
                Layout.fillWidth: true
                SFText {
                    //% "Address with the same comment already exists"
                    text:    qsTrId("general-addr-comment-error")
                    color:   Style.validator_error
                    visible: !control.commentValid
                    font.pixelSize: 12
                }
            }
        }

        Binding {
            target: control
            property: "comment"
            value: addressName.text
        }

        RowLayout {
            Layout.topMargin: 35
            Layout.alignment: Qt.AlignHCenter
            spacing: 15

            CustomButton {
				Layout.preferredHeight: 40

				//% "Cancel"
				text:        qsTrId("general-cancel")
                icon.source: "qrc:/assets/icon-cancel.svg"
                icon.color:  Style.content_main

				onClicked: {
                    control.destroy()
                }
			}

			PrimaryButton {
				id: saveButton
				Layout.preferredHeight: 40
				Layout.alignment: Qt.AlignHCenter

				//% "Save"
				text: qsTrId("edit-addr-save-button")
                icon.source: "qrc:/assets/icon-done.svg"
                enabled: control.commentValid &&
                         (
                           control.comment !==  addressItem.name ||
                           control.expiration.getTime() !==  addressItem.expirationDate.getTime()
                        )

                onClicked: {
                    viewModel.saveChanges(control.token, control.comment, control.expiration)
                    control.destroy()
                }
            }
        }
    }}
}
