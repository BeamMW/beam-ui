import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import Beam.Wallet      1.0
import "../controls"
import "../utils.js" as Utils

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27

    property var appsViewModel
    property var onBack

    function addPublisherByKey(publisherKey) {
        return appsViewModel.addPublisherByKey(publisherKey)
    }

    function removePublisherByKey(publisherKey) {
        appsViewModel.removePublisherByKey(publisherKey)
    }

    function showAddPublisherDialog() {
        addPublisherDialog.open()
    }

    function checkVisibilityOfSocialNetwork(row, socialNetwork) {
        return !!viewModel.getRoleValue(row, socialNetwork)
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
            icon.color:       Style.content_main
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
                font.weight:         Font.Bold
                color:               Style.content_main
                //% "Add publisher"
                text:                qsTrId("dapps-store-add-publisher-title")
            }

            // Note
            SFText {
                id:                  addressField
                Layout.bottomMargin: 20
                Layout.fillWidth:    true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      14
                color:               Style.content_main
                //% "To add a publisher you need to paste his Publisher Key in the field below"
                text:                qsTrId("dapps-store-add-publisher-note")
            }

            // input
            SFTextInput {
                id:                     publisherKeyInput
                Layout.bottomMargin:    publisherKeyError.visible ? 6 : 30
                Layout.leftMargin:      100
                Layout.rightMargin:     100
                Layout.fillWidth:       true
                Layout.preferredWidth:  560
                Layout.preferredHeight: 45
                height:                 45
                font.pixelSize:         14
                color:                  publisherKeyError.visible ? Style.validator_error : Style.content_main
                backgroundColor:        publisherKeyError.visible ? Style.validator_error : Style.content_main
                leftPadding:            15
                rightPadding:           15
                validator:              RegExpValidator { regExp: /[0-9a-fA-F]{66}/ }

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
                Layout.bottomMargin: 10
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
                    icon.source: "qrc:/assets/icon-next-blue.svg"
                    //% "Submit"
                    text:        qsTrId("dapps-store-submit")
                    onClicked: {
                        var publisherName = addPublisherByKey(publisherKeyInput.text)
                        if (publisherName) {
                            // TODO: Do we need to create a specific notification popup?

                            //% "<b>%1</b> added to the list of publishers that you follow"
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



    //
    // Body: publishers list
    //
     PublishersViewModel {
        id:             viewModel
        publishersInfo: control.appsViewModel.userPublishers
    }

    CustomTableView {
        id:                publishersTable
        Layout.alignment:  Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth:  true
        Layout.topMargin:  20

        selectionMode:        SelectionMode.NoSelection
        sortIndicatorVisible: true
        sortIndicatorColumn:  0
        sortIndicatorOrder:   Qt.DescendingOrder
        isSortIndicatorHidenForColumn: function(styleData) {
            return styleData.column != 0;
        }

        property int rowHeight:            109
        property real resizableWidth:      parent.width - 144
        property real columnResizeRatio:   resizableWidth / 828

        model:  viewModel.publishers

        onSortIndicatorColumnChanged: {
            if (publishersTable.sortIndicatorColumn != 0) {
                publishersTable.sortIndicatorColumn = 0;
            }
        }
        onSortIndicatorOrderChanged: {
            if (publishersTable.sortIndicatorColumn == 0) {
                viewModel.sortOrder = viewModel.sortOrder == Qt.DescendingOrder ? Qt.AscendingOrder : Qt.DescendingOrder;
            }
        }

        TableViewColumn { 
            id:        nickname
            role:      viewModel.nicknameRole
                       //% "Nickname"
            title:     qsTrId("publishers-list-nickname")
            width:     220 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  nicknameComponent
        }
        TableViewColumn {
            id:        about
            role:      viewModel.aboutRole
                       //% "About"
            title:     qsTrId("publishers-list-about")
            width:     400 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  aboutComponent
        }
        TableViewColumn {
            id:        socialNetworks
                       //% "Social networks"
            title:     qsTrId("publishers-list-social-net")
            width:     210 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  socialNetworksComponent
        }
        TableViewColumn {
            id:        publisherLink
                       //% "Publisher link"
            title:     qsTrId("publishers-list-publisher-link")
            width:     publishersTable.getAdjustedColumnWidth(publisherLink)//144
            movable:   false
            resizable: false
            delegate:  publisherLinkComponent
        }

        Component {
            id: nicknameComponent

            RowLayout {
                width: parent.width

                ColumnLayout {
                    Layout.preferredWidth: parent.width - 12
                    Layout.alignment:      Qt.AlignTop
                    Layout.topMargin:      12
                    Layout.leftMargin:     20
                    spacing:               0

                    SFLabel {
                        Layout.maximumWidth: parent.width
                        maximumLineCount:    2
                        font.pixelSize:      14
                        elide:               Text.ElideRight
                        wrapMode:            Text.WrapAtWordBoundaryOrAnywhere
                        color:               Style.content_main
                        text:                viewModel.getRoleValue(styleData.row, viewModel.nicknameRole)
                    }

                    SFLabel {
                        Layout.maximumWidth: parent.width
                        maximumLineCount:    3
                        font.pixelSize:      14
                        elide:               Text.ElideRight
                        wrapMode:            Text.WrapAtWordBoundaryOrAnywhere
                        color:               Style.content_secondary
                        text:                viewModel.getRoleValue(styleData.row, viewModel.shortTitleRole)
                    }
                }
            }
        }

        Component {
            id: aboutComponent

            RowLayout {
                width:  parent.width
                height: publishersTable.rowHeight

                SFLabel {
                    Layout.leftMargin:    20
                    Layout.alignment:     Qt.AlignTop
                    Layout.topMargin:     12
                    Layout.maximumWidth:  parent.width - 20
                    Layout.maximumHeight: parent.height - 12
                    font.pixelSize:       14
                    elide:                Text.ElideRight
                    wrapMode:             Text.WrapAtWordBoundaryOrAnywhere
                    color:                Style.content_main
                    text:                 viewModel.getRoleValue(styleData.row, viewModel.aboutRole)
                }                         
            }
        }

        Component {
            id: socialNetworksComponent
            RowLayout {
                width:  parent.width
                height: publishersTable.rowHeight
                RowLayout {
                    spacing:           0
                    Layout.leftMargin: 12
                    Layout.alignment:  Qt.AlignTop
                    Layout.topMargin:  4

                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.websiteRole)
                       icon.source: "qrc:/assets/icon-dapps-store-website-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                viewModel.getRoleValue(styleData.row, viewModel.websiteRole)
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.discordRole)
                       icon.source: "qrc:/assets/icon-dapps-store-discord-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://discord.gg/" + viewModel.getRoleValue(styleData.row, viewModel.discordRole)
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.twitterRole)
                       icon.source: "qrc:/assets/icon-dapps-store-twitter-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://twitter.com/" + viewModel.getRoleValue(styleData.row, viewModel.twitterRole)
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.instagramRole)
                       icon.source: "qrc:/assets/icon-dapps-store-instagram-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://instagram.com/" + viewModel.getRoleValue(styleData.row, viewModel.instagramRole)
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.linkedinRole)
                       icon.source: "qrc:/assets/icon-dapps-store-linkedin-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                               "https://linkedin.com/" + viewModel.getRoleValue(styleData.row, viewModel.linkedinRole)
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     checkVisibilityOfSocialNetwork(styleData.row,viewModel.telegramRole)
                       icon.source: "qrc:/assets/icon-dapps-store-telegram-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                               "https://t.me/" + viewModel.getRoleValue(styleData.row, viewModel.telegramRole)
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
                width:  parent.width
                height: publishersTable.rowHeight
                CustomToolButton {
                    anchors.top:         parent.top
                    anchors.topMargin:   4
                    anchors.right:       parent.right
                    anchors.rightMargin: 12
                    icon.source:         "qrc:/assets/icon-actions.svg"
                    onClicked: {            
                        publisherInfoContextMenu.publisherKey = viewModel.getRoleValue(styleData.row, viewModel.publisherLinkRole);
                        publisherInfoContextMenu.popup();
                    }
                }
            }
        }
        ContextMenu {
            id:    publisherInfoContextMenu
            modal: true
            dim:   false
            property var publisherKey

            Action {
                             //% "Copy publisher key"
                text:        qsTrId("copy-publisher-key")
                icon.source: "qrc:/assets/icon-copy.svg"
                onTriggered: {
                    BeamGlobals.copyToClipboard(publisherInfoContextMenu.publisherKey);
                }
            }

            Action {
                             //% "Remove from my list"
                text:        qsTrId("remove-from-list")
                icon.source: "qrc:/assets/icon-delete.svg"
                onTriggered: {
                    control.removePublisherByKey(publisherInfoContextMenu.publisherKey)
                }
            }
        }

        rowDelegate: Rectangle {
            color:  styleData.alternate ? Style.background_row_even : Style.background_row_odd
            height: publishersTable.rowHeight
        }
        itemDelegate: TableItem {
            elide:    Text.ElideRight
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
