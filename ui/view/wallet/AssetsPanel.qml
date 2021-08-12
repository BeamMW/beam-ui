import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    AssetsViewModel {
        id: viewModel

        onSelectedAssetChanged: function () {
            control.updateView()
        }
    }

    Connections {
        target: viewModel.assets
        function onDataChanged () {
            control.updateView()
        }
    }

    function updateView () {
        control.assetsCount = viewModel.assets.rowCount()

        if (selectedId >= 0) {
            var roleid = viewModel.assets.getRoleId("id")
            for (var idx = 0; idx < control.assetsCount; ++idx) {
                var modelIdx = viewModel.assets.index(idx, 0);
                var data = viewModel.assets.data(modelIdx, 258)

                if (selectedId >=0 && selectedId == data) {
                    // currently selected asset is still present
                    return
                }
            }
        }

        // there is no previously selected asset
        // reset selection to nothing
        selectedId  = -1
    }

    property real   hSpacing:        10
    property real   vSpacing:        10
    property int    maxVisibleRows:  3
    property alias  selectedId:      viewModel.selectedAsset
    property int    assetsCount:     1
    property bool   showFaucetPromo: viewModel.showFaucetPromo
    property bool   isSeedValidated: viewModel.isSeedValidated
    property bool   hideSeedValidationPromo: viewModel.hideSeedValidationPromo

    readonly property real  itemHeight:  75

    readonly property real itemWidth: {
        if (assetsCount == 1 && !showFaucetPromo) return (control.availableWidth - control.hSpacing) / (assetsCount + 1)
        return 220
    }

    readonly property real connectWidth: {
        return control.availableWidth - (control.itemWidth + control.hSpacing) * control.assetsCount
    }

    readonly property int gridColumns: {
        var avail = control.availableWidth
        var cnt = 0

        while (avail >= control.itemWidth) {
            avail -= control.itemWidth + control.hSpacing
            cnt++
        }

        return cnt
    }

    readonly property int gridRows: {
        var modelLength = control.assetsCount
        var gridCols    = control.gridColumns
        var rowsCnt     = Math.floor(modelLength / gridCols) + (modelLength % gridCols ? 1 : 0)
        return rowsCnt
    }

    readonly property bool hasScroll: {
        return control.gridRows > control.maxVisibleRows
    }

    readonly property real scrollViewHeight: {
        return control.hasScroll ? control.itemHeight * 3 + control.vSpacing * 2 : grid.implicitHeight
    }

    contentItem: ScrollView {
        id: scroll

        implicitHeight: !isSeedValidated && !hideSeedValidationPromo && (showFaucetPromo || control.assetsCount > 1) ? control.scrollViewHeight + 95 : control.scrollViewHeight
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: control.hasScroll && hovered ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

        clip: true
        hoverEnabled: true

        Grid {
            id: grid

            columnSpacing: control.hSpacing
            rowSpacing:    control.vSpacing
            columns:       control.gridColumns

            Repeater {
                model: viewModel.assets
                delegate: RowLayout {
                    Layout.fillWidth: (model.index +  1) % grid.columns == 0
                    spacing: 0

                    AssetInfo {
                        implicitHeight: control.itemHeight
                        implicitWidth:  control.itemWidth
                        assetInfo:      model
                        selected:       model.id == control.selectedId
                        opacity:        control.selectedId < 0 ? 1 : (selected ? 1 : 0.6)
                        layer.enabled:  model.verified

                        onClicked: function () {
                            control.selectedId = control.selectedId == model.id ? -1 : model.id
                        }
                    }

                    Item {
                       Layout.fillWidth: true
                       visible: control.assetsCount > 1
                    }
                }
            }

            Panel {
                width:  683
                height: 75
                visible: showFaucetPromo && control.assetsCount == 1

                content: RowLayout {
                    SFText {
                        Layout.topMargin:    -12
                        font.pixelSize:      14
                        color:               Style.content_main
                        //% "See the wallet in action. Get a small amount of Beams from the Faucet DAPP."
                        text:                qsTrId("faucet-promo")
                    }
                    Item {
                        Layout.preferredWidth: openFaucet.width + 10 + openFaucetIcon.width
                        height: 16
                        Layout.topMargin:    -12
                        Layout.rightMargin:  20
                        SvgImage {
                            id: openFaucetIcon
                            anchors.left: parent.left
                            anchors.top: parent.top
                            source: "qrc:/assets/icon-receive-skyblue.svg"
                        }
                        SFText {
                            id: openFaucet
                            anchors.right: parent.right
                            anchors.top: parent.top
                            font.pixelSize:      14
                            font.styleName:      "Bold"
                            font.weight:         Font.Bold
                            color:               Style.accent_incoming
                            //% "get coins"
                            text:                qsTrId("faucet-promo-get-coins")
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                main.openFaucet();
                            }
                            hoverEnabled: true
                        }
                    }
                    Item {
                        width:  16
                        height: 16
                        Layout.topMargin: -50
                        Layout.rightMargin: -9
                        SvgImage {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            source: "qrc:/assets/icon-cancel-white.svg"
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                viewModel.showFaucetPromo = false;
                            }
                            hoverEnabled: true
                        }
                    }
                }
            }
        }

        Row {
            id: seedValidationRow
            width: showFaucetPromo || control.assetsCount > 1 ? parent.width : parent.width / 2 - 5
            topPadding: showFaucetPromo || control.assetsCount > 1 ? grid.height + 10 : 0
            leftPadding: showFaucetPromo || control.assetsCount > 1 ? 0 : itemWidth + 10
            visible: !isSeedValidated && !hideSeedValidationPromo

            Panel {
                width: parent.width
                height: 75
                backgroundColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.1)

                content: RowLayout {
                    SFText {
                        Layout.topMargin:    -22
                        Layout.fillWidth:    !(showFaucetPromo || control.assetsCount > 1)
                        horizontalAlignment: Text.AlignHCenter
                        height: 32
                        font.pixelSize:      14
                        color:               Style.content_main
                        //% "Write down and validate your seed phrase so you can always recover your funds."
                        text:                qsTrId("seed-validation-promo")
                        wrapMode:            Text.WordWrap
                    }

                    SeedValidationHelper { id: seedValidationHelper }

                    SFText {
                        Layout.topMargin:    -22
                        Layout.fillWidth:    !(showFaucetPromo || control.assetsCount > 1)
                        Layout.leftMargin:   10
                        font.pixelSize:      14
                        color:               Style.active
                        //% "Secure your phrase"
                        text:                qsTrId("seed-validation-link")
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                seedValidationHelper.isSeedValidatiomMode = true;
                                main.parent.setSource("qrc:/start.qml");
                            }
                            hoverEnabled: true
                        }
                    }

                    Item {
                        height: 16
                        Layout.fillWidth: true
                        Layout.minimumWidth: showFaucetPromo ? 0 : 25
                    }
                    Item {
                        width:  16
                        height: 16
                        Layout.topMargin:   -55
                        Layout.rightMargin: -9
                        visible: viewModel.canHideSeedValidationPromo
                        SvgImage {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            source: "qrc:/assets/icon-cancel-white.svg"
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                viewModel.hideSeedValidationPromo = true;
                            }
                            hoverEnabled: true
                        }
                    }
                }
            }
        }
    }
}
