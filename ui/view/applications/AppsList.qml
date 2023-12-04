import QtQuick          2.15
import QtQuick.Layouts  1.15
import QtQuick.Controls 2.15
import Beam.Wallet      1.0
import "../utils.js" as Utils
import "../controls"

Item {
    id: control
    property var  appsList
    property bool hasLocal
    property bool isPublisherAdminMode:      false
    property bool isIPFSAvailable:           false

    onAppsListChanged: function() {
        if (!!appsList && appsList.length > 0) {
            for (let idx = 0; idx < 2; ++idx) {
                if (appsList[idx].local) {
                    hasLocal = true
                    return
                }
            }
        }
        hasLocal = false
    }

    signal launch(var app)
    signal install(var appGUID, var launchOnSuccess)
    signal update(var app)
    signal uninstall(var app)
    signal remove(var app)
    signal stopProgress(var appGuid)

    // Actuall apps list
    GridView {
        id:                          gridView
        anchors.fill:                parent
        anchors.rightMargin:         -10
        cellHeight:                  122
        cellWidth:                   gridView.width/3
        clip:                        true
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        model: control.appsList

        delegate: AppPanel {
            width:                  gridView.cellWidth  - 10
            height:                 gridView.cellHeight - 10
            app:                    modelData
            isPublisherAdminMode:   control.isPublisherAdminMode
            isIPFSAvailable:        control.isIPFSAvailable
                
            onLaunch: function (app) {
                control.launch(app)
            }
            onInstall: function (app, launchOnSuccess) {
                control.install(app, launchOnSuccess)
            }
            onUpdate: function (app) {
                control.update(app)
            }
            onUninstall: function (app) {
                //% "Are you sure you want to uninstall %1 DApp?"
                confirmUninstall.text = qsTrId("apps-uninstall-confirm").arg(app.name);
                confirmUninstall.accepted.connect(function () {
                    console.log("#### " + app.name)
                    control.uninstall(app);
                    confirmUninstall.accepted.disconnect()
                    console.log("4444")
                });
                confirmUninstall.open();
            }
            onRemove: function (app) {
                control.remove(app)
            }
            Component.onCompleted: {
                control.stopProgress.connect(stopProgress);
            }
            Component.onDestruction: {
                control.stopProgress.disconnect(stopProgress);
            }
        }
    }

    ConfirmationDialog {
        id:                     confirmUninstall
        width:                  460
                                //% "Uninstall DApp"
        title:                  qsTrId("app-uninstall-title")
                                //% "Uninstall"
        okButtonText:           qsTrId("apps-uninstall")
        okButtonIconSource:     "qrc:/assets/icon-delete.svg"
        okButtonColor:          Style.accent_fail
        cancelButtonIconSource: "qrc:/assets/icon-cancel-white.svg"
    }
}