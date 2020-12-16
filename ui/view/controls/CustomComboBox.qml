import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import "."

ComboBox {
    id: control
    
    spacing: 8
    property int fontPixelSize: 12
    property double fontLetterSpacing: 0
    property string color: Style.content_main
    property string underlineColor: color
    property bool enableScroll: false

    property var modelWidth: control.width
    property var calculatedWidth: Math.max(control.width, modelWidth)

    TextMetrics {
        id: textMetrics
        font {
            family:        "SF Pro Display"
		    styleName:     "Regular"
		    weight:        Font.Normal
            pixelSize:     control.fontPixelSize
            letterSpacing: control.fontLetterSpacing
        }
    }

    delegate: ItemDelegate {
        id: itemDelegate
        width: calculatedWidth

        property var iconW: Array.isArray(control.model) ? modelData["iconWidth"] : model["iconWidth"]
        property var iconH: Array.isArray(control.model) ? modelData["iconHeight"] : model["iconHeight"]
        property var iconS: (Array.isArray(control.model) ? modelData["icon"] : model["icon"]) || ""

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
                width: iconW / 2.5
                height: parent.height
            }
            SFText {
                text: {
                    if (control.textRole) {
                        return Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]
                    }
                    return modelData
                }
                color: Style.content_main
                elide: Text.ElideMiddle
                font.pixelSize: fontPixelSize
                font.letterSpacing: fontLetterSpacing
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        highlighted: control.highlightedIndex === index

        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 20
            opacity: enabled ? 1 : 0.3
            color:itemDelegate.highlighted ? Style.content_secondary : Style.background_details
        }
    }

    ItemDelegate {
        id: forCalc
        visible: false
    }

    onModelChanged: {
        if (model) {
            for(var i = 0; i < model.length; i++){
                textMetrics.text = control.textRole ? model[i][control.textRole] : model[i]
                var iconW = model[i]["iconWidth"] || 0
                modelWidth = Math.max(textMetrics.width +
                                      forCalc.leftPadding +
                                      forCalc.rightPadding +
                                      iconW +
                                      iconW / 2.5, modelWidth)
            }
        }
    }

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
            width: iconW / 4
            height: parent.height
        }

        SFText  {
            clip: true
            text: control.editable ? control.editText : control.displayText
            color: control.enabled ? control.color : Style.content_secondary 
            font.pixelSize: fontPixelSize
            anchors.verticalCenter: parent.verticalCenter
        }
        Item {
            width: control.indicator.width + control.spacing
            height: parent.height
        }
    }

    background: Item {
        Rectangle {
            width: control.width
            height: control.activeFocus || control.hovered ? 1 : 1
            y: control.height - 1
            color: control.underlineColor
            opacity: (control.activeFocus || control.hovered)? 0.3 : 0.1
        }
    }

    popup: Popup {
        y: control.height - 1
        width: calculatedWidth
        padding: 1

        contentItem: ColumnLayout {
            spacing: 0
            ListView {
                id: listView
                Layout.fillWidth: true
                clip: true
                implicitHeight: enableScroll ? 250 : contentHeight
                model: control.popup.visible ? control.delegateModel : null
                currentIndex: control.highlightedIndex
                ScrollIndicator.vertical: ScrollIndicator { }
            }
            Item {
                Layout.fillWidth: true
                Layout.minimumHeight:10
            }
        }

        background: Item {
            Rectangle {
                color: Style.background_details
                anchors.left: parent.left
                anchors.right: parent.right
                height: control.height
            }
            Rectangle {
                anchors.fill: parent
                color: Style.background_details
                radius: 10
            }
        }
    }
}
