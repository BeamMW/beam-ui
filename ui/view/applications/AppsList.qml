import QtQuick          2.15
import QtQuick.Layouts  1.15
import QtQuick.Controls 2.15
import Beam.Wallet      1.0
import "../utils.js" as Utils
import "../controls"

ColumnLayout {
    id: control
    spacing:    0
    property var  appsList
    property bool hasLocal
    property bool isPublisherAdminMode:      false
    property bool isIPFSAvailable:           false
    property alias model:                    gridView.model
    property var   appActionsMenu:           undefined
    property bool showActions:               true

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

    state: "installed"
    states: [
        State {
            name: "installed"
            PropertyChanges { target: installedTab; state: "active" }
            PropertyChanges { target: appFilterProxy; filterString: "false" }
            PropertyChanges { target: emptyMessage; text:
                //% "You don't have any applications installed yet"
                qsTrId("apps-no-installed")}
        },
        State {
            name: "all"
            PropertyChanges { target: allTab; state: "active" }
            PropertyChanges { target: appFilterProxy; filterString: "*" }
            PropertyChanges { target: emptyMessage; text:
                //% "No applications yet"
                qsTrId("apps-no-at-all")}
        }
    ]

    signal launch(var app)
    signal install(var appGUID, var launchOnSuccess)
    signal update(var app)
    signal uninstall(var app)
    signal remove(var app)
    signal stopProgress(var appGuid)
   
    RowLayout {
        Layout.fillHeight:      false
        Layout.bottomMargin:    10
        TxFilter {
            id: installedTab
            Layout.alignment: Qt.AlignVCenter
            //% "Installed"
            label: qsTrId("apps-installed-tab")
            onClicked: control.state = "installed"
        }

        TxFilter {
            id: allTab
            Layout.alignment: Qt.AlignVCenter
            //% "All"
            label: qsTrId("apps-all-tab")
            onClicked: control.state = "all"
        }
        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }
        CustomToolButton {
            id:                       appActionButton
            padding:                  0
            icon.color:               Style.content_main
            icon.source:              "qrc:/assets/icon-settings.svg"
            onClicked:                if (!!appActionsMenu) appActionsMenu.popup(appActionButton)
            visible:                  control.showActions
        }
        SearchBox {
            id: searchBox
            Layout.alignment: Qt.AlignVCenter
            //% "Enter search text..."
            placeholderText: qsTrId("wallet-search-transactions-placeholder")
        }
    }

    ColumnLayout {
        Layout.topMargin:   90
        Layout.alignment:   Qt.AlignHCenter
        visible:            gridView.model.count == 0

        SvgImage {
            Layout.alignment:       Qt.AlignHCenter
            source:                 "qrc:/assets/icon-applications.svg" 
            sourceSize:             Qt.size(60, 60)
        }

        SFText {
            id:                   emptyMessage
            Layout.topMargin:     30
            Layout.alignment:     Qt.AlignHCenter
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            color:                Style.content_main
            opacity:              0.5
            lineHeight:           1.43
        }

        Item {
            Layout.fillHeight: true
        }
    }

    // Actuall apps list
    GridView {
        id:                          gridView
        Layout.fillWidth:            true
        Layout.fillHeight:           true
        Layout.rightMargin:          -10
        cellHeight:                  122
        cellWidth:                   gridView.width / Math.floor(gridView.width / 320)
        clip:                        true
        visible:                     model.count > 0
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        model: SortFilterProxyModel {
            id:           appFilterProxy
            filterRole:   "notInstalled"
            filterString: "false"
            filterSyntax: SortFilterProxyModel.Wildcard
            filterCaseSensitivity: Qt.CaseInsensitive
            source: SortFilterProxyModel {
                        id:                     searchProxyModel
                        filterRole:             "name"
                        filterString:           searchBox.text
                        filterSyntax:           SortFilterProxyModel.Wildcard
                        filterCaseSensitivity:  Qt.CaseInsensitive
                        source:                 control.appsList
            }
        }
        
        //model:control.appsList
        delegate: AppPanel {
            width:                  gridView.cellWidth  - 10
            height:                 gridView.cellHeight - 10
            app:                    model
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
                
                function acceptHandler() {
                    confirmUninstall.accepted.disconnect(acceptHandler)
                    control.uninstall(app);
                }
                confirmUninstall.accepted.connect(acceptHandler);
                confirmUninstall.open();
            }
            onRemove: function (app) {
                control.remove(app)
            }
            onShowDetails: function (app) {
                appDetails.app = app;
                appDetails.open()
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

    AppDetails {
        id:         appDetails
    }
}