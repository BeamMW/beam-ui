import QtQuick          2.15
import QtQuick.Layouts  1.15
import QtQuick.Controls 2.15
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
        changePublisherInfoDialog.open();
    }

    function showPublicKey() {
        publisherKeyDialog.open()
    }

    function loadPublisherDApps() {
        var publisherKey = viewModel.publisherInfo.pubkey
        appsList = viewModel.getPublisherDApps(publisherKey)
    }

    function uploadNewVersion(app) {
        uploadDAppDialog.currentApp = app
        uploadDAppDialog.open()
    }

    function remove(app) {
        viewModel.removeDApp(app.guid)
    }

    function showDAppStoreTxPopup(comment, txId) {
        const appName = "DApps Store"
        const appicon = "qrc:/assets/icon-dapps_store.svg"
        main.showAppTxPopup(comment, appName, appicon, txId, true);
    }

    Component.onCompleted: {
        viewModel.appsChanged.connect(loadPublisherDApps)
        viewModel.showDAppStoreTxPopup.connect(showDAppStoreTxPopup)
        loadPublisherDApps()
    }

    Component.onDestruction: {
        viewModel.appsChanged.disconnect(loadPublisherDApps)
        viewModel.showDAppStoreTxPopup.disconnect(showDAppStoreTxPopup)
    }

    //% "Publisher's page"
    property string title: qsTrId("dapps-store-publisher-page")

    //
    // Page Header (publisher's buttons)
    //
    property var titleContent:  RowLayout {
        id:      header
        spacing: 0
        Item {
            Layout.fillWidth:       true
            Layout.fillHeight:      true
        }
        Item {
            Layout.alignment:       Qt.AlignRight
            width:                  uploadDappButton.width
            height:                 uploadDappButton.height

            CustomButton {
                id:                     uploadDappButton
                enabled:                viewModel.isIPFSAvailable
                anchors.fill:           parent
                height:                 36
                Layout.preferredHeight: 36
                width:                  36
                radius:                 10
                display:                AbstractButton.IconOnly
                leftPadding:            6
                rightPadding:           6
                palette.button:         Style.active
                icon.source:            "qrc:/assets/icon-dapps_store-publisher-upload-dapp.svg"
                icon.width:             24
                icon.height:            24
                icon.color:             Style.background_main
                onClicked:              uploadApp()

                ToolTip {
                    id:           tooltip
                    delay:        500
                    timeout:      2000
                    visible:      uploadDappButtonHoverArea.containsMouse
                    width:        300
                    modal:        false
                    x:            uploadDappButton.width - width - 20
                    y:            uploadDappButton.height + 4
                    z:            100
                    padding:      20
                    closePolicy:  Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                    contentItem: SFText {
                        color:            Style.validator_error
                        font.pixelSize:   12
                                          //% "IPFS Service is not running or is not connected to the peers. Please check the settings."
                        text:             qsTrId("dapps-store-ipfs-unavailable")
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
            MouseArea {
                id:                      uploadDappButtonHoverArea
                anchors.fill:            uploadDappButton
                hoverEnabled:            true
                propagateComposedEvents: true
                enabled:                 !uploadDappButton.enabled
            }
        }

        CustomButton {
            Layout.leftMargin:      20
            Layout.rightMargin:     20
            Layout.alignment:       Qt.AlignRight
            height:                 36
            Layout.preferredHeight: 36
            width:                  36
            radius:                 10
            display:                AbstractButton.IconOnly
            leftPadding:            6
            rightPadding:           6
            palette.button:         Qt.rgba(255, 255, 255, 0.1)
            icon.source:            "qrc:/assets/icon-dapps_store-publisher-edit.svg"
            icon.width:             24
            icon.height:            24
            icon.color:             Style.active
            onClicked:              editDetails()
        }

        CustomButton {
            Layout.rightMargin:     10
            Layout.alignment:       Qt.AlignRight
            height:                 36
            Layout.preferredHeight: 36
            width:                  36
            radius:                 10
            display:                AbstractButton.IconOnly
            leftPadding:            6
            rightPadding:           6
            palette.button:         Qt.rgba(255, 255, 255, 0.1)
            icon.source:            "qrc:/assets/icon-dapps_store-publisher-show-key.svg"
            icon.width:             24
            icon.height:            24
            icon.color:             Style.active
            onClicked:              showPublicKey()
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
        font.letterSpacing:   3.11
        opacity:              0.5
        //% "My DAPPs - admin panel"
        text: qsTrId("dapps-store-my-dapps")
    }

    //
    // Body: AppList or dummy page
    //

    // dummy page
    ColumnLayout {
        visible:           !appsListView.visible
        Layout.fillHeight: true
        Layout.fillWidth:  true
        spacing:           0

        Item {
            Layout.fillHeight: true
            Layout.fillWidth:  true
        }
        
        Item {
            width:            60
            height:           60
            Layout.topMargin: -60
            Layout.alignment: Qt.AlignHCenter
            SvgImage {
                anchors.fill: parent
                sourceSize:   Qt.size(60, 60)
                source:       "qrc:/assets/icon-dapps_store-empty-dapps-list.svg"
            }
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

        Item {
            Layout.topMargin: 40
            Layout.alignment: Qt.AlignHCenter
            width:            uploadFirstDAppButton.width
            height:           uploadFirstDAppButton.height

            PrimaryButton {
                id:               uploadFirstDAppButton
                enabled:          viewModel.isIPFSAvailable
                                  //% "upload your first dapp"
                text:             qsTrId("dapps-store-publisher-upload-first-dapp")
                icon.source:      "qrc:/assets/icon-dapps_store-publisher-upload-dapp.svg"
                icon.width:       16
                icon.height:      16
                onClicked:        uploadApp()

                ToolTip {
                    delay:        500
                    timeout:      2000
                    visible:      uploadFirstDAppButtonHoverArea.containsMouse
                    width:        300
                    modal:        false
                    x:            parent.width - width - 20
                    y:            parent.height + 4
                    z:            100
                    padding:      20
                    closePolicy:  Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                    contentItem: SFText {
                        color:            Style.validator_error
                        font.pixelSize:   12
                                          //% "IPFS Service is not running or is not connected to the peers. Please check the settings."
                        text:             qsTrId("dapps-store-ipfs-unavailable")
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
            MouseArea {
                id:                      uploadFirstDAppButtonHoverArea
                anchors.fill:            uploadFirstDAppButton
                hoverEnabled:            true
                propagateComposedEvents: true
                enabled:                 !uploadFirstDAppButton.enabled
            }
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
        isPublisherAdminMode:     true
        isIPFSAvailable:          control.viewModel.isIPFSAvailable

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
            control.viewModel.changePublisherInfo(info, false);
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

        readonly property string publicKey: !!control.viewModel.publisherInfo ? control.viewModel.publisherInfo.pubkey : ""

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
                font.weight:         Font.Bold
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
