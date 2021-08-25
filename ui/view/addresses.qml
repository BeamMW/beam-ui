import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.2
import "controls"
import Beam.Wallet 1.0

ColumnLayout {
    id: control
    anchors.fill: parent

	AddressBookViewModel {
	    id: viewModel
    }

	Title {
        //% "Address Book"
        text: qsTrId("addresses-tittle")
    }

    StatusBar {
        id: status_bar
        model: statusbarModel
    }

    property bool isShieldedSupported: statusbarModel.isConnectionTrusted && statusbarModel.isOnline

    ConfirmationDialog {
		id: confirmationDialog
        property bool isOwn
    }

    state: "active"
    states: [
        State {
            name: "active";
            PropertyChanges {target: activeAddressesFilter; state: "active"}
        },
        State {
            name: "expired";
            PropertyChanges {target: expiredAddressesFilter; state: "active"}
        },
        State {
            name: "contacts";
            PropertyChanges {target: contactsFilter; state: "active"}
            PropertyChanges { target: emptyMessage;  
                //% "Your contact list is empty"
                text: qsTrId("contacts-empty")
            }
        }
    ]

    RowLayout {
        Layout.minimumHeight: 40
        Layout.maximumHeight: 40
        Layout.topMargin:       54
        visible:                viewModel.contacts.length > 0  || viewModel.activeAddresses.length > 0 || viewModel.expiredAddresses.length > 0
        TxFilter{
            id: activeAddressesFilter
            //% "My active addresses"
            label: qsTrId("addresses-tab-active")
            onClicked: control.state = "active"
        }

        TxFilter{
            id: expiredAddressesFilter
            //% "My expired addresses"
            label: qsTrId("addresses-tab-expired")
            onClicked: control.state = "expired"
        }

        TxFilter{
            id: contactsFilter
            //% "Contacts"
            label: qsTrId("addresses-tab-contacts")
            onClicked: control.state = "contacts"
        }
    }

    ColumnLayout {
        Layout.topMargin: 90
        Layout.alignment: Qt.AlignHCenter
        Layout.fillHeight: true
        Layout.fillWidth:  true
        visible:          contactsViewItem.visible && contactsView.model.length == 0 ||
                          activeAddressesViewItem.visible && activeAddressesView.model.length == 0 ||
                          expiredAddressesViewItem.visible && expiredAddressesView.model.length == 0
    
        SvgImage {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/assets/icon-addressbook-empty.svg"
            sourceSize: Qt.size(60, 60)
        }
    
        SFText {
            id:                   emptyMessage
            Layout.topMargin:     30
            Layout.alignment:     Qt.AlignHCenter
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            color:                Style.content_main
            opacity:              0.5
            lineHeight:           1.43
            //% "Your address book is empty"
            text: qsTrId("addressbook-empty")
        }
    
        Item {
            Layout.fillHeight: true
        }
    }

    Item {
        Layout.fillWidth:  true
        Layout.fillHeight: true
        Item {
            id: activeAddressesViewItem
            visible:      control.state == "active"
            anchors.fill: parent
            AddressTable {
                id: activeAddressesView
                model: viewModel.activeAddresses
                parentModel: viewModel
                visible: activeAddressesView.model.length > 0
                isShieldedSupported: control.isShieldedSupported

                sortIndicatorVisible: true
                sortIndicatorColumn: 0
                sortIndicatorOrder: Qt.DescendingOrder
        
                Binding{
                    target: viewModel
                    property: "activeAddrSortRole"
                    value: activeAddressesView.getColumn(activeAddressesView.sortIndicatorColumn).role
                }
        
                Binding{
                    target: viewModel
                    property: "activeAddrSortOrder"
                    value: activeAddressesView.sortIndicatorOrder
                }
            }
        }
        Item {
            id: expiredAddressesViewItem
            visible:     control.state == "expired"
            anchors.fill: parent
            AddressTable {
                id: expiredAddressesView
                model: viewModel.expiredAddresses
                visible: expiredAddressesView.model.length > 0
                parentModel: viewModel
                isExpired: true
                isShieldedSupported: control.isShieldedSupported
        
                sortIndicatorVisible: true
                sortIndicatorColumn: 0
                sortIndicatorOrder: Qt.DescendingOrder
        
                Binding{
                    target: viewModel
                    property: "expiredAddrSortRole"
                    value: expiredAddressesView.getColumn(expiredAddressesView.sortIndicatorColumn).role
                }
        
                Binding{
                    target: viewModel
                    property: "expiredAddrSortOrder"
                    value: expiredAddressesView.sortIndicatorOrder
                }
            }
        }
        Item {
            id: contactsViewItem
            anchors.fill: parent
            visible: control.state == "contacts"
            CustomTableView {
                id: contactsView
            
                property int rowHeight: 56
                property int resizableWidth: parent.width - actions.width
                property double columnResizeRatio: resizableWidth / 914
            
                anchors.fill: parent
                frameVisible: false
                selectionMode: SelectionMode.NoSelection
                backgroundVisible: false
                model: viewModel.contacts
                sortIndicatorVisible: true
                sortIndicatorColumn: 0
                sortIndicatorOrder: Qt.DescendingOrder
                visible:            contactsView.model.length > 0
                
                Binding{
                    target: viewModel
                    property: "contactSortRole"
                    value: contactsView.getColumn(contactsView.sortIndicatorColumn).role
                }
            
                Binding{
                    target: viewModel
                    property: "contactSortOrder"
                    value: contactsView.sortIndicatorOrder
                }
            
                TableViewColumn {
                    role: viewModel.nameRole
                    //% "Comment"
                    title: qsTrId("general-comment")
                    width: 300 * contactsView.columnResizeRatio
                    movable: false
                }
            
                TableViewColumn {
                    role: control.isShieldedSupported ? viewModel.tokenRole : viewModel.walletIDRole
                    title: qsTrId("general-address")
                    width: 280 * contactsView.columnResizeRatio
                    movable: false
                    delegate: 
                    Item {
                        width: parent.width
                        height: contactsView.rowHeight
                        clip:true
            
                        SFLabel {
                            font.pixelSize: 14
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 20
                            anchors.rightMargin: 80
                            elide: Text.ElideMiddle
                            anchors.verticalCenter: parent.verticalCenter
                            text: styleData.value
                            color: Style.content_main
                            copyMenuEnabled: true
                            onCopyText: BeamGlobals.copyToClipboard(text)
                        }
                    }
                }
            
                TableViewColumn {
                    id: identityColumn
                    role: viewModel.identityRole
                    //% "Wallet's signature"
                    title: qsTrId("general-wallet-signature")
                    width: contactsView.getAdjustedColumnWidth(identityColumn)
                    movable: false
                    delegate:
                    Item {
                        width: parent.width
                        height: contactsView.rowHeight
                        clip:true
            
                        SFLabel {
                            font.pixelSize: 14
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 20
                            anchors.rightMargin: 100
                            elide: Text.ElideMiddle
                            anchors.verticalCenter: parent.verticalCenter
                            text: styleData.value
                            color: Style.content_main
                            copyMenuEnabled: true
                            onCopyText: BeamGlobals.copyToClipboard(text)
                        }
                    }
                }
            
                TableViewColumn {
                    //role: "status"
                    id: actions
                    title: ""
                    width: 40
                    movable: false
                    resizable: false
                    delegate: txActions
                }
            
                rowDelegate: Item {
            
                    height: contactsView.rowHeight
            
                    anchors.left: parent.left
                    anchors.right: parent.right
            
                    Rectangle {
                        anchors.fill: parent
                        color: styleData.selected ? Style.row_selected :
                                (styleData.alternate ? Style.background_row_even : Style.background_row_odd)
                    }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: {
                            if (mouse.button == Qt.RightButton && styleData.row != undefined)
                            {
                                contextMenu.walletID = contactsView.model[styleData.row].walletID;
                                contextMenu.token = contactsView.model[styleData.row].token;
                                contextMenu.popup();
                            }
                        }
                    }
                }
            
                itemDelegate: TableItem {
                    text: styleData.value
                    elide: styleData.elideMode
                }
            
                Component {
                    id: txActions
                    Item {
                        Item {
                            width: parent.width
                            height: contactsView.rowHeight
            
                            Row{
                                anchors.right: parent.right
                                anchors.rightMargin: 12
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 10
                                CustomToolButton {
                                    icon.source: "qrc:/assets/icon-actions.svg"
                                    //% "Actions"
                                    ToolTip.text: qsTrId("general-actions")
                                    onClicked: {
                                        contextMenu.walletID = contactsView.model[styleData.row].walletID;
                                        contextMenu.token = contactsView.model[styleData.row].token;
                                        contextMenu.popup();
                                    }
                                }
                            }
                        }
                    }
                }
            
                ContextMenu {
                    id: contextMenu
                    modal: true
                    dim: false
                    property string walletID
                    property string token
                    Action {
                        //% "Send"
                        text: qsTrId("general-send")
                        icon.source: "qrc:/assets/icon-send-blue.svg"
                        onTriggered: {
                            main.openSendDialog(contextMenu.token);
                        }
                    }
                    Action {
                        //% "Delete contact"
                        text: qsTrId("address-table-cm-delete-contact")
                        icon.source: "qrc:/assets/icon-delete.svg"
                        onTriggered: {
                            viewModel.deleteAddress(contextMenu.token);
                        }
                    }
                }
            }
        }
    }
}
