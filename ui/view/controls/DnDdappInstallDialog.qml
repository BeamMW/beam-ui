import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts  1.12
import "../utils.js" as Utils
import "."

CustomDialog {
    id: control

    property var onGetFileName: function(fileName) {
        console.log(fileName);
    }
    property bool isOk: false
    property bool isFail: false
    property string appName: ""

    onOpened: {
        control.isOk = false;
        control.isFail = false;
        control.appName = "";
    }

    modal: true
    //% "Install DApp"
    title: qsTrId("dnd-app-install-title")

    width: 520
    height: 420
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    visible: false

    header: SFText {
        text: control.title
        topPadding: 30
        visible: control.title.length > 0
        horizontalAlignment : Text.AlignHCenter
        font.pixelSize: 18
        font.styleName: "Bold"; font.weight: Font.Bold
        color: Style.content_main
    }

    contentItem:
        DropArea {
        id: dropArea
        anchors.fill: parent

        onEntered: function (drag) {
            if (drag.urls && drag.urls.length == 1) {
                let fname = drag.urls[0]
                if (fname.startsWith("file:///") && fname.endsWith(".dapp"))  {
                    drag.accepted = true
                    return
                }
            }
            drag.accepted = false
        }

        onDropped: function (drag) {
            let fname = drag.urls[0]
            if (!control.isOk && !control.isFail) {
                onGetFileName(fname);
            }
        }

         ColumnLayout {
            anchors.fill: parent
            anchors.margins: 30

            spacing: 30

            PrimaryButton {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 50
                icon.source: "qrc:/assets/icon-receive-blue.svg"
                //% "load a file"
                text: qsTrId("dnd-load-a-file")
                onClicked: {
                    onGetFileName("");
                }
                visible: !control.isOk && !control.isFail
            }

            SFText {
                Layout.alignment: Qt.AlignHCenter
                font {
                    styleName: "Normal"
                    weight: Font.Normal
                    italic: true
                    pixelSize: 14
                }
                color: Style.content_secondary
                //% "or just drop it here"
                text: qsTrId("dnd-jus-drop")
                visible: !control.isOk && !control.isFail
            }

            SFText {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 40
                font {
                    styleName: "Normal"
                    weight: Font.Normal
                    italic: true
                    pixelSize: 14
                }
                color: Style.content_secondary
                //% "The \"%1\" DApp is installed successfully."
                text: qsTrId("dnd-install-success").arg(control.appName)
                visible: control.isOk
            }

            SFText {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 40
                font {
                    styleName: "Normal"
                    weight: Font.Normal
                    italic: true
                    pixelSize: 14
                }
                color: Style.validator_error
                //% "The DApp installation error."
                text: qsTrId("dnd-install-fail")
                visible: control.isFail
            }

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/upload-a-file.svg"
                Layout.maximumHeight: 99
                Layout.maximumWidth: 89
                visible: !control.isOk && !control.isFail
            }

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/upload-success.svg"
                Layout.maximumHeight: 106
                Layout.maximumWidth: 106
                visible: control.isOk
            }

            SvgImage {
                Layout.alignment: Qt.AlignHCenter
                source: "qrc:/assets/upload-error.svg"
                Layout.maximumHeight: 106
                Layout.maximumWidth: 106
                visible: control.isFail
            }

            CustomButton {
                Layout.topMargin: 24
                Layout.alignment: Qt.AlignHCenter
                text: control.isOk || control.isFail ? qsTrId("general-close") : qsTrId("general-cancel")
                icon.source: "qrc:/assets/icon-cancel-white.svg"
                onClicked: control.close()
            }
        }
    }
}
