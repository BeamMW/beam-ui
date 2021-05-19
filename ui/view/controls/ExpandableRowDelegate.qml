import QtQuick 2.11

Rectangle {
    id: rowItem

    property bool collapsed: true
    property bool animating: false
    property bool rowInModel: true 
    property int  rowHeight: 10
    property color backgroundColor: Style.background_row_even
    property var onLeftClick: function() { return true; }
    property var tableView
    default property Component delegate  

    height:         rowHeight
    anchors.left:   parent.left
    anchors.right:  parent.right
    color:          backgroundColor                          

    onCollapsedChanged: {
        rowItem.height = collapsed
            ? rowItem.rowHeight
            : rowItem.rowHeight + txDetails.maximumHeight
        txDetails.height = collapsed ? 0 : txDetails.maximumHeight
    }

    onRowInModelChanged: {
        if (!rowInModel) {
            collapsed = true
        }
    }

    function expand(animate) {
        if (!rowInModel) return

        if (!txDetails.detailsPanel)
        {
            txDetails.detailsPanel = rowItem.delegate.createObject(txDetails);
            txDetails.detailsPanel.implicitHeightChanged.connect(txDetails.onDelegateImplicitHeightChanged);
            txDetails.onDelegateImplicitHeightChanged();
        }

        if (animate) expandAnimation.start()
        else collapsed = false
    }

    function collapse(animate) {
        if (!rowInModel) return
        if (animate) collapseAnimation.start()
        else collapsed = true
    }

    Item {
        id:      txDetails
        height:  0
        y:       rowItem.rowHeight
        width:   parent.width
        clip:    true
        property int maximumHeight: 0
        property var detailsPanel

        function onDelegateImplicitHeightChanged() {
            if (txDetails && txDetails.detailsPanel !== undefined) {
                maximumHeight = txDetails.detailsPanel.implicitHeight;
                rowItem.collapsedChanged();
            }
        }

        Rectangle {
            anchors.fill: parent
            color: Style.background_details
        }
    }

    MouseArea {
        anchors.top:      parent.top
        anchors.left:     parent.left
        height:           rowItem.rowHeight
        width:            parent.width
        acceptedButtons:  Qt.LeftButton | Qt.RightButton

        onClicked: {
            if (!rowInModel)
            {
                return;
            }
            if (mouse.button === Qt.RightButton )
            {
                transactionsTable.showContextMenu(styleData.row);
            }
            else if (mouse.button === Qt.LeftButton)
            {
                if (onLeftClick()) {
                    parent.collapsed ? expand(true) : collapse(true);
                }
            }
        }
    }

    ParallelAnimation {
        id: expandAnimation
        running: false
        property int expandDuration: 200

        NumberAnimation {
            target: rowItem
            easing.type: Easing.Linear
            property: "height"
            to: rowItem.rowHeight + txDetails.maximumHeight
            duration: expandAnimation.expandDuration
        }

        NumberAnimation {
            target: txDetails
            easing.type: Easing.Linear
            property: "height"
            to: txDetails.maximumHeight
            duration: expandAnimation.expandDuration
        }

        onStarted: {
            rowItem.animating = true
        }

        onStopped: {
            rowItem.collapsed = false
            rowItem.animating = false
        }
    }

    ParallelAnimation {
        id: collapseAnimation
        running: false
        property int collapseDuration: 200

        NumberAnimation {
            target: rowItem
            easing.type: Easing.Linear
            property: "height"
            to: rowItem.rowHeight
            duration: collapseAnimation.collapseDuration
        }

        NumberAnimation {
            target: txDetails
            easing.type: Easing.Linear
            property: "height"
            to: 0
            duration: collapseAnimation.collapseDuration
        }

         onStarted: {
            rowItem.animating = true
        }

        onStopped: {
            rowItem.collapsed = true
            rowItem.animating = false
        }
    }
}
