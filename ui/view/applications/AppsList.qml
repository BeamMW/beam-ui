import QtQuick          2.15
import QtQuick.Layouts  1.12
import QtQuick.Controls 2.4
import Beam.Wallet      1.0
import "../utils.js" as Utils
import "../controls"

Item {
    id: control
    property var  appsList
    property bool hasLocal
    property alias showInstallFromFilePanel: installFromFilePanel.visible
    property var onOpenDnd: function(){
        console.log('open DnD dialog');
    }

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
    signal install(var appGUID)
    signal installFromFile(string fname)
    signal update(var app)
    signal uninstall(var app)


    // Actuall apps list
    ScrollView {
        id: scrollView
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        clip: true

        GridLayout {
            width: scrollView.availableWidth
            columnSpacing: 20
            rowSpacing: 15
            columns: 2

            Repeater {
                model: control.appsList

                delegate: AppPanel {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 440
                    Layout.preferredHeight: 144
                    app: modelData

                    onLaunch: function (app) { 
                        control.launch(app)
                    }
                    onInstall: function (appGUID) {
                        control.install(appGUID)
                    }
                    onUpdate: function (app) {
                        control.update(app)
                    }
                    onUninstall: function (app) {
                        control.uninstall(app)
                    }
                }
            }

            Item {
                id: installFromFilePanel
                Layout.fillWidth: true
                Layout.preferredHeight: 144
                opacity: 0.3

                Canvas {
                    anchors.fill: parent
                    antialiasing: true

                    onPaint: function (rect) {
                        var radius = 10
                        var ctx = getContext("2d")
                        ctx.save()
                        ctx.setLineDash([5, 5])
                        ctx.beginPath()
                            ctx.moveTo(0,0)
                            ctx.lineTo(rect.width, 0)
                            ctx.lineTo(rect.width, rect.height)
                            ctx.lineTo(0, rect.height)
                            ctx.lineTo(0, 0)
                            ctx.closePath()
                        ctx.strokeStyle = "#1af6d6"
                        ctx.stroke()
                        ctx.restore()
                        }
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 6

                    SvgImage {
                        Layout.alignment:   Qt.AlignVCenter
                        Layout.leftMargin:  20
                        source:             "qrc:/assets/icon-add-green.svg"
                        sourceSize:         Qt.size(18, 18)
                    }

                    SFText {
                        Layout.alignment:    Qt.AlignVCenter
                        Layout.fillWidth:    true
                        font {
                            styleName: "Normal"
                            weight:    Font.Normal
                            pixelSize: 14
                        }
                        color: Style.active
                        wrapMode: Text.WordWrap
                        //% "Install DApp from file"
                        text: qsTrId("apps-install-from-file")
                    }
                }

                MouseArea {
                    id:               clickArea
                    anchors.fill:     parent
                    acceptedButtons:  Qt.LeftButton
                    hoverEnabled:     true
                    cursorShape:      Qt.PointingHandCursor
                    onClicked:        onOpenDnd()//dndDialog.open()//control.install("")
                }
            }
        }
    }
}