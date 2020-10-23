import QtQuick 2.11
import QtQuick.Layouts 1.3
import "../utils.js" as Utils
import Beam.Wallet 1.0
import "."

Rectangle {
    property color borderColor: Style.currencyPaneBorder
    property int borderSize: 0
    property color gradLeft: Style.currencyPaneLeftBEAM
    property color gradRight: Style.currencyPaneRight
    property string currencyIcon: ""
    property var currencyIcons: []
    property color stateIndicatorColor: Style.swapCurrencyStateIndicator
    property string amount: ""
    property string unitName: ""
    property string valueSecondaryStr: ""
    property bool isOk: true
    property bool isConnecting: false
    property int textSize: 16
    property int textSecondarySize: 12
    property color textColor: Style.content_main
    property color textSecondaryColor: Style.content_secondary
    property string textConnectionError: "error"
    property string textConnecting: "connectring..."
    property alias amountWrapMode: amountField.wrapMode
    property var onClick: function() {}
    property string swapSettingsPane: ""
   
    id: control
    Layout.fillWidth: true
    height: 67
    color: "transparent"

   PanelGradient {
        id: rect

        leftColor:  control.gradLeft
        rightColor: control.gradRight
        anchors.fill: parent

        borderWidth:      control.borderSize
        leftBorderColor:  control.borderColor
        rightBorderColor: control.borderColor
   }

    Item {
        anchors.fill: parent

        Item {
            id: currencyLogo
            width: childrenRect.width
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: {
                left: 20
            }
            SvgImage {
                anchors.verticalCenter: parent.verticalCenter
                source: currencyIcon
                visible: currencyIcon.length
            }

            Repeater {
                model: currencyIcons.length
                visible: currencyIcons.length
                
                SvgImage {
                    anchors.verticalCenter: parent.verticalCenter
                    x: parent.x + index * 15 - 20
                    source: currencyIcons[index]
                }
            }
        }

        Column {
            anchors.left: currencyLogo.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: isOk
            RowLayout{
                anchors.left: parent.left
                anchors.right: parent.right
                Item {width:15}
                spacing: 0
                SFLabel {
                    id: amountField
                    Layout.fillWidth: true
                    font.pixelSize: textSize
                    color: control.textColor
                    elide: Text.ElideRight
                    text: unitName.length ? [Utils.uiStringToLocale(amount), unitName].join(" ") : Utils.uiStringToLocale(amount)
                    fontSizeMode: Text.Fit
                    visible: amount.length
                    verticalAlignment: Text.AlignVCenter
                    copyMenuEnabled: unitName.length
                    onCopyText: BeamGlobals.copyToClipboard(amount)
                }
                Item {width:20}
            }
            SFText {
                anchors.left: parent.left
                anchors.right: parent.right
                leftPadding: 15
                rightPadding: 20
                font.pixelSize: textSecondarySize
                color: textSecondaryColor
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                text: valueSecondaryStr
                visible: valueSecondaryStr.length
            }
        }

        SFText {
            id: connectionError
            anchors.left: currencyLogo.right
            anchors.right: connectionErrorIndicator.left
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 10
            rightPadding: 10
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
            color: Style.validator_error
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            text: textConnectionError
            visible: !isOk && !isConnecting

             MouseArea {
                id:                clickArea
                anchors.fill:      parent
                acceptedButtons:   Qt.LeftButton
                onClicked:         main.openSwapSettings(swapSettingsPane)
                hoverEnabled:      true
                onPositionChanged: clickArea.cursorShape = Qt.PointingHandCursor;
            }
        }

        SFText {
            id: connecting
            anchors.left: currencyLogo.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 10
            rightPadding: 10
            font.pixelSize: 12
            verticalAlignment: Text.AlignVCenter
            color: textColor
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            text: textConnecting
            visible: isConnecting
        }

        Item {
            id: connectionErrorIndicator
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: {
                right: 15
            }
            width: childrenRect.width
            visible: !isOk && !isConnecting

            property int radius: 5

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left

                width: parent.radius * 2
                height: parent.radius * 2
                radius: parent.radius
                color: stateIndicatorColor
            }
        }
    }
}
