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
    padding: 8
    leftPadding:    16
    rightPadding:   16
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
    property bool filterAssets: false

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
        required property var model
        required property int index

        width: calculatedWidth
        padding: 0
        leftPadding: control.leftPadding
        bottomPadding: control.dropSpacing
        topPadding: 2

        property var myModel : itemDelegate.model

        property var  iconW:    myModel["iconWidth"] || 0
        property var  iconH:    myModel["iconHeight"] || 0
        property var  iconS:    myModel["icon"] || ""
        property bool verified: myModel["verified"] || false

        contentItem: RowLayout {
            id: contentRow
            spacing: 0
            property int parentHeight: 0
            property bool showRow: control.filterAssets
                ? containSearchSubStr(myModel[control.textRole]) && myModel["allowed"]
                : containSearchSubStr(myModel[control.textRole])

            visible: showRow
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
                    var text = model
                    if (control.textRole) {
                        text = myModel[control.textRole]
                    }
                    return (transformText && typeof(transformText) == "function")
                        ? transformText(text)
                        : text;
                }
                color: highlighted ? Style.active : Style.content_main
                elide: Text.ElideMiddle
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

    indicator: Item {}

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
                model: control.popup.visible ? control.delegateModel : null
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
