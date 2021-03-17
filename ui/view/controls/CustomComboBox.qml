import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
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
    property bool enableScroll: false
    property int textMaxLenDrop: 0
    property int textMaxLenDisplay: 0
    property int dropOffset: 0

    property var modelWidth: control.width
    property var calculatedWidth: Math.max(control.width, modelWidth)

    TextMetrics {
        id: textMetrics
        font {
            family:        "SF Pro Display"
		    styleName:     "Regular"
		    weight:        Font.Normal
            pixelSize:     control.dropFontPixelSize
            letterSpacing: control.fontLetterSpacing
        }
    }

    delegate: ItemDelegate {
        id: itemDelegate
        width: calculatedWidth
        padding: 0

        property var iconW: Array.isArray(control.model)  ? modelData["iconWidth"]  : model["iconWidth"]
        property var iconH: Array.isArray(control.model)  ? modelData["iconHeight"] : model["iconHeight"]
        property var iconS: (Array.isArray(control.model) ? modelData["icon"]       : model["icon"]) || ""

        contentItem: Row {
            spacing: 0

            SvgImage {
                source:  iconS
                width:   iconW
                height:  iconH
                visible: iconW > 0
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                visible: iconW > 0
                width:   10
                height:  parent.height
            }

            SFText {
                text: {
                    var text = modelData
                    if (control.textRole) {
                        text = Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]
                    }
                    return Utils.limitText(text, control.textMaxLenDrop)
                }
                color: highlighted ? Style.active : Style.content_main
                elide: Text.ElideMiddle
                font.pixelSize: dropFontPixelSize
                font.letterSpacing: fontLetterSpacing
                font.styleName: highlighted ? "DemiBold" : "Normal"
                font.weight: highlighted ? Font.DemiBold : Font.Normal
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                width:  17
                height: parent.height
            }
        }

        highlighted: control.highlightedIndex === index
        background: Item {}
    }

    function recalcSize() {
        if (model) {
            for(var i = 0; i < model.length; i++) {
                textMetrics.text = Utils.limitText(control.textRole ? model[i][control.textRole] : model[i], control.textMaxLenDrop)
                var iconW = model[i]["iconWidth"] || 0
                modelWidth = Math.max(textMetrics.width +
                                      iconW +
                                      10 + // spacing between icon & text
                                      17,  // right padding
                                      modelWidth)
            }
        }
    }

    onModelChanged: recalcSize()

    indicator: SvgImage {
        source: "qrc:/assets/icon-down.svg"
        anchors.right: control.right
        anchors.verticalCenter: control.verticalCenter
        visible: control.enabled
    }

    property var iconW: (control.model && control.model[currentIndex] ? control.model[currentIndex]["iconWidth"] : 0) || 0
    property var iconH: (control.model && control.model[currentIndex] ? control.model[currentIndex]["iconHeight"] : 0) || 0
    property var iconS: (control.model && control.model[currentIndex] ? control.model[currentIndex]["icon"] : "") || ""

    contentItem: Row {
        spacing: 0

        SvgImage {
            source: iconS
            sourceSize: Qt.size(iconW, iconH)
            anchors.verticalCenter: parent.verticalCenter
            visible: iconW > 0
        }

        Item {
            visible: iconW > 0
            width:   10
            height:  parent.height
        }

        SFText  {
            clip: true
            text: control.editable ? control.editText : Utils.limitText(control.displayText, control.textMaxLenDisplay)
            color: control.enabled ? control.color : Style.content_secondary 
            font.pixelSize: fontPixelSize
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            width:  control.indicator.width + control.spacing
            height: parent.height
        }
    }

    background: Item {
        Rectangle {
            width:  control.width
            height: 1
            y: control.height - 1
            color: control.underlineColor
            opacity: (control.activeFocus || control.hovered)? 0.3 : 0.1
        }
    }

   popup: Popup {
        id: comboPopup
        onAboutToShow: recalcSize

        y: control.height + 7
        x: {
            if (iconW) return control.parent.mapToItem(parent, control.x, 0).x - comboPopup.leftPadding
            return control.parent.mapToItem(parent, control.x, 0).x + control.width / 2 - width / 2 + control.dropOffset
        }

        width: calculatedWidth + leftPadding + rightPadding

        topPadding:    20
        bottomPadding: 20
        leftPadding:   20
        rightPadding:  3

        contentItem: ColumnLayout {
            spacing: 0
            ListView {
                id: listView
                Layout.fillWidth: true
                clip: true
                spacing: control.dropSpacing
                implicitHeight: enableScroll ? Math.min(400, contentHeight) : contentHeight
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex
                ScrollBar.vertical: ScrollBar {
                    policy: enableScroll && listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
                }
            }
        }

        background: Rectangle {
            anchors.fill: parent
            color: Style.background_popup
        }
    }
}
