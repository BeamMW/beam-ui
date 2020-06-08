import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."

Control {
    id: control
    Layout.fillWidth: true

    property string title
    property bool   folded: true
    property var    content: null
    spacing: 10

    // Status indicator
    property bool   hasStatusIndicatior: false
    property alias  connectionStatus: statusIndicator.status
    property alias  connectionErrorMsg: statusErrorMsg.text
    property bool   connectionError: connectionStatus == "error"

    contentItem: ColumnLayout {
        spacing: 0
        clip:    true

        Item {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            height: header.height

            RowLayout {
                id:      header
                width:   parent.width
                spacing: 0

                Item {
                    width:   statusIndicator.radius
                    visible: hasStatusIndicatior
                }

                ExternalNodeStatus {
                    id:               statusIndicator
                    Layout.alignment: Qt.AlignVCenter
                    visible:          hasStatusIndicatior
                }

                Item {
                    width: control.spacing
                    visible: hasStatusIndicatior
                }

                ColumnLayout
                {
                    SFText {
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment:   Text.AlignVCenter
                        text:  control.title
                        color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)
                        Layout.fillWidth: true

                        font {
                            styleName:      "Medium"
                            weight:         Font.Medium
                            pixelSize:      14
                            letterSpacing:  3.11
                            capitalization: Font.AllUppercase
                        }
                    }
                }

                SFText {
                    text: "^"
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment:   Text.AlignVCenter
                    rotation: control.folded ? "180" : 0
                    color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)

                    font {
                        styleName:      "Medium"
                        weight:         Font.Medium
                        pixelSize:      14
                        letterSpacing:  3.11
                        capitalization: Font.AllUppercase
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                cursorShape:  Qt.PointingHandCursor
                onClicked: {
                    control.folded = !control.folded;
                }
            }
        }

        Item {
            Layout.fillWidth:  true
            visible: connectionError
            height:  errorRow.height

            RowLayout {
                id: errorRow
                width: parent.width
                spacing: 0

                Item {
                    width: statusIndicator.radius + statusIndicator.width + control.spacing
                }

                SFText {
                    Layout.fillWidth:  true
                    id:                statusErrorMsg
                    color:             Style.validator_error
                    font.pixelSize:    12
                    font.italic:       true
                }
            }

            MouseArea {
                anchors.fill: parent
                cursorShape:  Qt.PointingHandCursor
                onClicked: {
                    control.folded = !control.folded;
                }
            }
        }

        Control {
            visible:           !control.folded
            Layout.fillWidth:  true
            Layout.topMargin:  connectionError ? 25 - errorRow.height : 25
            Layout.alignment:  Qt.AlignTop
            contentItem:       content
        }
    }

    background: Rectangle {
        radius:  10
        color:   Style.background_second
    }

    leftPadding:   20
    rightPadding:  22
    topPadding:    20
    bottomPadding: connectionError && control.folded ? 16 : 20
}
