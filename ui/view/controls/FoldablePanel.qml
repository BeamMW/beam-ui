import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import "."

Pane {
    id: control

    property string title
    property string titleTip

    property bool  folded:            true
    property var   content:           null
    property var   headerContent:     null
    property alias titleOpacity:      headerTitle.opacity
    property int   contentItemHeight: 0

    spacing: 0
    padding: 20

    Component {
        id: foldClick

        Rectangle {
            color: "transparent"
            MouseArea {
                anchors.fill:    parent
                acceptedButtons: Qt.LeftButton
                cursorShape:     Qt.PointingHandCursor
                onClicked: {
                    control.folded = !control.folded;
                }
            }
        }
    }

    contentItem: Item {
        ColumnLayout {
            spacing: 0
            clip:    folded
            width: parent.width
            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                spacing: 0

                Loader {
                    width: parent.width
                    height: parent.height
                    sourceComponent: foldClick
                }

                RowLayout {
                    Layout.fillHeight: true
                    spacing: 0

                    SFText {
                        id:    headerTitle
                        text:  title
                        color: Style.content_main

                        font {
                            styleName:      "Bold"
                            weight:         Font.Bold
                            pixelSize:      14
                            letterSpacing:  3.11
                            capitalization: Font.AllUppercase
                        }

                        Loader {
                            anchors.fill: parent
                            sourceComponent: foldClick
                        }
                    }

                    Item {
                        width: 5
                        Layout.fillHeight: true
                        visible: titleTip.length != 0
                    }

                    SFText {
                        color:   Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 1)
                        text:    titleTip
                        visible: titleTip.length != 0

                        font {
                            styleName:      "Bold"
                            weight:         Font.Bold
                            pixelSize:      14
                            letterSpacing:  0.35
                        }

                        Loader {
                            anchors.fill: parent
                            sourceComponent: foldClick
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: !headerContent
                }

                Item {
                    width: 5
                    Layout.fillHeight: true
                }

                SvgImage {
                    Layout.alignment:       Qt.AlignCenter
                    Layout.maximumHeight:   8
                    Layout.maximumWidth:    13
                    source:                 control.folded ? "qrc:/assets/icon-grey-arrow-down.svg" : "qrc:/assets/icon-grey-arrow-up.svg"
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth:  true
                    visible: !!headerContent
                }

                Control {
                    id:  headerPlaceholder
                    visible: headerContent && !folded
                    contentItem: headerContent
                }
            }

            Control {
                id:                     placeholder
                Layout.fillWidth:       true
                Layout.topMargin:       folded ? 0 : 20
                Layout.alignment:       Qt.AlignTop
                contentItem:            control.content

                Layout.preferredHeight: folded ? 0 : (control.contentItemHeight ? control.contentItemHeight : placeholder.implicitHeight)
                opacity:                folded ? 0.0 : 1.0

                Behavior on Layout.preferredHeight {
                    NumberAnimation { duration:  100 }
                }
                Behavior on Layout.topMargin {
                    NumberAnimation { duration:  100 }
                }
                Behavior on opacity {
                    NumberAnimation { duration:  200 }
                }
            }
        }
    }

    background: Rectangle {
        radius:  10
        color:   Style.background_second
    }
}
