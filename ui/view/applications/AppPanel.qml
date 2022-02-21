import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"

Item {
    id: control

    property var app

    signal launch(var app)
    signal install(var fname)
    signal update(var app)
    signal uninstall(var app)

    // background
    Rectangle {
        anchors.fill: parent
        radius:       10
        color:        Style.active
        opacity:      hoverArea.containsMouse ? 0.15 : 0.1
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 20
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.bottomMargin: 16
        spacing: 20

        // icon
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 50
            Rectangle {
                width:  50
                height: 50
                radius: 25
                color:  Style.background_main

                SvgImage {
                    id: defaultIcon
                    source: hoverArea.containsMouse ? "qrc:/assets/icon-defapp-active.svg" : "qrc:/assets/icon-defapp.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: !customIcon.visible
                }

                SvgImage {
                    id: customIcon
                    source: app.icon ? app.icon : "qrc:/assets/icon-defapp.svg"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: !!app.icon && progress == 1.0
                }
            }
        }
                    
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 13
            RowLayout {
                Layout.fillWidth: true
                spacing: 0

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    SFText {
                        text: app.name
                        font {
                            styleName:  "DemiBold"
                            weight:     Font.DemiBold
                            pixelSize:  16
                        }
                        color: Style.content_main
                    }

                    SFText {
                        Layout.maximumWidth:  284
                        text: app.description
                        font.pixelSize:  14
                        elide: Text.ElideRight
                        color: Style.content_main
                        visible: app.supported
                        maximumLineCount: 2
                        wrapMode: Text.Wrap
                    }

                    // TODO: find place in new design
                    SFText {
                        elide: Text.ElideRight
                        color: Style.validator_error
                        visible: !app.supported
                        font.italic: true
                        //% "This DApp requires version %1 of Beam Wallet or higher. Please update your wallet."
                        text: qsTrId("apps-version-error").arg(app.min_api_version || app.api_version)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                SFText {
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    text: "category" // app.category
                    font.pixelSize:  14
                    elide: Text.ElideRight
                    // TODO: get color for the category
                    color: "#FF57BF"
                    // visible: !!app.category
                }

            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom
                spacing: 0

                // TODO: implement "Publisher name" + publisher pubKey
                SFText {
                    Layout.alignment: Qt.AlignTop
                    text: "Publisher name" // app.publisherName ?
                    font.pixelSize:  12
                    elide: Text.ElideRight
                    color: Style.content_secondary
                }

                Item {
                    Layout.fillWidth: true
                }

                CustomButton {
                    Layout.alignment: Qt.AlignTop | Qt.ALignRight
                    palette.button: Style.background_button
                    palette.buttonText : Style.content_main
                    icon.source: "qrc:/assets/icon-run.svg"
                    icon.height: 16
                    visible: app.supported && !!app.notInstalled
                    //% "Install"
                    text: qsTrId("dapps-store-install")

                    MouseArea {
                        anchors.fill:     parent
                        acceptedButtons:  Qt.LeftButton
                        hoverEnabled:     true
                        propagateComposedEvents: true
                        onClicked:        control.install(modelData)
                    }
                }

                // TODO: change buttons to new design

                CustomButton {
                    Layout.alignment: Qt.AlignTop
                    palette.button: Style.background_button
                    palette.buttonText : Style.content_main
                    icon.source: "qrc:/assets/icon-run.svg"
                    icon.height: 16
                    visible: app.supported && !app.notInstalled && !!app.hasUpdate
                    //% "Update"
                    text: qsTrId("dapps-store-update")

                    MouseArea {
                        anchors.fill:     parent
                        acceptedButtons:  Qt.LeftButton
                        hoverEnabled:     true
                        propagateComposedEvents: true
                        onClicked:        control.update(modelData)
                    }
                }

                CustomButton {
                    Layout.alignment: Qt.AlignTop | Qt.ALignRight
                    palette.button: Style.background_button
                    palette.buttonText : Style.content_main
                    icon.source: "qrc:/assets/icon-run.svg"
                    icon.height: 16
                    visible: app.supported && !app.notInstalled
                    //% "launch"
                    text: qsTrId("apps-run")

                    MouseArea {
                        anchors.fill:     parent
                        acceptedButtons:  Qt.LeftButton
                        hoverEnabled:     true
                        propagateComposedEvents: true
                        onClicked:        control.launch(modelData)
                    }
                }

                CustomToolButton {
                    Layout.alignment: Qt.AlignTop
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 10
                    width: 36

                    opacity: app.local ? 0.5 : 0
                    icon.source: "qrc:/assets/icon-actions.svg"
                    visible: app.supported && !!app.local

                    //% "Actions"
                    ToolTip.text: qsTrId("general-actions")
                    MouseArea {
                        anchors.fill:     parent
                        acceptedButtons:  Qt.LeftButton
                        hoverEnabled:     true
                        propagateComposedEvents: true
                        onClicked: function () {
                            if (app.local) {
                                appMenu.popup()
                            }
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        id:            hoverArea
        anchors.fill:  parent
        hoverEnabled:  true
        propagateComposedEvents: true
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
        text: qsTrId("apps-uninstall-confirm").arg(app.name)
        //% "Uninstall"
        okButtonText: qsTrId("apps-uninstall")
        okButtonIconSource: "qrc:/assets/icon-delete.svg"
        okButtonColor: Style.accent_fail
        cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"

        onAccepted: function () {
            control.uninstall(modelData)
        }
    }
}