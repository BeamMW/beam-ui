import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"

Item {
    id:             control
    implicitHeight: 144
    implicitWidth:  440

    property var app
    property bool showButtons:          true
    property bool isPublisherAdminMode: false

    readonly property int textWidth: 200

    signal launch(var app)
    signal install(var app)
    signal update(var app)
    signal uninstall(var app)
    signal remove(var app)

    // background
    Rectangle {
        anchors.fill: parent
        radius:       10
        color:        Style.active
        opacity:      hoverArea.containsMouse ? 0.15 : 0.1
    }

    RowLayout {
        anchors.fill:         parent
        anchors.topMargin:    20
        anchors.leftMargin:   20
        anchors.rightMargin:  20
        anchors.bottomMargin: 16
        spacing:              20

        // icon
        RowLayout {
            Layout.alignment:      Qt.AlignTop
            Layout.preferredWidth: 50
            Rectangle {
                width:  50
                height: 50
                radius: 25
                color:  Style.background_main

                SvgImage {
                    id:                       defaultIcon
                    source:                   hoverArea.containsMouse ? "qrc:/assets/icon-defapp-active.svg" : "qrc:/assets/icon-defapp.svg"
                    anchors.verticalCenter:   parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible:                  !customIcon.visible
                }

                SvgImage {
                    id:                       customIcon
                    source:                   app.icon ? app.icon : "qrc:/assets/icon-defapp.svg"
                    anchors.verticalCenter:   parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible:                  !!app.icon && progress == 1.0
                }
            }
        }
                    
        ColumnLayout {
            Layout.fillWidth: true
            spacing:          13
            RowLayout {
                Layout.fillWidth: true
                spacing:          0

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing:          4

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
                        Layout.maximumWidth: 284
                        text:                app.description
                        font.pixelSize:      14
                        elide:               Text.ElideRight
                        color:               Style.content_main
                        visible:             app.supported
                        maximumLineCount:    2
                        wrapMode:            Text.Wrap
                    }

                    // TODO: find place in new design
                    SFText {
                        elide:       Text.ElideRight
                        color:       Style.validator_error
                        visible:     !app.supported
                        font.italic: true
                                     //% "This DApp requires version %1 of Beam Wallet or higher. Please update your wallet."
                        text:        qsTrId("apps-version-error").arg(app.min_api_version || app.api_version)
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                SFText {
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    text:             !!app.category ? app.category : "" 
                    font.pixelSize:   14
                    elide:            Text.ElideRight
                    // TODO: get color for the category
                    color:            !!app.categoryColor ? app.categoryColor : "#FF57BF"
                    visible:          !!app.category
                }
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom
                spacing:          0

                // TODO: implement "Publisher name" + publisher pubKey
                ColumnLayout {
                    SFText {
                        text:             !!app.publisherName ? app.publisherName : ""
                        font.pixelSize:   12
                        elide:            Text.ElideRight
                        Layout.preferredWidth: control.textWidth
                        color:            Style.content_secondary
                    }

                    SFText {
                        text:             !!app.publisher ? app.publisher : ""
                        font.pixelSize:   12
                        elide:            Text.ElideMiddle
                        color:            Style.content_secondary
                        Layout.preferredWidth: control.textWidth
                        visible: !!app.publisher
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                AppButton {
                    id: button
                    Layout.alignment:   Qt.AlignTop | Qt.ALignRight
                    icon.height:        16
                    visible: showButtons
                    
                    onClicked: {
                        if (isPublisherAdminMode) {
                            control.update(modelData)
                        } else {
                            if (!!app.notInstalled) 
                                control.install(modelData)
                            else if (!app.notInstalled && !!app.hasUpdate)
                                control.update(modelData)
                            else if (!app.notInstalled)
                                control.launch(modelData)
                        }
                    }
                    
                    onClickedByAdditional: {
                        appMenu.popup(button, button.width - appMenu.width, button.height);
                    }
                }
            }
        }
    }

    MouseArea {
        id:                      hoverArea
        anchors.fill:            parent
        hoverEnabled:            true
        propagateComposedEvents: true
    }

    ContextMenu {
        id:    appMenu
        modal: true
        dim:   false
    }

    Action {
        id:          uninstallAction
                     //% "Uninstall"
        text:        qsTrId("apps-uninstall")
        icon.source: "qrc:/assets/icon-delete.svg"
        onTriggered: function () {
            confirmUninstall.open()
        }
    }

    Action {
        id:          removeAction
                     //% "remove dapp"
        text:        qsTrId("dapps-store-remove-dapp")
        icon.source: "qrc:/assets/icon-delete.svg"
        onTriggered: function () {
            control.remove(modelData)
        }
    }

    ConfirmationDialog {
        id:                     confirmUninstall
        width:                  460
                                //% "Uninstall DApp"
        title:                  qsTrId("app-uninstall-title")
                                //% "Are you sure you want to uninstall %1 DApp?"
        text:                   qsTrId("apps-uninstall-confirm").arg(app.name)
                                //% "Uninstall"
        okButtonText:           qsTrId("apps-uninstall")
        okButtonIconSource:     "qrc:/assets/icon-delete.svg"
        okButtonColor:          Style.accent_fail
        cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"

        onAccepted: function () {
            control.uninstall(modelData)
        }
    }

    function getButtonText() {
        if (app.supported) {
            if (isPublisherAdminMode) {
                //% "update"
                return qsTrId("dapps-store-update")
            } else {
                if (!!app.notInstalled)
                    //% "install"
                    return qsTrId("dapps-store-install")
                if (!app.notInstalled && !!app.hasUpdate)
                    //% "update"
                    return qsTrId("dapps-store-update")
                if (!app.notInstalled)
                    //% "launch"
                    return qsTrId("dapps-store-launch")
            }
        }
        return ""
    }

    function getButtonSource() {
        "qrc:/assets/icon-dapps-store-install.svg"
        if (isPublisherAdminMode) {
            return "qrc:/assets/icon-dapps-store-update.svg"
        } else {
            if (!!app.notInstalled)
                return "qrc:/assets/icon-dapps-store-install.svg"
            if (!app.notInstalled && !!app.hasUpdate)
                return "qrc:/assets/icon-dapps-store-update.svg"
            if (!app.notInstalled)
                return "qrc:/assets/icon-dapps-store-launch.svg"
        }
        return "";
    }

    Component.onCompleted: {
        button.text = getButtonText();
        button.icon.source = getButtonSource();
        if (isPublisherAdminMode) {
            appMenu.addAction(removeAction)
        } else if (!app.notInstalled && !app.isFromServer) {
            appMenu.addAction(uninstallAction)
        } else {
            button.showAdditional = false;
        }
    }
}