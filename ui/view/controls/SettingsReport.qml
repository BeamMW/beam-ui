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
        spacing: 25
        height:  reportText.height + spacing + reportButton.height

        SFText {
            id: reportText
            Layout.alignment: Qt.AlignTop
            property string beamEmail: "<a href='mailto:support@beam.mw'>support@beam.mw</a>"
            property string beamGithub: "<a href='https://github.com/BeamMW'>Github</a>"
            //% "To report a problem:"
            property string rpm0: qsTrId("settings-report-problem-message-l0")
            //% "1. Click “Save wallet logs” and choose a destination folder for log archive"
            property string rpm1: qsTrId("settings-report-problem-message-l1")
            //% "2. Send email to %1 or open a ticket in %2"
            property string rpm2: qsTrId("settings-report-problem-message-l2").arg(beamEmail).arg(beamGithub)
            //% "3. Don’t forget to attach logs archive"
            property string rpm3: qsTrId("settings-report-problem-message-l3")
            Layout.preferredWidth: parent.width
            text: Style.linkStyle + rpm0 + "<br />" + rpm1 + "<br />" + rpm2 + "<br />" + rpm3
            textFormat: Text.RichText
            color: Style.content_main
            font.pixelSize: 14
            wrapMode: Text.WordWrap
            linkEnabled: true
            onLinkActivated: {
                Utils.openExternalWithConfirmation(link)
            }
        }
        CustomButton {
            id: reportButton
            Layout.preferredHeight: 38
            Layout.alignment: Qt.AlignCenter
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
