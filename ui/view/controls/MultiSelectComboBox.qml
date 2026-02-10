import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Controls.impl
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import "."
import "../utils.js" as Utils

ComboBox {
    id: control
    
    spacing: 8
    property int dropSpacing: 20
    property int fontPixelSize: 12
    property int dropFontPixelSize: 13
    property double fontLetterSpacing: 0
    property string color: Style.content_main
    property string underlineColor: color
    property int textMaxLenDrop: 0

    property var modelWidth: control.width
    property var calculatedWidth: Math.max(control.width, modelWidth)

    property bool showBackground: true
    property var onSelectChanged: function(id, state) {
        console.log(id);
        console.log(state);
    }

    TextMetrics {
        id: textMetrics
        font {
            family:        "Proxima Nova"
            styleName:     "Bold"
            weight:        Font.Bold
            pixelSize:     control.dropFontPixelSize
            letterSpacing: control.fontLetterSpacing
        }
    }

    delegate: ItemDelegate {
        id: itemDelegate
        width: calculatedWidth
        padding: 0

        contentItem: RowLayout {
            spacing: 0
            property var itemData: modelData

            CustomCheckBox {
                id: checkBox
                Layout.alignment: Qt.AlignLeft
                checked: parent.itemData["checked"]
                onClicked: {
                    control.onSelectChanged(parent.itemData["id"], checked);
                }
            }

            SFText {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: modelData["text"]
                color: checkBox.checked ? Style.active : Style.content_main
                elide: Text.ElideRight
                font.pixelSize: dropFontPixelSize
                font.letterSpacing: fontLetterSpacing
                font.styleName: checkBox.checked ? "Bold" : "DemiBold"
                font.weight: checkBox.checked ? Font.Bold : Font.DemiBold
            }
        }
        background: Item {}
    }

    indicator: Item {}

    function recalcSize() {
        if (model) {
            for(var i = 0; i < model.length; i++) {
                textMetrics.text = Utils.limitText(model[i].text, control.textMaxLenDrop)
                modelWidth = Math.max(textMetrics.width + 26, modelWidth)
            }
        }
    }

    onDownChanged: {
        recalcSize();
    }

    contentItem: RowLayout {
        spacing: 0

        SFText  {
            Layout.fillWidth:   true
            Layout.alignment:   Qt.AlignVCenter
            Layout.rightMargin: control.enabled ? 10 : 0

            clip: true
            text: {
                var titleText = "";
                var checkedCount = 0;
                for (var mData of model) {
                    if (mData["checked"]){
                         if (titleText.length === 0) {
                            titleText = mData["text"];
                         }
                         ++checkedCount;
                    }
                }

                if (!checkedCount) {
                    //% "None"
                    return qsTrId("multiselect-none");
                } else if (checkedCount == model.length) {
                    //% "All"
                    return qsTrId("multiselect-all");
                } else if (checkedCount == 1) {
                    return titleText;
                    
                } else {
                    return titleText + " +" + (checkedCount - 1);
                }
            }
            opacity: 0.5
            color: Style.content_main
            font.pixelSize: fontPixelSize
            font.weight: Font.Bold
            font.styleName: "Bold"
            elide: Text.ElideRight
        }

        SvgImage {
            id: imgDown
            source: "qrc:/assets/icon-down.svg"
            Layout.alignment: Qt.AlignVCenter
            visible: control.enabled && !control.down
            sourceSize: Qt.size(7, 5)
        }
        SvgImage {
            id: imgUp
            source: "qrc:/assets/icon-up.svg"
            Layout.alignment: Qt.AlignVCenter
            visible: control.enabled && control.down
            sourceSize: Qt.size(7, 5)
        }
    }

    background: Item {
        Rectangle {
            width:  control.width
            height: 1
            visible: control.showBackground
            y: control.height - 1
            color: control.underlineColor
            opacity: (control.activeFocus || control.hovered)? 0.3 : 0.1
        }
    }

   popup: Popup {
        id: comboPopup

        x: -(width - control.width)
        y: control.height + 7
        width: modelWidth + leftPadding + rightPadding

        topPadding:    20
        bottomPadding: 20
        leftPadding:   20
        rightPadding:  20

        contentItem: ColumnLayout {
            spacing: 0
            ListView {
                id: listView
                Layout.fillWidth: true
                clip: true
                spacing: control.dropSpacing
                implicitHeight: contentHeight
                model: control.popup.visible ? control.delegateModel : null
            }
        }

        background: Rectangle {
            anchors.fill: parent
            color: Style.background_popup
            radius: 5
        }
    }
}
