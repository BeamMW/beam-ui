import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtWebEngine
import QtWebChannel
import Beam.Wallet      1.0
import "../controls"

Item {
    id:             control
    implicitHeight: 112
    implicitWidth:  320

    property var    app
    property bool   showButtons:          true
    property bool   isPublisherAdminMode: false
    property bool   isIPFSAvailable:      false
    property bool   isBusy:               false
    property string statusText:           ""
    property bool   isEnabled:            isPanelEnabled()

    readonly property int textWidth: 200

    property var stopProgress: function(appGuid) {
        if (!!control.app && !!control.app.guid && control.app.guid === appGuid) {
            isBusy = false;
            statusText = ""
        }
    }

    signal launch(var app)
    signal install(var app, var launchOnSuccess)
    signal update(var app)
    signal uninstall(var app)
    signal remove(var app)
    signal showDetails(var app)

    function isPanelEnabled() {
        if (isPublisherAdminMode) {
            return isIPFSAvailable
        } 
        if (!app.supported) {
            return false
        } else if (!!app.notInstalled || (!app.notInstalled && !!app.hasUpdate)) {
            // Bundled default apps install from local file, no IPFS required
            return !!app.defaultDappPath || isIPFSAvailable
        }
        return true
     }

    // background
    Rectangle {
        anchors.fill: parent
        radius:       10
        color:        Style.active
        opacity:      hoverArea.containsMouse ? 0.15 : (isEnabled ? 0.1 : 0.05)
    }

    Row {
        z:                        42
        anchors.right:            parent.right
        anchors.top:              parent.top
        anchors.margins:          16
        visible:                  !isBusy
        CustomToolButton {
            id:                       installButton
            padding:                  0
            height:                   16
            width:                    16
            icon.color:               Style.active
            icon.source:              "qrc:/assets/icon-download-green.svg"
            enabled:                  isEnabled
            visible:                  !!app.notInstalled
            onClicked: {
                control.isBusy = true;
                control.install(app, false)
            }
        }
        CustomToolButton {
            id:                       actionsButton
            padding:                  0
            height:                   16
            width:                    16
            icon.color:               Style.active
            icon.source:              "qrc:/assets/icon-actions.svg"
            enabled:                  isEnabled
            visible:                  !app.notInstalled || isPublisherAdminMode
            onClicked: {
                var instance = appMenuComponent.createObject(actionsButton);
                if (!isPublisherAdminMode) {
                    instance.removeAction(instance.myRemoveAction)
                } 
                if (!(!app.notInstalled && !app.devApp)) {
                    instance.removeAction(instance.uninstallAction)
                }
                instance.open()
            }
        }
    }
    RowLayout {
        anchors.right:          parent.right
        anchors.top:            parent.top
        anchors.margins:        16
        spacing:                4
        SFText {
            color:              Style.active
            font.pixelSize:     12
            text:               control.statusText
        }
        UpdateIndicator {
            radius:             8
            visible:            isBusy
        }
    }

    RowLayout {
        anchors.fill:         parent
        anchors.topMargin:    16
        anchors.leftMargin:   16
        anchors.rightMargin:  16
        anchors.bottomMargin: 16
        spacing:              16

        // icon
        Rectangle {
            Layout.preferredWidth:  80
            Layout.preferredHeight: 80
            radius:                 40
            color:                  Style.background_main

            SvgImage {
                id:                       defaultIcon
                width:                    50
                height:                   50
                source:                   hoverArea.containsMouse ? "qrc:/assets/icon-defapp-active.svg" : "qrc:/assets/icon-defapp.svg"
                anchors.verticalCenter:   parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                visible:                  !customIcon.visible
            }

            SvgImage {
                id:                       customIcon
                width:                    50
                height:                   50
                source:                   !!app.icon ? app.icon : "qrc:/assets/icon-defapp.svg"
                anchors.verticalCenter:   parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                visible:                  !!app.icon && progress == 1.0
            }
        }

        ColumnLayout {
            Layout.alignment:   Qt.AlignVCenter
            Layout.fillWidth:   true
            spacing:            4
            Item {
                Layout.fillWidth:   true
                Layout.fillHeight:  true
            }

            SFText {
                Layout.fillWidth:   true
                text: app.name
                font {
                    styleName:      "DemiBold"
                    weight:         Font.DemiBold
                    pixelSize:      16
                }
                color:              Style.content_main
                wrapMode:           Text.Wrap
            }

            SFText {
                Layout.fillWidth:      true
                text:                  !!app.publisherName ? app.publisherName : ""
                visible:                !!app.publisherName
                font.pixelSize:        12
                elide:                 Text.ElideRight
                color:                 Style.content_secondary
            }

            Item {
                Layout.fillWidth:   true
                Layout.fillHeight:  true
            }
        }
    }

    MouseArea {
        id:                      hoverArea
        anchors.fill:            parent
        hoverEnabled:            true
        propagateComposedEvents: true
        enabled:                 isEnabled
        onClicked: {
            if (isBusy) {
                return;
            }
            if (isPublisherAdminMode) {
                control.update(app)
            } else {
            if (!!app.notInstalled) {
                control.isBusy = true;
                //% "installing"
                control.statusText = qsTrId("dapps-store-installing")
                control.install(app, true)
            } else if (!app.notInstalled && !!app.hasUpdate) {
                control.isBusy = true;
                //% "updating"
                control.statusText = qsTrId("dapps-store-updating")
                control.update(app)
            } else if (!app.notInstalled)
                control.launch(app)
            }
        }
        ToolTip {
            id:           toolTip
            delay:        500
            timeout:      2000
            visible:      false
            text:         !app.supported && !control.isPublisherAdminMode ? 
                            //% "This DApp requires version %1 of Beam Wallet or higher. Please update your wallet."
                            qsTrId("apps-version-error").arg(app.min_api_version || app.api_version)
                            //% "IPFS Service is not running or is not connected to the peers. Please check the settings."
                            : qsTrId("dapps-store-ipfs-unavailable")
            width:        300
            modal:        false
            parent:       control
            x:            parent.width - width - 20
            y:            parent.height + 4
            z:            100
            padding:      20
            closePolicy:  Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

            contentItem: SFText {
                color:            Style.validator_error
                font.pixelSize:   12
                text:             toolTip.text
                maximumLineCount: 2
                wrapMode:         Text.WordWrap
            }

            background: Rectangle {
                radius:       10
                color:        Style.background_popup
                anchors.fill: parent
            }
        }
    }
    Component {
        id:     appMenuComponent
        ContextMenu {
            id:    appMenu
            modal: true
            dim:   false
            property alias updateAction:        updateActionInternal
            property alias myRemoveAction:      removeActionInternal
            property alias uninstallAction:     uninstallActionInternal
            Action {
                id:          updateActionInternal
                                //% "update"
                text:        qsTrId("dapps-store-update")
                icon.source: "qrc:/assets/icon-dapps-store-update.svg"
                enabled:     !app.notInstalled && !!app.hasUpdate
                onTriggered: function () {
                    control.isBusy = true
                    control.update(app)
                }
            }

            Action {
                id:          uninstallActionInternal
                             //% "Uninstall"
                text:        qsTrId("apps-uninstall")
                icon.source: "qrc:/assets/icon-delete.svg"
                onTriggered: function () {
                    control.isBusy = true
                    control.uninstall(app)
                }
            }

            Action {
                id:          removeActionInternal
                             //% "remove dapp"
                text:        qsTrId("dapps-store-remove-dapp")
                icon.source: "qrc:/assets/icon-delete.svg"
                onTriggered: function () {
                    control.isBusy = true
                    control.remove(app)
                }
            }
            Action {
                id:          detailsActionInternal
                             //% "dapp details"
                text:        qsTrId("dapps-store-dapp-details")
                icon.source: "qrc:/assets/icon-show_tx_details.svg"
                onTriggered: control.showDetails(app)
            }
        }
    }

}