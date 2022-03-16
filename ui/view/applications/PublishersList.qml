import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"
import "../utils.js" as Utils

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27

    property var appsList: undefined
    readonly property bool hasApps: !!appsList && appsList.length > 0
    property var onBack
    property var addPublisherByKey

    function showAddPublisherDialog() {
        addPublisherDialog.open()
    }

    function checkVisibilityOfSocialNetwork(row, socialNetwork) {
        return typeof(tableView.model.getRoleValue(row, socialNetwork)) !== "undefined"
    }

    //
    // Page Header (Back button + title + add publisher button)
    //
    RowLayout {
        id: header

        CustomButton {
            id:             backButton
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true

            font {
                styleName: "DemiBold"
                weight:    Font.DemiBold
            }

            //% "Back"
            text:        qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            visible:     true

            onClicked:   control.onBack()
        }

        SFText {
            Layout.fillWidth:     true
            color:                Style.content_main
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            font.weight:          Font.Bold
            font.capitalization:  Font.AllUppercase
            //% "Publishers that i follow"
            text: qsTrId("dapps-store-publishers-page-title")
        }

        CustomButton {
            Layout.alignment: Qt.AlignRight
            palette.button:   Qt.rgba(255, 255, 255, 0.1)
            //% "add publisher"
            text: qsTrId("dapps-store-add-publisher")
            icon.source:      "qrc:/assets/icon-dapps_store-add-publisher.svg"
            icon.color:       Style.white
            onClicked:        showAddPublisherDialog()
        }
    }

    CustomDialog {
        id:      addPublisherDialog
        modal:   true
        x:       (parent.width - width) / 2
        y:       (parent.height - height) / 2
        parent:  Overlay.overlay

        onOpened: {
            forceActiveFocus()
        }

        onClosed: {
            publisherKeyInput.text = ""
        }

        contentItem: ColumnLayout {
            spacing: 0

            // title
            SFText {
                Layout.fillWidth:    true
                Layout.topMargin:    40
                Layout.bottomMargin: 30
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      18
                color:               Style.content_main
                //% "Add publisher"
                text:                qsTrId("dapps-store-add-publisher-title")
            }

            // Note
            SFText {
                id:                  addressField
                Layout.bottomMargin: 30
                Layout.fillWidth:    true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      14
                color:               Style.content_main
                //% "To add a publisher you need to paste his Publisher Key in the field below"
                text:                qsTrId("dapps-store-add-publisher-note")
            }

            // input
            SFTextInput {
                id:                    publisherKeyInput
                Layout.bottomMargin:   publisherKeyError.visible ? 6 : 30
                Layout.leftMargin:     100
                Layout.rightMargin:    100
                Layout.fillWidth:      true
                Layout.preferredWidth: 560
                font.pixelSize:        14
                color:                 publisherKeyError.visible ? Style.validator_error : Style.content_main
                backgroundColor:       publisherKeyError.visible ? Style.validator_error : Style.content_main
                leftPadding:           15
                rightPadding:          15
                validator:             RegExpValidator { regExp: /[0-9a-fA-F]{66}/ }

                background: Rectangle {
                    color:   publisherKeyError.visible ? Style.validator_error : Style.white
                    opacity: 0.1
                    radius:  10
                }
                onTextChanged: resetError()

                function resetError() {
                    publisherKeyError.visible = false
                }
            }

            SFText {
                id:                  publisherKeyError
                visible:             false
                Layout.bottomMargin: 20
                Layout.leftMargin:   100
                color:               Style.validator_error
                font.pixelSize:      12
                font.italic:         true
                //% "Publisher with that Key is not found"
                text:                qsTrId("dapps-store-add-publisher-error")
            }

            Row {
                id:                  buttonsLayout
                Layout.fillHeight:   true
                Layout.bottomMargin: 30
                Layout.alignment:    Qt.AlignHCenter
                spacing:             30

                CustomButton {
                    icon.source: "qrc:/assets/icon-cancel-16.svg"
                    //% "Close"
                    text:        qsTrId("general-close")
                    onClicked: {
                        addPublisherDialog.close()
                    }
                }

                PrimaryButton {
                    enabled:     !publisherKeyError.visible && publisherKeyInput.acceptableInput
                    icon.source: "qrc:/assets/icon-dapps_store-add-publisher-submit.svg"
                    icon.height: 10
                    icon.width:  12
                    //% "Submit"
                    text:        qsTrId("dapps-store-submit")
                    onClicked: {
                        var publisherName = addPublisherByKey(publisherKeyInput.text)
                        if (publisherName) {
                            // TODO: Do we need to create a specific notification popup?

                            //% "%1 added to the list of publishers that you follow"
                            main.showSimplePopup(qsTrId("dapps-store-add-publisher-notification").arg(publisherName))
                            addPublisherDialog.close()
                            return
                        }
                        
                        publisherKeyError.visible = true
                    }
                }
            }
        }
    }



    Item {
        Layout.preferredWidth: 20
    }
    //
    // Body: publishers list
    //
    CustomTableView {
        id: tableView
        Layout.alignment:  Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth:  true

        selectionMode: SelectionMode.NoSelection
        sortIndicatorVisible: true
        sortIndicatorColumn: 5
        sortIndicatorOrder: Qt.DescendingOrder

        property int rowHeight: 86
        property int resizableWidth: parent.width - publisherLink.width
        property double columnResizeRatio: resizableWidth / 914

        model: PublishersViewModel {
            id: viewModel
        }

        TableViewColumn { 
            id: nickname
            role: viewModel.nicknameRole
            //% "nickname"
            title: "Nickname"
            width: 150
            movable:    false
            resizable:  false
            delegate: nicknameComponent
        }
        TableViewColumn {
            id: about
            role: viewModel.aboutRole
            //% "about"
            title: "About"
            width: tableView.getAdjustedColumnWidth(about)
            movable:    false
            resizable:  false
        }
        TableViewColumn {
            id: socialNetworks
            //role: viewModel.socialNetworksRole
            //% "social-networks"
            title: "Social networks"
            width: 162 * tableView.columnResizeRatio
            movable:    false
            resizable:  false
            delegate: socialNetworksComponent
        }
        TableViewColumn {
            id: publisherLink
            //% "publisher-link"
            title: "Publisher link"
            width: 138
            movable:    false
            resizable:  false
            delegate: publisherLinkComponent
        }

        Component {
            id: nicknameComponent
            //check
            Item {
                width: parent.width
                height: tableView.rowHeight

                GridLayout {
                    Layout.fillWidth:   true
                    Layout.alignment:   Qt.AlignCenter
                    columns:            1
                    SFLabel {
                        Layout.leftMargin: 15
                        Layout.alignment: Qt.AlignTop
                        font.pixelSize: 14
                        elide: Text.ElideNone
                        fontSizeMode: Text.Fit
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        color: Style.content_main
                        text: tableView.model.getRoleValue(styleData.row, viewModel.nicknameRole)
                    }

                    SFLabel {
                        Layout.leftMargin: 15
                        font.pixelSize: 14
                        elide: Text.ElideNone
                        fontSizeMode: Text.Fit
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        color: Style.content_secondary
                        text: tableView.model.getRoleValue(styleData.row, viewModel.shortTitleRole)
                    }
                }
            }
        }

        Component {
            id: socialNetworksComponent
            Item {
                width: parent.width
                height: tableView.rowHeight
                RowLayout {
                   spacing: 0
                  // Layout.alignment: Qt.AlignCenter
                   CustomToolButton {
                       Layout.leftMargin: 15
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.websiteRole)
                       //Layout.alignment: Qt.AlignTop
                       icon.source: "qrc:/assets/icon-dapps-store-website.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.websiteRole)
                            );
                       }
                   }
                   CustomToolButton {
                  //     Layout.alignment: Qt.AlignCenter
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.discordRole)
                       icon.source: "qrc:/assets/icon-dapps-store-discord.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.discordRole)
                            );
                       }
                   }
                   CustomToolButton {
                       //Layout.alignment: Qt.AlignCenter
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.twitterRole)
                       icon.source: "qrc:/assets/icon-dapps-store-twitter.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.twitterRole)
                            );
                       }
                   }
                   CustomToolButton {
                       //Layout.alignment: Qt.AlignCenter
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.instagramRole)
                       icon.source: "qrc:/assets/icon-dapps-store-instagram.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.instagramRole)
                            );
                       }
                   }
                   CustomToolButton {
                       //Layout.alignment: Qt.AlignCenter
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.linkedinRole)
                       icon.source: "qrc:/assets/icon-dapps-store-linkedin.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.linkedinRole)
                            );
                       }
                   }

                   CustomToolButton {
                       //Layout.alignment: Qt.AlignCenter
                       visible: checkVisibilityOfSocialNetwork(styleData.row,viewModel.telegramRole)
                       icon.source: "qrc:/assets/icon-dapps-store-telegram.svg"
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                tableView.model.getRoleValue(styleData.row, viewModel.telegramRole)
                            );
                       }
                   }
               }
            }
        }


        // publisher link

        Component {
            id: publisherLinkComponent
            Item {
                Item {
                    width: parent.width
                    height: tableView.rowHeight
                    CustomToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 12
                        icon.source: "qrc:/assets/icon-actions.svg"
                        onClicked: {
                            publisherInfoContextMenu.publisherKey = tableView.model.getRoleValue(styleData.row, viewModel.publisherLinkRole);
                            publisherInfoContextMenu.popup();
                        }
                    }
                }
            }
        }
        ContextMenu {
            id: publisherInfoContextMenu
            modal: true
            dim: false
            property var publisherKey

            Action {
                //% "Copy publisher key"
                text: qsTrId("copy-publisher-key")
                icon.source: "qrc:/assets/icon-copy.svg"
                onTriggered: {
                    BeamGlobals.copyToClipboard(publisherInfoContextMenu.publisherKey);
                }
            }

            Action {
                //% "Remove from my list"
                text: qsTrId("remove-from-list")
                icon.source: "qrc:/assets/icon-delete.svg"
                onTriggered: {
                    // delete
                }
            }
        }

        rowDelegate: Rectangle {
            color:          styleData.alternate ? Style.background_row_even : Style.background_row_odd
            height:         tableView.rowHeight
            anchors.left:   parent.left
            anchors.right:  parent.right
        }
        itemDelegate: TableItem {
         //   elide: Text.ElideNone
            elide: Text.ElideRight
            fontSizeMode: Text.Fit
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: styleData.value
        }

        Component.onCompleted: tableView.resizeColumnsToContents()
    }
}
