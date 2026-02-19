import QtQuick

QtObject {
    property string role: ""
    property string title: ""
    property real width: 100
    property bool movable: false
    property bool resizable: false
    property bool visible: true
    property int elideMode: Text.ElideRight
    property Component delegate: null
}
