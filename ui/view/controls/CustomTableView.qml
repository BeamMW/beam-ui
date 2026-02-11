import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "."

Item {
    id: tableView

    // ── public API (matches old QtQuick.Controls 1 TableView) ──
    property int  headerHeight: 46
    property int  headerTextFontSize: 14
    property int  headerTextLeftMargin: 20
    property var  mainBackgroundRect: null
    property var  backgroundRect: mainBackgroundRect != null ? mainBackgroundRect : (typeof main !== "undefined" ? main.backgroundRect : null)
    property color headerColor: Style.table_header
    property bool headerShaderVisible: true
    property var  isSortIndicatorHidenForColumn: function(col) { return true; }

    property bool frameVisible: false
    property bool backgroundVisible: false

    // selection
    property int  currentRow: -1
    property int  selectionMode: 0   // 0 = noSelection, 1 = singleSelection
    property var  selection: QtObject {
        function select(row) { tableView.currentRow = row; }
        function clear()     { tableView.currentRow = -1;  }
    }

    // sorting
    property bool sortIndicatorVisible: false
    property int  sortIndicatorColumn: 0
    property int  sortIndicatorOrder: Qt.AscendingOrder

    // model
    property var model

    // delegates (compat)
    property Component rowDelegate: null
    property Component itemDelegate: null

    // ── column management ──
    default property list<QtObject> _data
    property var columns: []
    readonly property int columnCount: {
        var cnt = 0;
        for (var i = 0; i < columns.length; ++i)
            if (columns[i].visible) ++cnt;
        return cnt;
    }

    // Use a child Item to run init logic, so consumer Component.onCompleted
    // on the CustomTableView instance doesn't override this.
    Item {
        id: _columnInit
        visible: false
        Component.onCompleted: {
            var cols = [];
            for (var i = 0; i < tableView._data.length; ++i) {
                if (tableView._data[i] instanceof TableViewColumn) {
                    cols.push(tableView._data[i]);
                }
            }
            tableView.columns = cols;
        }
    }

    function getColumn(index) {
        var cnt = 0;
        for (var i = 0; i < columns.length; ++i) {
            if (columns[i].visible) {
                if (cnt === index) return columns[i];
                ++cnt;
            }
        }
        return null;
    }

    function getAdjustedColumnWidth(column) {
        var acc = 0;
        for (var i = 0; i < columns.length; ++i) {
            var c = columns[i];
            if (c !== column && c.visible)
                acc += c.width;
        }
        return width - acc;
    }

    function positionViewAtRow(row, mode) {
        listView.positionViewAtIndex(row, mode !== undefined ? mode : ListView.Beginning);
    }

    // ── visual layout ──
    clip: true

    // header row
    Row {
        id: headerRow
        z: 2
        width: parent.width
        height: tableView.headerHeight

        Repeater {
            model: tableView.columns.length
            delegate: Item {
                visible: tableView.columns[index].visible
                width:   tableView.columns[index].visible ? tableView.columns[index].width : 0
                height:  tableView.headerHeight
                clip:    isFirstOrLast

                property int  colIndex: index
                property bool isLast:  {
                    for (var i = tableView.columns.length - 1; i >= 0; --i)
                        if (tableView.columns[i].visible) return i === index;
                    return false;
                }
                property bool isFirst: {
                    for (var i = 0; i < tableView.columns.length; ++i)
                        if (tableView.columns[i].visible) return i === index;
                    return false;
                }
                property bool isFirstOrLast: isFirst || isLast

                // background
                Rectangle {
                    x: isLast ? -12 : 0
                    width: parent.width + (isFirstOrLast ? 12 : 0)
                    height: parent.height + (isFirstOrLast ? 12 : 0)
                    color: tableView.headerColor
                    radius: isFirstOrLast ? 10 : 0
                }

                // sort indicator + title
                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: tableView.headerTextLeftMargin
                    anchors.right: parent.right
                    anchors.rightMargin: 4
                    spacing: 6
                    layoutDirection: Qt.RightToLeft

                    SFText {
                        anchors.verticalCenter: parent.verticalCenter
                        text: tableView.columns[colIndex].title
                        font.pixelSize: tableView.headerTextFontSize
                        font.family: "Proxima Nova"
                        font.styleName: "Regular"
                        color: tableView.sortIndicatorColumn === colIndex ? Style.content_main : Style.content_secondary
                        font.weight: tableView.sortIndicatorColumn === colIndex ? Font.Bold : Font.Normal
                    }

                    SvgImage {
                        anchors.verticalCenter: parent.verticalCenter
                        visible: tableView.sortIndicatorVisible
                                 && !tableView.isSortIndicatorHidenForColumn({"column": colIndex, "value": tableView.columns[colIndex].title})
                                 && tableView.columns[colIndex].title !== ""
                        source: tableView.sortIndicatorColumn === colIndex
                                    ? "qrc:/assets/icon-sort-active.svg"
                                    : "qrc:/assets/icon-sort.svg"
                        sourceSize: Qt.size(5, 8)
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (tableView.sortIndicatorColumn === colIndex) {
                            tableView.sortIndicatorOrder = (tableView.sortIndicatorOrder === Qt.AscendingOrder)
                                ? Qt.DescendingOrder : Qt.AscendingOrder;
                        } else {
                            tableView.sortIndicatorColumn = colIndex;
                        }
                    }
                }
            }
        }
    }

    // list body
    ListView {
        id: listView
        anchors.top: headerRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        model: tableView.model
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            contentItem: Rectangle {
                implicitWidth: 6
                radius: 3
                color: "white"
                opacity: parent.pressed ? 0.12 : 0.5
            }
        }

        delegate: Item {
            id: rowRoot
            width: listView.width
            height: rowDelegateLoader.item ? rowDelegateLoader.item.height
                        : (tableView.rowDelegate ? 56 : rowContent.height)

            // expose model data to child delegates
            property var    rowModel: model
            property int    rowIndex: index
            property bool   isSelected: tableView.currentRow === index
            property bool   isAlternate: index % 2 === 1

            // row background
            Loader {
                id: rowDelegateLoader
                anchors.fill: parent
                sourceComponent: tableView.rowDelegate
                property var styleData: QtObject {
                    readonly property int  row:       rowRoot.rowIndex
                    readonly property bool selected:  rowRoot.isSelected
                    readonly property bool alternate: rowRoot.isAlternate
                }
                property var model: rowRoot.rowModel
            }

            // click for selection
            MouseArea {
                anchors.fill: parent
                enabled: tableView.selectionMode === 1
                z: -1
                onClicked: {
                    tableView.currentRow = rowRoot.rowIndex;
                }
            }

            // columns
            Row {
                id: rowContent
                width: parent.width

                Repeater {
                    model: tableView.columns.length
                    delegate: Item {
                        visible: tableView.columns[index].visible
                        width:   tableView.columns[index].visible ? tableView.columns[index].width : 0
                        height:  rowDelegateLoader.item ? rowDelegateLoader.item.height : 56

                        property var columnObj: tableView.columns[index]
                        property string roleName: columnObj.role
                        property var roleValue: {
                            if (!roleName || !rowRoot.rowModel) return "";
                            var v = rowRoot.rowModel[roleName];
                            return v !== undefined ? v : "";
                        }

                        Loader {
                            anchors.fill: parent
                            sourceComponent: columnObj.delegate
                                ? columnObj.delegate
                                : tableView.itemDelegate
                            property var styleData: QtObject {
                                readonly property var  value:     roleValue
                                readonly property int  row:       rowRoot.rowIndex
                                readonly property int  column:    index
                                readonly property bool selected:  rowRoot.isSelected
                                readonly property bool alternate: rowRoot.isAlternate
                                readonly property int  elideMode: columnObj.elideMode
                            }
                            property var model: rowRoot.rowModel
                        }
                    }
                }
            }
        }
    }
}