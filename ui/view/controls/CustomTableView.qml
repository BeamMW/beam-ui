import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls.impl 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

TableView {
    id: tableView
    property int headerHeight: 46
    property int headerTextFontSize: 14
    property int headerTextLeftMargin: 20
    property var mainBackgroundRect: null
    property var backgroundRect: mainBackgroundRect != null ? mainBackgroundRect : main.backgroundRect
    property color headerColor: Style.table_header

    // Scrollbar fine-tuning
    __scrollBarTopMargin: tableView.headerHeight
    verticalScrollBarPolicy: hoverArea.containsMouse ? Qt.ScrollBarAlwaysOn : Qt.ScrollBarAsNeeded

    style: TableViewStyle {
        transientScrollBars: !hoverArea.containsMouse
        minimumHandleLength: 30

        handle: Rectangle {
            color: "white"
            radius: 3
            opacity: __verticalScrollBar.handlePressed ? 0.12 : 0.5
            implicitWidth: 6
        }

        scrollBarBackground: Rectangle {
            color: "transparent"
            implicitWidth: 6
            anchors.topMargin: 46
        }

        decrementControl: Rectangle {
            color: "transparent"
        }

        incrementControl: Rectangle {
            color: "transparent"
        }
    }

    function getAdjustedColumnWidth (column) {
        var acc = 0;
        for (var i = 0; i < columnCount; ++i)
        {
            var c = getColumn(i);
            if (c == column) continue;
            acc += c.width;
        }
        return width - acc;
    }

    frameVisible: false
    backgroundVisible: false
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    headerDelegate: Rectangle {
        id: rect
        height: headerHeight
       
        color:"transparent"// Style.background_main

        ShaderEffectSource {
            id: shaderSrc
            objectName: "renderRect"

            sourceRect.x: rect.mapToItem(backgroundRect, rect.x, rect.y).x
            sourceRect.y: rect.mapToItem(backgroundRect, rect.x, rect.y).y
            sourceRect.width: rect.width
            sourceRect.height: rect.height
            width: rect.width
            height: rect.height
            sourceItem: backgroundRect
            visible: true
        }

        property bool lastColumn: styleData.column == tableView.columnCount-1
        property bool firstOrLastColumn : styleData.column == 0 || lastColumn
        
        clip: firstOrLastColumn
        Rectangle {
            x: lastColumn ? -12 : 0
            width: parent.width + (firstOrLastColumn ? 12 : 0)
            height: parent.height + (firstOrLastColumn ? 12 : 0)
            color: headerColor
            radius: firstOrLastColumn ? 10 : 0
        }

        IconLabel {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: headerTextLeftMargin
            font.pixelSize: headerTextFontSize
            color: tableView.sortIndicatorColumn == styleData.column ? Style.content_main : Style.content_secondary
            font.weight: tableView.sortIndicatorColumn == styleData.column ? Font.Bold : Font.Normal
            font.family: "SF Pro Display"
            font.styleName: "Regular"

            icon.source: styleData.value == "" ? "" : tableView.sortIndicatorColumn == styleData.column ? "qrc:/assets/icon-sort-active.svg" : "qrc:/assets/icon-sort.svg"
            icon.width: 5
            icon.height: 8
            spacing: 6
            mirrored: true

            text: styleData.value
        }
    }

    MouseArea {
        id:               hoverArea
        anchors.fill:     parent
        acceptedButtons:  Qt.NoButton
        hoverEnabled:     true
        propagateComposedEvents: true
        preventStealing: true
    }

    Component.onCompleted: {
        var numchilds = __scroller.children.length
        __scroller.children[numchilds -1].anchors.rightMargin = 0
    }
}