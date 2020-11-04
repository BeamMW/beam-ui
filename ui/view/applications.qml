import QtQuick         2.11
import QtQuick.Layouts 1.0
import QtWebEngine     1.1
import QtWebChannel    1.0
import Beam.Wallet     1.0
import "controls"

ColumnLayout {
    id: control
    anchors.fill: parent

    ApplicationsViewModel {
        id: viewModel
    }

    //
    // Page Header (Title + Status Bar)
    //
    Title {
        //% "Applications"
        text: qsTrId("apps-title")
    }

    StatusBar {
        id: statusBar
        model: statusbarModel
    }

    //
    // C++ object published to web control & web control itself
    //
    WebAPIBeam {
        id: webapiBEAM
        WebChannel.id: "BEAM"
        property var style: Style
    }

    WebChannel {
        id: apiChannel
        registeredObjects: [webapiBEAM]
    }

    WebEngineView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.bottomMargin: 9
        Layout.topMargin: 34

        id: webView
        url: Style.appsUrl
        webChannel: apiChannel

        // TODO:check why cache doesn't respect headers and always load cached page
        profile.httpCacheType: WebEngineProfile.NoCache
    }

    Component.onCompleted: {
        viewModel.onCompleted(webView)
    }
}
