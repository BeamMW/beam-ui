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
        spacing:    0
        clip:       folded

        Item {
            Layout.fillWidth:  true
            Layout.alignment:  Qt.AlignTop
            Layout.leftMargin: 20
            height: header.height

            RowLayout {
                id:      header
                width:   parent.width
                spacing: 0

                ExternalNodeStatus {
                    id:               statusIndicator
                    Layout.alignment: Qt.AlignVCenter
                    visible:          hasStatusIndicatior
                }

                Item {
                    width: control.spacing
                    visible: hasStatusIndicatior
                }

                SFText {
                    text:  control.title
                    color: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.5)

                    font {
                        styleName:      "Bold"
                        weight:         Font.Bold
                        pixelSize:      14
                        letterSpacing:  3.11
                        capitalization: Font.AllUppercase
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                SvgImage {
                    id:     originalSizeImage
                    Layout.maximumHeight:   8
                    Layout.maximumWidth:    13
                    source: control.folded ? "qrc:/assets/icon-grey-arrow-down.svg" : "qrc:/assets/icon-grey-arrow-up.svg"
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
            implicitHeight:  errorRow.height
            Layout.leftMargin: 20
            Layout.topMargin: 3

            RowLayout {
                id: errorRow
                width: parent.width
                spacing: 0

                SFText {
                    Layout.fillWidth:  true
                    id:                statusErrorMsg
                    color:             Style.validator_error
                    font.pixelSize:    12
                    font.italic:       true
                    wrapMode:          Text.Wrap
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
            id:                     contentControl
            Layout.fillWidth:       true
            Layout.topMargin:       folded ? 0 : connectionError ? Math.max(5, 25 - errorRow.height) : 25
            Layout.leftMargin:      20
            Layout.alignment:       Qt.AlignTop
            contentItem:            content
            Layout.preferredHeight: folded ? 0 : contentControl.implicitHeight
            opacity:                folded ? 0.0 : 1.0

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration:  200 }
            }

            Behavior on Layout.topMargin {
                NumberAnimation { duration:  200 }
            }

            Behavior on opacity {
                NumberAnimation { duration:  200 }
            }
        }
    }

    background: Rectangle {
        radius:  10
        color:   Style.background_second

        MouseArea {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: control.topPadding + header.height +
                    ( control.folded ? control.bottomPadding : 0 ) +
                    ( errorRow.visible ? errorRow.height : 0 ) +
                    ( contentControl.visible ? contentControl.Layout.topMargin : 0 )

            cursorShape:  Qt.PointingHandCursor
            onClicked: {
                control.folded = !control.folded;
            }
        }
    }

    leftPadding:   0
    rightPadding:  22
    topPadding:    20
    bottomPadding: connectionError && control.folded ? 16 : 20
}
