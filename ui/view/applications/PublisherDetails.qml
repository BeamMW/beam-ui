import QtQuick          2.11
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import QtWebEngine      1.4
import QtWebChannel     1.0
import Beam.Wallet      1.0
import "../controls"

ColumnLayout {
    id: control
    Layout.fillWidth: true
    Layout.topMargin: 27
    spacing:          0

    property var viewModel
    property var appsList: undefined
    readonly property bool hasApps: !!appsList && appsList.length > 0

    property var onBack: function () {
        console.log("PublisherDetails::onBack is not initialized")
    }

    property var uninstall: function () {
        console.log("PublisherDetails::uninstall is not initialized")
    }

    function uploadApp() {
        uploadDAppDialog.open()
    }

    function editDetails() {
        // TODO: implement
        changePublisherInfoDialog.open();
    }

    function showPublicKey() {
        publisherKeyDialog.open()
    }

    function loadPublisherDApps() {
        appsList = viewModel.getPublisherDApps(viewModel.publisherInfo.publisherKey)
    }

    function uploadNewVersion(app) {
        uploadDAppDialog.currentApp = app
        uploadDAppDialog.open()
    }

    function remove(app) {
        viewModel.removeDApp(app.guid)
    }

    Component.onCompleted: {
        control.viewModel.appsChanged.connect(function() {
            loadPublisherDApps()
        })

        loadPublisherDApps()
    }

    //
    // Page Header (Back button + title + publisher's buttons)
    //
    RowLayout {
        id: header

        CustomButton {
            id:             backButton
            palette.button: "transparent"
            leftPadding:    0
            showHandCursor: true

            font {
                styleName: "DemiBold"
                weight:    Font.DemiBold
            }

            //% "Back"
            text:        qsTrId("general-back")
            icon.source: "qrc:/assets/icon-back.svg"
            visible:     true

            onClicked:   control.onBack()
        }

        SFText {
            Layout.fillWidth:     true
            color:                Style.content_main
            horizontalAlignment:  Text.AlignHCenter
            font.pixelSize:       14
            font.weight:          Font.Bold
            font.capitalization:  Font.AllUppercase
            //% "Publisher's page"
            text: qsTrId("dapps-store-publisher-page")
        }

        CustomButton {
            Layout.alignment: Qt.AlignRight
            width:            38
            radius:           10
            display:          AbstractButton.IconOnly
            leftPadding:      11
            rightPadding:     11
            palette.button:   Style.active
            icon.source:      "qrc:/assets/icon-dapps_store-publisher-upload-dapp.svg"
            icon.color:       Style.background_main
            onClicked:        uploadApp()
        }

        CustomButton {
            Layout.leftMargin:  20
            Layout.rightMargin: 20
            Layout.alignment:   Qt.AlignRight
            width:              38
            radius:             10
            display:            AbstractButton.IconOnly
            leftPadding:        11
            rightPadding:       11
            palette.button:     Qt.rgba(255, 255, 255, 0.1)
            icon.source:        "qrc:/assets/icon-dapps_store-publisher-edit.svg"
            icon.color:         Style.active
            onClicked:          editDetails()
        }

        CustomButton {
            Layout.rightMargin: 30
            Layout.alignment:   Qt.AlignRight
            width:              38
            radius:             10
            display:            AbstractButton.IconOnly
            leftPadding:        11
            rightPadding:       11
            palette.button:     Qt.rgba(255, 255, 255, 0.1)
            icon.source:        "qrc:/assets/icon-dapps_store-publisher-show-key.svg"
            icon.color:         Style.active
            onClicked:          showPublicKey()
        }
    }

    //
    // Title
    //
    SFText {
        Layout.topMargin:     20
        Layout.bottomMargin:  20
        color:                Style.content_main
        font.pixelSize:       14
        font.weight:          Font.Bold
        font.capitalization:  Font.AllUppercase
        opacity:              0.5
        //% "My DAPPs - admin panel"
        text: qsTrId("dapps-store-my-dapps")
    }

    //
    // Body: AppList or dummy page
    //

    // dummy page
    ColumnLayout {
        visible: !appsListView.visible
        Layout.fillHeight: true
        Layout.fillWidth:  true

        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
        }

        SvgImage {
            Layout.topMargin: -80
            Layout.alignment: Qt.AlignHCenter
            width:   60
            height:  60
            opacity: 0.5
            source: "qrc:/assets/icon-dapps_store-empty-dapps-list.svg"
        }

        SFText {
            Layout.topMargin: 30
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize:   14
            color:            Style.content_main
            opacity:          0.5
            //% "You have no published DApps yet"
            text: qsTrId("dapps-store-publisher-have-not-dapps")
        }

        PrimaryButton {
            Layout.topMargin: 40
            Layout.alignment: Qt.AlignHCenter
            //% "upload your fist dapp"
            text: qsTrId("dapps-store-publisher-upload-first-dapp")
            icon.source: "qrc:/assets/icon-dapps_store-publisher-upload-dapp.svg"
            // TODO: remove size changing
            icon.width:  9
            icon.height: 11
            onClicked:   uploadApp()
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
        }
    }

    AppsList {
        id:                       appsListView
        Layout.fillHeight:        true
        Layout.fillWidth:         true
        visible:                  control.hasApps
        appsList:                 control.appsList
        showInstallFromFilePanel: false
        isPublisherAdminMode:     true

        onUpdate: function (app) {
            control.uploadNewVersion(app)
        }

        onRemove: function (app) {
            control.remove(app)
        }
    }

    BecomePublisher {
        id: changePublisherInfoDialog

        newPublisher: false
        publisherInfo: control.viewModel.publisherInfo

        onChangePublisherInfo: function(info) {
            control.viewModel.changePublisherInfo(info);
        }
    }

    TransactionIsSent {
        id: transactionSentDialog
        newPublisher: false
    }

    CustomDialog {
        id:      publisherKeyDialog
        modal:   true
        x:       (parent.width - width) / 2
        y:       (parent.height - height) / 2
        parent:  Overlay.overlay

        readonly property string publicKey: !!control.viewModel.publisherInfo ? control.viewModel.publisherInfo.publisherKey : ""

        onOpened: {
            forceActiveFocus()
        }

        contentItem: ColumnLayout {
            spacing:             30

            // Title
            SFText {
                Layout.fillWidth:    true
                Layout.topMargin:    40
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize:      18
                color:               Style.content_main
                //% "Publisher Key"
                text:                qsTrId("dapps-store-publisher-key")
            }

            // Note
            SFText {
                Layout.leftMargin:     100
                Layout.rightMargin:    100
                Layout.preferredWidth: 580
                wrapMode:              Text.WordWrap
                // horizontalAlignment:   Text.AlignHCenter
                font.pixelSize:        14
                color:                 Style.content_main
                //% "Here's your personal Publisher Key. Any user can use it to add you to their personal list and follow your apps. You can add it on your personal page or website."
                text:                  qsTrId("dapps-store-publisher-key-dialog-note")
            }

            // Body
            RowLayout {
                Layout.leftMargin:  100
                Layout.rightMargin: 100
                spacing:            10
                Layout.alignment:   Qt.AlignVCenter

                SFText {
                    text:             publisherKeyDialog.publicKey
                    width:            parent.width
                    color:            Style.active
                    font.pixelSize:   14
                }

                SvgImage {
                    Layout.alignment: Qt.AlignVCenter
                    source:           "qrc:/assets/icon-copy-green.svg"
                    sourceSize:       Qt.size(16, 16)

                    MouseArea {
                        anchors.fill:    parent
                        acceptedButtons: Qt.LeftButton
                        cursorShape:     Qt.PointingHandCursor
                        onClicked: function () {
                                BeamGlobals.copyToClipboard(publisherKeyDialog.publicKey)
                        }
                    }
                }
            }

            Row {
                id:                  buttonsLayout
                Layout.fillHeight:   true
                Layout.bottomMargin: 30
                Layout.alignment:    Qt.AlignHCenter
                spacing:             30
        
                CustomButton {
                    icon.source: "qrc:/assets/icon-cancel-16.svg"
                    //% "Close"
                    text:        qsTrId("general-close")
                    onClicked: {
                        publisherKeyDialog.close()
                    }
                }

                PrimaryButton {
                    icon.source:        "qrc:/assets/icon-copy.svg"
                    palette.buttonText: Style.content_opposite
                    icon.color:         Style.content_opposite
                    palette.button:     Style.active
                    //% "copy and close"
                    text:               qsTrId("general-copy-and-close")
                    onClicked: {
                        BeamGlobals.copyToClipboard(publisherKeyDialog.publicKey)
                        publisherKeyDialog.close();
                    }
                }
            }
        }
    }

    UploadDApp {
        id:                    uploadDAppDialog
        chooseFile:            control.viewModel.chooseFile
        getDAppFileProperties: control.viewModel.getDAppFileProperties
        parseDAppFile:         control.viewModel.parseDAppFile
        publishDApp:           control.viewModel.publishDApp
        checkDAppNewVersion:   control.viewModel.checkDAppNewVersion
    }
}
