import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

CustomTableView {
    id: rootControl

    property bool isShieldedSupported: true

    property int rowHeight: 56
    property int resizableWidth: parent.width - actions.width
    property double columnResizeRatio: resizableWidth / 914

    property var parentModel
    property bool isExpired: false
    property var showQRDialog
    anchors.fill: parent
    frameVisible: false
    selectionMode: SelectionMode.NoSelection
    backgroundVisible: false
    sortIndicatorVisible: true
    sortIndicatorColumn: 0
    sortIndicatorOrder: Qt.DescendingOrder

    //onSortIndicatorColumnChanged: {
    //    if (sortIndicatorColumn != 3 &&
    //        sortIndicatorColumn != 4) {
    //        sortIndicatorOrder = Qt.AscendingOrder;
    //    } else {
    //        sortIndicatorOrder = Qt.DescendingOrder;
    //    }
    //}

    TableViewColumn {
        role: parentModel.nameRole
        //% "Comment"
        title: qsTrId("general-comment")
        width: 300 * rootControl.columnResizeRatio
        resizable: false
        movable: false
        //delegate: TableItem {
        //    text:           styleData.value
        //    elide:          styleData.elideMode
        //    fontWeight:     Font.Bold
        //    fontStyleName: "Bold"
        //}
    }

    TableViewColumn {
        role: rootControl.isShieldedSupported ? parentModel.tokenRole : parentModel.walletIDRole
        //% "Address"
        title: qsTrId("general-address")
        width: 280 *  rootControl.columnResizeRatio
        movable: false
        resizable: false
        delegate: 
        Item {
            width: parent.width
            height: rootControl.rowHeight
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
        id:   identityColumn
        role: viewModel.identityRole
        //% "Wallet's signature"
        title: qsTrId("general-wallet-signature")
        width: rootControl.getAdjustedColumnWidth(identityColumn)//150 *  rootControl.columnResizeRatio
        resizable: false
        movable: false
        delegate:
        Item {
            width: parent.width
            height: rootControl.rowHeight
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

    //TableViewColumn {
    //    role: parentModel.expirationRole
    //    //% "Expiration date"
    //    title: qsTrId("general-exp-date")
    //    width: 150 *  rootControl.columnResizeRatio
    //    resizable: false
    //    movable: false
    //    visible: false
    //    delegate: Item {
    //        Item {
    //            width: parent.width
    //            height: rootControl.rowHeight
    //
    //            SFText {
    //                font.pixelSize: 14
    //                anchors.left: parent.left
    //                anchors.right: parent.right
    //                anchors.leftMargin: 20
    //                elide: Text.ElideRight
    //                anchors.verticalCenter: parent.verticalCenter
    //                text: Utils.formatDateTime(styleData.value, BeamGlobals.getLocaleName())
    //                color: Style.content_main
    //            }
    //        }
    //    }
    //}
    //
    //TableViewColumn {
    //    id: createdColumn
    //    role:parentModel.createdRole
    //    //% "Created"
    //    title: qsTrId("general-created")
    //    width: rootControl.getAdjustedColumnWidth(createdColumn)
    //    resizable: false
    //    movable: false
    //    visible: false
    //    delegate: Item {
    //        Item {
    //            width: parent.width
    //            height: rootControl.rowHeight
    //
    //            SFText {
    //                font.pixelSize: 14
    //                anchors.left: parent.left
    //                anchors.right: parent.right
    //                anchors.leftMargin: 20
    //                elide: Text.ElideRight
    //                anchors.verticalCenter: parent.verticalCenter
    //                text: Utils.formatDateTime(styleData.value, BeamGlobals.getLocaleName())
    //                color: Style.content_main
    //            }
    //        }
    //    }
    //}

    TableViewColumn {
        id: actions
        title: ""
        width: 40
        movable: false
        resizable: false
        delegate: txActions
    }

    rowDelegate: Item {

        height: rootControl.rowHeight

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
                    contextMenu.addressItem = rootControl.model[styleData.row]
                    contextMenu.popup()
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
                height: rootControl.rowHeight

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
                            contextMenu.addressItem = rootControl.model[styleData.row]
                            contextMenu.popup()
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
        property string address
        property var addressItem
        Action {
            id:          receiveAction
            //: Entry in address table context menu to get receive token
            //% "receive"
            text:        qsTrId("address-table-cm-receive")
            icon.source: "qrc:/assets/icon-receive-blue.svg"
            enabled:     contextMenu.addressItem && !contextMenu.addressItem.isExpired
            onTriggered: {
                main.openReceiveDialog(contextMenu.addressItem.token)
            }
        }

        Action {
            id: showQRAction
            //: Entry in address table context menu to show QR
            //% "Show QR code"
            text: qsTrId("address-table-cm-show-qr")
            icon.source: "qrc:/assets/icon-qr.svg"
            onTriggered: {
                var popup = Qt.createComponent("AddressQRDialog.qml").createObject(main)
                popup.address = rootControl.isShieldedSupported
                    ? contextMenu.addressItem.token
                    : contextMenu.addressItem.walletID;

                 //: show qr dialog address label
                //% "Your address"
                popup.addressLabelText = qsTrId("show-qr-tx-token-label");
                popup.open();
            }
        }

        Action {
            //: Entry in address table context menu to edit
            //% "Edit address"
            text: qsTrId("address-table-cm-edit")
            icon.source: "qrc:/assets/icon-edit.svg"
            onTriggered: {
                var dialog = Qt.createComponent("EditAddress.qml").createObject(main, {
                    viewModel:           rootControl.parentModel,
                    addressItem:         contextMenu.addressItem,
                    isShieldedSupported: rootControl.isShieldedSupported
                })
                dialog.open();
            }
        }

        Action {
            //: Entry in address table context menu to delete
            //% "Delete address"
            text: qsTrId("address-table-cm-delete")
            icon.source: "qrc:/assets/icon-delete.svg"
            onTriggered: {
                if (viewModel.isWIDBusy(contextMenu.addressItem.walletID)) {
                    return deleteAddressDialog.open()
                }
                viewModel.deleteAddress(contextMenu.addressItem.token)
            }
        }
    
        Component.onCompleted: {
            if (isExpired) {
                contextMenu.removeAction(showQRAction);
            }
        }
    }
    
    ConfirmationDialog {
        id:                 deleteAddressDialog
        width:              460
        //% "Delete address"
        title:              qsTrId("addresses-delete-warning-title")
        //% "There is active transaction that uses this address, therefore the address cannot be deleted."
        text:               qsTrId("addresses-delete-warning-text")
        //% "Ok"
        okButtonText:       qsTrId("general-ok")
        okButtonIconSource: "qrc:/assets/icon-done.svg"
        cancelButtonVisible: false
    }
}
