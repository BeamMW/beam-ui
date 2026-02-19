import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Beam.Wallet      1.0
import "../controls"

CustomDialog {
    id:          control
    modal:       true
    x:           (parent.width - width) / 2
    y:           (parent.height - height) / 2
    parent:      Overlay.overlay
    width:       761
    closePolicy: Popup.NoAutoClose

    property bool newPublisher: true
    property string nickname: ""
    property string publisherKey: ""

    signal goToMyAccount()

    contentItem: ColumnLayout {
        spacing: 30

        // Title
        SFText {
            Layout.topMargin: 40
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize:   18
            color:            Style.content_main
            //% "You now are a publisher!"
            text:             qsTrId("dapps-store-you-now-are-publisher")
        }

        // Note
        SFText {
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize:   14
            color:            Style.content_main
            //% "Congratulations %1!\n\nWe are glad to see you at our DApp Store as a Publisher.\nBelow you will see your personal Publisher Key. Any user can use it to add you to their personal\nlist and follow your apps. You can add it on your personal page or website.\nAll the time you have access to it in yout personal publisher account.\n\nLet's start!"
            text: qsTrId("dapps-store-congratulations").arg(control.nickname)
        }

        RowLayout {
            Layout.leftMargin:  100
            Layout.rightMargin: 100
            spacing:            10
            Layout.alignment:   Qt.AlignVCenter

            SFText {
                text:             control.publisherKey
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
                        BeamGlobals.copyToClipboard(control.publisherKey)
                    }
                }
            }
        }

        PrimaryButton {
            icon.source:        "qrc:/assets/icon-dapps-store-user.svg"
            Layout.bottomMargin: 40
            palette.buttonText: Style.content_opposite
            icon.color:         Style.content_opposite
            palette.button:     Style.active
            Layout.alignment:   Qt.AlignHCenter
            //% "go to my account"
            text:               qsTrId("dapps-store-to-to-my-account")
            onClicked: {
                control.goToMyAccount();
            }
        }
    }
}