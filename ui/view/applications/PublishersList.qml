import QtQuick          2.15
import QtQuick.Layouts  1.15
//import QtQuick.Controls 1.4 as Controls1
import QtQuick.Controls 2.15
import Beam.Wallet      1.0
import "../controls"
import "../utils.js" as Utils

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27

    property var appsViewModel
    property var onBack

    function addUnwantedPublisherByKey(publisherKey) {
        return appsViewModel.addUnwantedPublisherByKey(publisherKey)
    }

    function removeUnwantedPublisherByKey(publisherKey) {
        appsViewModel.removeUnwantedPublisherByKey(publisherKey)
    }

    //% "Publishers"
    property string title: qsTrId("dapps-store-publishers-page-main-title")

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

        selectionMode:        Controls1.SelectionMode.NoSelection
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
        Controls1.TableViewColumn {
            role:      viewModel.publisherStatusRole
            width:     44 
            movable:   false
            resizable: false
            delegate:  showDappsComponent
        }
        Controls1.TableViewColumn { 
            id:        nickname
            role:      viewModel.nicknameRole
                       //% "Nickname"
            title:     qsTrId("publishers-list-nickname")
            width:     220 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  nicknameComponent
        }
        Controls1.TableViewColumn {
            id:        about
            role:      viewModel.aboutRole
                       //% "About"
            title:     qsTrId("publishers-list-about")
            width:     366 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  aboutComponent
        }
        Controls1.TableViewColumn {
            id:        socialNetworks
                       //% "Social networks"
            title:     qsTrId("publishers-list-social-net")
            width:     200 * publishersTable.columnResizeRatio
            movable:   false
            resizable: false
            delegate:  socialNetworksComponent
        }
        Controls1.TableViewColumn {
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
                        text:                modelData && modelData.nickname 
                    }

                    SFLabel {
                        Layout.maximumWidth: parent.width
                        maximumLineCount:    3
                        font.pixelSize:      14
                        elide:               Text.ElideRight
                        wrapMode:            Text.WrapAtWordBoundaryOrAnywhere
                        color:               Style.content_secondary
                        text:                modelData && modelData.shortTitle
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
                    text:                 modelData && modelData.aboutMe
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
                       visible:     modelData && !!modelData.website
                       icon.source: "qrc:/assets/icon-dapps-store-website-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                modelData && modelData.website
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     modelData && !!modelData.discord
                       icon.source: "qrc:/assets/icon-dapps-store-discord-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://discord.gg/" + modelData && modelData.discord
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     modelData && !!modelData.twitter
                       icon.source: "qrc:/assets/icon-dapps-store-twitter-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://twitter.com/" + modelData && modelData.twitter
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     modelData && !!modelData.instagram
                       icon.source: "qrc:/assets/icon-dapps-store-instagram-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                                "https://instagram.com/" + modelData && modelData.instagram
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     modelData && !!modelData.linkedin
                       icon.source: "qrc:/assets/icon-dapps-store-linkedin-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                               "https://linkedin.com/" + modelData && modelData.linkedin
                            );
                       }
                   }
                   CustomToolButton {
                       visible:     modelData && !!modelData.telegram
                       icon.source: "qrc:/assets/icon-dapps-store-telegram-green.svg"
                       icon.color: Style.active
                       onClicked: {
                           Utils.openExternalWithConfirmation(
                               "https://t.me/" + modelData && modelData.telegram
                            );
                       }
                   }
                }
            }
        }

        Component {
            id: showDappsComponent
            Item {
                anchors.fill:           parent
                CustomCheckBox {
                    id:                     showDappsCheckBox
                    anchors.right:          parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: {
                        if (!modelData) {
                            return false;
                        }
                        return modelData.enabled
                    }
                    onClicked : {
                        let publisherKey = modelData.publisherKey;
                        if (modelData.enabled) {
                            control.addUnwantedPublisherByKey(publisherKey)
                        } else {
                            control.removeUnwantedPublisherByKey(publisherKey)
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
                        publisherInfoContextMenu.publisherKey = modelData && modelData.publisherKey;
                        publisherInfoContextMenu.publisherEnabled = modelData && modelData.enabled;
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
            property bool publisherEnabled

            Action {
                                //% "Copy publisher key"
                text:        qsTrId("copy-publisher-key")
                icon.source: "qrc:/assets/icon-copy.svg"
                onTriggered: {
                    BeamGlobals.copyToClipboard(publisherInfoContextMenu.publisherKey);
                }
            }

            Action {                                  
                text:        !publisherInfoContextMenu.publisherEnabled ? 
                                //% "Show DApps"
                                qsTrId("dapps-store-publisher-show-dapps") : 
                                //% "Hide DApps"
                                qsTrId("dapps-store-publisher-hide-dapps")
                icon.source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/check.png"
                onTriggered: {
                    if (publisherInfoContextMenu.publisherEnabled) {
                        control.addUnwantedPublisherByKey(publisherInfoContextMenu.publisherKey)
                    } else {
                        control.removeUnwantedPublisherByKey(publisherInfoContextMenu.publisherKey)
                    }
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
