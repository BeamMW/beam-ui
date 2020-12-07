import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.0
import Beam.Wallet 1.0
import "."
import "../utils.js" as Utils

SettingsFoldable {
    id: reportBlock
    property var viewModel
    //% "Report a problem"
    title: qsTrId("settings-report-problem-title")

    content:  ColumnLayout {
        spacing: 3

        SFText {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            //% "1. Click “Save wallet logs” and choose a destination folder for log archive"
            property string rpm1: qsTrId("settings-report-problem-message-l1")
            text: rpm1
            wrapMode:   Text.WordWrap
            color: Style.content_main
            font.pixelSize: 14
        }
        SFText {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            property string beamEmail: "<a href='mailto:support@beam.mw'>support@beam.mw</a>"
            property string beamGithub: "<a href='https://github.com/BeamMW'>Github</a>"
            //% "2. Send email to %1 or open a ticket in %2"
            property string rpm2: qsTrId("settings-report-problem-message-l2").arg(beamEmail).arg(beamGithub)
            text: Style.linkStyle + rpm2
            textFormat: Text.RichText
            wrapMode:   Text.WordWrap
            color: Style.content_main
            font.pixelSize: 14
            linkEnabled: true
            onLinkActivated: {
                Utils.openExternalWithConfirmation(link)
            }
        }
        SFText {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            //% "3. Don’t forget to attach logs archive"
            property string rpm3: qsTrId("settings-report-problem-message-l3")
            text: rpm3
            wrapMode:   Text.WordWrap
            color: Style.content_main
            font.pixelSize: 14
        }
        CustomButton {
            id: reportButton
            Layout.preferredHeight: 38
            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: 22
            //: settings tab, report problem section, save logs button
            //% "Save wallet logs"
            text: qsTrId("settings-report-problem-save-log-button")
            icon.source: "qrc:/assets/icon-save.svg"
            palette.buttonText : "white"
            palette.button: Style.background_button
            onClicked: viewModel.reportProblem()
        }
    }
}
