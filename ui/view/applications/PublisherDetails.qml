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

    property var appsList: undefined
    readonly property bool hasApps: !!appsList && appsList.length > 0
    property var onBack

    function uploadApp() {
        // TODO: implement
        // uploadDAppDialog.open()
    }

    function editDetails() {
        // TODO: implement
    }

    function showPublicKey() {
        // TODO: implement
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
        color:                Style.content_main
        font.pixelSize:       14
        font.weight:          Font.Bold
        opacity:              0.5
        //% "My DAPPs"
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
            // TODO: change icon
            source: "qrc:/assets/icon-applications.svg"
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
        id: appsListView
        Layout.fillHeight: true
        Layout.fillWidth:  true
        visible:  control.hasApps && !control.activeApp
        // TODO: implement
    }
}
