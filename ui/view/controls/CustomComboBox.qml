import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.impl 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12
import "."
import "../utils.js" as Utils

ComboBox {
    id: control
    
    spacing: 8
    padding: 6
    leftPadding: 8
    property int dropSpacing: 20
    property int fontPixelSize: 12
    property int dropFontPixelSize: 13
    property double fontLetterSpacing: 0
    property string color: Style.content_main
    property bool colorConst: false
    property string underlineColor: color
    property bool enableScroll: false
    property int textMaxLenDrop: 0

    property var modelWidth: 140
    property var calculatedWidth: Math.max(control.width, modelWidth)
    property var transformText: undefined

    property var controlFont: Font.Normal
    property string controlFontStyle: "Regular"
    property color controlColor: control.enabled || control.colorConst ? control.color : Style.content_secondary
    property bool showBackground: true

    property int maxTextWidth: 400 
    property var dropDownIconSixe: Qt.size(5, 3)
    property int dropDownIconRightMargin: 0

    property alias backgroundColor : backgroundRect.color
    backgroundColor: Style.content_main
    property string searchPlaseholder: ""
    property alias searchText: searchInput.text

    TextMetrics {
        id: textMetrics
        font {
            family:        "Proxima Nova"
            styleName:     "Regular"
            weight:        Font.Normal
            pixelSize:     control.dropFontPixelSize
            letterSpacing: control.fontLetterSpacing
        }
    }

    property var containSearchSubStr: function(text) {
        if (!control.searchText.length) return true;
        return text.toLowerCase().includes(control.searchText.toLowerCase());
    }

    delegate: ItemDelegate {
        id: itemDelegate
        width: calculatedWidth
        padding: 0
        leftPadding: control.leftPadding
        bottomPadding: control.dropSpacing
        topPadding: 2

        property var  iconW:    (Array.isArray(control.model)  ? modelData["iconWidth"]  : model["iconWidth"]) || 0
        property var  iconH:    (Array.isArray(control.model)  ? modelData["iconHeight"] : model["iconHeight"]) || 0
        property var  iconS:    (Array.isArray(control.model)  ? modelData["icon"]       : model["icon"]) || ""
        property bool verified: (Array.isArray(control.model)  ? modelData["verified"]   : model["verified"]) || false

        contentItem: RowLayout {
            id: contentRow
            spacing: 0
            property int parentHeight: 0

            visible: (Array.isArray(control.model) ? containSearchSubStr(modelData[control.textRole]) && modelData["allowed"] : containSearchSubStr(model[control.textRole]) && model["allowed"])
            onVisibleChanged: {
                parent.height = visible ? parentHeight : 0;
            }

            SvgImage {
                source: iconS
                visible: iconW > 0
                Layout.alignment: Qt.AlignVCenter
                Layout.rightMargin: 10
                Layout.preferredWidth: iconW
                Layout.preferredHeight: iconH

                SvgImage {
                    source: "qrc:/assets/icon-verified-asset.svg";
                    visible: verified

                    x: parent.width - width / 1.6
                    y: - height / 3.6

                    width:  18 * parent.width / 26
                    height: 18 * parent.width / 26
                }
            }

            SFText {
                id: textLabel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: {
                    var text = modelData
                    if (control.textRole) {
                        text = Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]
                    }
                    return (transformText && typeof(transformText) == "function")
                        ? transformText(text)
                        : text;
                }
                color: highlighted ? Style.active : Style.content_main
                elide: Text.ElideRight
                font.pixelSize: dropFontPixelSize
                font.letterSpacing: fontLetterSpacing
                font.styleName: highlighted ? "DemiBold" : "Normal"
                font.weight: highlighted ? Font.DemiBold : Font.Normal
            }

            Component.onCompleted: {
                contentRow.parentHeight = contentRow.parent.height
            }
        }

        highlighted: control.highlightedIndex === index
        background: Item {}
    }

    function recalcSize() {
        if (model) {
            for(var i = 0; i < model.length; i++) {
                var modelText = control.textRole ? model[i][control.textRole] : model[i];
                if (transformText && typeof(transformText) == "function")
                    modelText = transformText(modelText);
                textMetrics.text = Utils.limitText(modelText, control.textMaxLenDrop)

                var iconW = model[i]["iconWidth"] || 0
                modelWidth = Math.max(textMetrics.width +
                                      iconW + 10, // spacing between icon & text
                                      modelWidth)
            }
        }
    }

    onModelChanged: recalcSize()
    indicator: Item {}
    onDownChanged: {
        recalcSize();
    }

    property var  iconW:    (control.model && control.model[currentIndex] ? control.model[currentIndex]["iconWidth"] : 0) || 0
    property var  iconH:    (control.model && control.model[currentIndex] ? control.model[currentIndex]["iconHeight"] : 0) || 0
    property var  iconS:    (control.model && control.model[currentIndex] ? control.model[currentIndex]["icon"] : "") || ""
    property bool verified: (control.model && control.model[currentIndex] ? control.model[currentIndex]["verified"] : false) || false

    contentItem: RowLayout {
        spacing: 0

        SvgImage {
            source: iconS
            sourceSize: Qt.size(iconW, iconH)
            visible: iconW > 0
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: 10

            SvgImage {
                source: "qrc:/assets/icon-verified-asset.svg";
                visible: verified

                x: parent.width - width / 1.6
                y: - height / 3.6

                width:  18 * parent.width / 26
                height: 18 * parent.width / 26
            }
        }

        SFText  {
            Layout.fillWidth:   true
            Layout.alignment:   Qt.AlignVCenter
            Layout.maximumWidth: control.maxTextWidth
            Layout.rightMargin: control.enabled ? 10 : 0

            clip: true
            text: control.editable ? control.editText : control.displayText
            color: control.controlColor
            font.pixelSize: fontPixelSize
            font.weight: control.controlFont
            font.styleName: control.controlFontStyle
            elide: Text.ElideRight
        }

        SvgImage {
            id: imgDown
            source: "qrc:/assets/icon-down.svg"
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: control.dropDownIconRightMargin
            visible: control.enabled && !control.down
            sourceSize: control.dropDownIconSixe
        }
        SvgImage {
            id: imgUp
            source: "qrc:/assets/icon-up.svg"
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: control.dropDownIconRightMargin
            visible: control.enabled && control.down
            sourceSize: control.dropDownIconSixe
        }
    }

    background: Item {
        Rectangle {
            id: backgroundRect
            visible: control.showBackground
            color: control.underlineColor
            opacity: (control.activeFocus || control.hovered)? 0.3 : 0.1
            anchors.fill: parent
            radius: 10
        }
    }

   popup: Popup {
        id: comboPopup
        onAboutToShow: recalcSize

        y: control.height + 7
        x: {
            if (iconW) return control.parent.mapToItem(parent, control.x, 0).x - comboPopup.leftPadding
            return control.parent.mapToItem(parent, control.x, 0).x + control.width / 2 - width / 2
        }

        width: calculatedWidth + leftPadding + rightPadding

        topPadding:    20
        bottomPadding: 20
        leftPadding:   0
        rightPadding:  0

        contentItem: ColumnLayout {
            spacing: 0
            SearchBox {
               id: searchInput
               Layout.fillWidth:     true
               Layout.bottomMargin: 15
               Layout.rightMargin:  15
               Layout.leftMargin:   15
               visible:             control.delegateModel.count > 12
               alwaysVisibleInput:  true
               placeholderText:     searchPlaseholder
            }
            ListView {
                id: listView
                Layout.fillWidth: true
                clip: true
                implicitHeight: control.delegateModel.count > 12 ? Math.min(250, contentHeight) : contentHeight
                model: control.delegateModel
                currentIndex: control.highlightedIndex
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
            }
        }

        background: Rectangle {
            anchors.fill: parent
            color: Style.background_popup
            radius: 5
        }
    }
}
