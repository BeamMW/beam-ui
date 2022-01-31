import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"
import "../wallet"
import "../applications"

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27
    spacing: 0

    property string   errorMessage: ""
    property var      appsList: viewModel.apps
    property var      unsupportedCnt: 0
    property var      activeApp: undefined
    property var      appToOpen: undefined
    property bool     showBack: true
    readonly property bool hasApps: !!appsList && appsList.length > 0
    property var      publishersList: viewModel.publishers

    DappsStoreViewModel {
        id: viewModel

        onAppInstallOK: function (appName) {
            //% "'%1' is successfully installed."
            installOK.text = qsTrId("apps-install-success").arg(appName)
            installOK.open()
        }

        onAppInstallFail: function (appName) {
            //% "Failed to install DApp:\n%1"
            installFail.text = qsTrId("apps-install-fail").arg(appName)
            installFail.open()
        }
    }

    CustomDialog {
        id:      publisherDialog
        width:   Math.round(2 * parent.width / 3)
        height:  Math.round(2 * parent.height / 3)
        parent:  Overlay.overlay
        x:       Math.round((parent.width - width) / 2)
        y:       Math.round((parent.height - height) / 2)
        modal:   true

        contentItem: ColumnLayout {
            spacing: 0
            anchors.fill:    parent
            anchors.margins: 30

            // title
            SFText {
                Layout.fillWidth:     true
                color:                Style.white
                horizontalAlignment:  Text.AlignHCenter
                font.pixelSize:       18
                font.weight:          Font.Bold
                text: "Publishers"
            }

            ScrollView {
                Layout.fillWidth:          true
                Layout.fillHeight:         true
                Layout.topMargin:          35
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                clip:                      true

                ColumnLayout {
                    Layout.fillWidth:  true
                    Layout.fillHeight: true
                    spacing:           30

                    Repeater {
                        model: control.publishersList
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            SFLabel {
                                id: pubKeyId
                                Layout.fillWidth:    true
                                horizontalAlignment: Text.AlignLeft
                                text:                modelData.pubkey
                                font.pixelSize:      14
                                color:               Style.content_main
                                copyMenuEnabled:     true
                                onCopyText:          BeamGlobals.copyToClipboard(text)
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 10
                            }

                            SFLabel {
                                Layout.fillWidth:    true
                                horizontalAlignment: Text.AlignLeft
                                text:                modelData.name
                                font.pixelSize:      14
                                color:               Style.content_main
                                copyMenuEnabled:     true
                                onCopyText:          BeamGlobals.copyToClipboard(text)
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.preferredWidth: 10
                            }

                            CustomToolButton {
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                icon.source: "qrc:/assets/icon-copy.svg"
                                //% "Copy address"
                                ToolTip.text: qsTrId("settings-swap-copy-address")
                                onClicked: BeamGlobals.copyToClipboard(pubKeyId.text)
                            }
                        }
                    }
                }
            }

            // buttons
            CustomButton {
                Layout.topMargin:     24
                Layout.alignment:     Qt.AlignHCenter
                text:             qsTrId("general-close")
                icon.source:      "qrc:/assets/icon-cancel-white.svg"
                onClicked:        publisherDialog.close()
            }
        }
    }

    //
    // Page Header (Title + Status Bar)
    //
    Title {
        //% "DApp Store"
        text: qsTrId("apps-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    RowLayout {
        id: publisherPanel
        Layout.fillWidth: true
        Layout.topMargin: 20

        CustomButton {
            //% "publish"
            text:                qsTrId("dapps-store-publish")
            palette.buttonText:  Style.content_main
            palette.button:      Style.background_button
            icon.source:         "qrc:/assets/icon-add.svg"
            onClicked: {
                viewModel.uploadApp()
            }
        }

        CustomButton {
            //% "show my publisherKey"
            text:                qsTrId("dapps-store-show-publisher-key")
            palette.buttonText:  Style.content_main
            palette.button:      Style.background_button
            icon.source:         "qrc:/assets/icon-add.svg"
            onClicked: {
                showPublisherKeyDialog.open()
            }
        }

        CustomButton {
            //% "register as publisher"
            text:                qsTrId("dapps-store-register-publisher")
            palette.buttonText:  Style.content_main
            palette.button:      Style.background_button
            icon.source:         "qrc:/assets/icon-add.svg"
            onClicked: {
                // TODO: provide form to fill data
                viewModel.registerPublisher()
            }
        }

        CustomButton {
            //% "show all publishers"
            text:                qsTrId("dapps-store-show-all-publishers")
            palette.buttonText:  Style.content_main
            palette.button:      Style.background_button
            icon.source:         "qrc:/assets/icon-add.svg"
            onClicked: {
                publisherDialog.open()
            }
        }
    }

    function installApp(app) {
        viewModel.installApp(app.guid)
    }

    function launchApp(app) {
        var appName = app.name;
        var appid = app.appid;
        main.updateItem("applications", {"appToOpen": { "name": appName, "appid": appid}, "showBack": false})
    }

    function updateApp(app) {
        viewModel.updateApp(app.guid)
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth:  true
        visible: !appsListView.visible

        ColumnLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height / 2 - this.height / 2 - 40
            spacing: 40

            SFText {
                Layout.alignment: Qt.AlignHCenter
                color: control.errorMessage.length ? Style.validator_error : Style.content_main
                opacity: 0.5

                font {
                    italic:    true
                    pixelSize: 16
                }

                text: {
                    if (control.errorMessage.length) {
                        return control.errorMessage
                    }

                    if (control.activeApp || control.appToOpen) {
                        //% "Please wait, %1 is loading"
                        return qsTrId("apps-loading-app").arg(
                            (control.activeApp || control.appToOpen).name
                        )
                    }

                    if (!control.appsList) {
                        //% "Loading..."
                        return qsTrId("apps-loading")
                    }

                    //% "There are no applications at the moment"
                    return qsTrId("apps-nothing")
                }
            }

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/dapp-loading.svg"
                sourceSize: Qt.size(245, 140)

                visible: {
                    if (control.errorMessage.length) {
                        return false
                    }

                    if (control.activeApp || control.appToOpen) {
                        return true
                    }

                    return false
                }
            }
        }
    }

    AppsList {
        id: appsListView
        Layout.topMargin:  40 - (unsupportedCnt ? errCntMessage.height + 5 + parent.spacing : 0)
        Layout.fillHeight: true
        Layout.fillWidth:  true
        Layout.bottomMargin: 10
        opacity:  1.0
        visible:  control.hasApps && !control.activeApp
        appsList: control.appsList
        showInstallFromFilePanel: false

        onLaunch: function (app) {
            launchApp(app)
        }

        onInstall: function (app) {
            installApp(app)
        }

        onUpdate: function (app) {
            updateApp(app)
        }
    }

    function loadAppsList () {
       // control.appsList = viewModel.apps
    }

    ConfirmationDialog {
        id: installOK
        //% "Install DApp"
        title: qsTrId("app-install-title")
        //% "Ok"
        okButtonText: qsTrId("general-ok")
        cancelButtonVisible: false
    }

    ConfirmationDialog {
        id: installFail
        //% "Install DApp"
        title: qsTrId("app-install-title")
        //% "Ok"
        okButtonText: qsTrId("general-ok")
        okButton.palette.button: Style.accent_fail
        cancelButtonVisible: false
    }

    ConfirmationDialog {
        id: showPublisherKeyDialog
        //% "Publisher key"
        title: qsTrId("dapps-store-publisher-key")
        okButtonText: qsTrId("general-copy-and-close")
        okButtonIconSource: "qrc:/assets/icon-copy.svg"
        cancelButtonVisible: false
        width: 470
        // TODO: check & mb change to lazy getting
        text: viewModel.publisherKey
        onAccepted: {
            BeamGlobals.copyToClipboard(viewModel.publisherKey);
        }
    }
}
