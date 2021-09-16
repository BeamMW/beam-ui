import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import "../utils.js" as Utils
import "../controls"

ColumnLayout {
    id: control
    property var  appsList
    property bool hasLocal

    onAppsListChanged: function() {
        if (appsList) {
            for (let idx = 0; idx < 2; ++idx) {
                if (appsList[idx].local) {
                    hasLocal = true
                    return
                }
            }
        }
        hasLocal = false
    }

    spacing: 10
    signal launch(var app)
    signal install()
    signal uninstall(var app)

    // Actuall apps list
    ScrollView {
        Layout.fillHeight: true
        Layout.fillWidth:  true

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        clip: true

        ColumnLayout
        {
            width: parent.width
            spacing: 15

            Repeater {
                model: control.appsList

                delegate: Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100

                    Rectangle {
                        anchors.fill: parent
                        radius:       10
                        color:        Style.active
                        opacity:      hoverArea.containsMouse ? 0.15 : 0.1
                    }

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Rectangle {
                            Layout.leftMargin: 30
                            width:  60
                            height: 60
                            radius: 30
                            color:  Style.background_main

                            SvgImage {
                                id: defaultIcon
                                source: hoverArea.containsMouse ? "qrc:/assets/icon-defapp-active.svg" : "qrc:/assets/icon-defapp.svg"
                                sourceSize: Qt.size(30, 30)
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                visible: !customIcon.visible
                            }

                            SvgImage {
                                id: customIcon
                                source: modelData.icon ? modelData.icon : "qrc:/assets/icon-defapp.svg"
                                sourceSize: Qt.size(30, 30)
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                visible: !!modelData.icon && progress == 1.0
                            }
                        }

                        Column {
                            Layout.leftMargin: 20
                            spacing: 10

                            SFText {
                                text: modelData.name
                                font {
                                    styleName:  "DemiBold"
                                    weight:     Font.DemiBold
                                    pixelSize:  18
                                }
                                color: Style.content_main
                            }

                            SFText {
                                text: Utils.limitText(modelData.description, 80)
                                font.pixelSize:  14
                                elide: Text.ElideRight
                                color: Style.content_main
                                visible: modelData.supported
                            }

                            SFText {
                                elide: Text.ElideRight
                                color: Style.validator_error
                                visible: !modelData.supported
                                font.italic: true
                                //% "This DApp requires version %1 of Beam Wallet or higher. Please update your wallet."
                                text: qsTrId("apps-version-error").arg(modelData.min_api_version || modelData.api_version)
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        CustomButton {
                            Layout.rightMargin: control.hasLocal ? 0 : 20
                            height: 40
                            palette.button: Style.background_button
                            palette.buttonText : Style.content_main
                            icon.source: "qrc:/assets/icon-run.svg"
                            icon.height: 16
                            visible: modelData.supported
                            //% "launch"
                            text: qsTrId("apps-run")

                            MouseArea {
                                anchors.fill:     parent
                                acceptedButtons:  Qt.LeftButton
                                hoverEnabled:     true
                                propagateComposedEvents: true
                                preventStealing:  true
                                onClicked:        control.launch(modelData)
                            }
                        }

                        CustomToolButton {
                            Layout.fillHeight: true
                            Layout.rightMargin: 10
                            width: 36

                            opacity: modelData.local ? 0.5 : 0
                            icon.source: "qrc:/assets/icon-actions.svg"
                            visible: control.hasLocal && (modelData.supported || !!modelData.local)

                            //% "Actions"
                            ToolTip.text: qsTrId("general-actions")
                            MouseArea {
                                anchors.fill:     parent
                                acceptedButtons:  Qt.LeftButton
                                hoverEnabled:     true
                                propagateComposedEvents: true
                                preventStealing:  true
                                onClicked: function () {
                                    if (modelData.local) {
                                        appMenu.popup()
                                    }
                                }
                            }
                        }
                    }

                    ContextMenu {
                        id:    appMenu
                        modal: true
                        dim:   false

                        Action {
                            //% "Uninstall"
                            text: qsTrId("apps-uninstall")
                            icon.source: "qrc:/assets/icon-delete.svg"
                            onTriggered: function () {
                                confirmUninstall.open()
                            }
                        }
                    }

                    ConfirmationDialog {
                        id: confirmUninstall
                        width: 460
                        //% "Uninstall DApp"
                        title: qsTrId("app-uninstall-title")
                        //% "Are you sure you want to uninstall %1 DApp?"
                        text: qsTrId("apps-uninstall-confirm").arg(modelData.name)
                        //% "Uninstall"
                        okButtonText: qsTrId("apps-uninstall")
                        okButtonIconSource: "qrc:/assets/icon-delete.svg"
                        okButtonColor: Style.accent_fail
                        cancelButtonIconSource: "qrc:/assets/icon-cancel.svg"

                        onAccepted: function () {
                            control.uninstall(modelData)
                        }
                    }

                    MouseArea {
                        id:               hoverArea
                        anchors.fill:     parent
                        hoverEnabled:     true
                        propagateComposedEvents: true
                        preventStealing: true
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                opacity: 0.3

                Canvas {
                    anchors.fill: parent
                    antialiasing: true

                    onPaint: function (rect) {
                        var radius = 10
                        var ctx = getContext("2d")
                        ctx.save()
                        ctx.setLineDash([5, 5])
                        ctx.beginPath()
                            ctx.moveTo(0,0)
                            ctx.lineTo(rect.width, 0)
                            ctx.lineTo(rect.width, rect.height)
                            ctx.lineTo(0, rect.height)
                            ctx.lineTo(0, 0)
                            ctx.closePath()
                        ctx.strokeStyle = "#1af6d6"
                        ctx.stroke()
                        ctx.restore()
                     }
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 6

                    SvgImage {
                        Layout.alignment:   Qt.AlignVCenter
                        Layout.leftMargin:  20
                        source:             "qrc:/assets/icon-add-green.svg"
                        sourceSize:         Qt.size(18, 18)
                    }

                    SFText {
                        Layout.alignment:    Qt.AlignVCenter
                        Layout.fillWidth:    true
                        font {
                            styleName: "Normal"
                            weight:    Font.Normal
                            pixelSize: 14
                        }
                        color: Style.active
                        wrapMode: Text.WordWrap
                        //% "Install DApp from file"
                        text: qsTrId("apps-install-from-file")
                    }
                }

                MouseArea {
                    id:               clickArea
                    anchors.fill:     parent
                    acceptedButtons:  Qt.LeftButton
                    hoverEnabled:     true
                    cursorShape:      Qt.PointingHandCursor
                    onClicked:        control.install()
                }
            }
        }
    }
}