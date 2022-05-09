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

        if (selectedIds.length != 0) {
            var roleid = viewModel.assets.getRoleId("id")
            for (var idx = 0; idx < control.assetsCount; ++idx) {
                var modelIdx = viewModel.assets.index(idx, 0)
                var data = viewModel.assets.data(modelIdx, 258)
            }
        }
    }

    function clearSelectedAssets(assets) {
        viewModel.clearSelectedAssets(assets)
    }

    Component.onCompleted: function() {
        control.updateView()
    }

    SeedValidationHelper { id: seedValidationHelper }

    property real   hSpacing:       10
    property real   vSpacing:       10
    property int    maxVisibleRows: 3
    property alias  selectedIds:    viewModel.selectedAssets
    property int    assetsCount:    1
    property real   itemHeight:     75
    property bool   showSelected:   false
    property bool   selectable:     true

    property bool  showFaucetPromo: viewModel.showFaucetPromo
    property bool  showValidationPromo: viewModel.showValidationPromo && !seedValidationHelper.isSeedValidated

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

    readonly property real scrollContentHeight: {
        return grid.implicitHeight + (showValidationPromo && (showFaucetPromo || control.assetsCount > 1) ? 95 : 0)
    }

    readonly property real scrollViewHeight: {
        return control.hasScroll
            ? control.itemHeight * control.maxVisibleRows + control.vSpacing * (control.maxVisibleRows - 1)
            : control.scrollContentHeight
    }

    contentItem: ScrollView {
        id: scroll

        implicitHeight: control.scrollViewHeight
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: control.hasScroll && hovered ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

        clip: true
        hoverEnabled: true

        Column {
            spacing: 10
            height: grid.implicitHeight + (showValidationPromo ? 95 : 0)
            width: parent.width
            Grid {
                id: grid

                columnSpacing: control.hSpacing
                rowSpacing:    control.vSpacing
                columns:       control.gridColumns
                width:         parent.width

                Repeater {
                    model: viewModel.assets
                    delegate: RowLayout {
                        Layout.fillWidth: (model.index +  1) % grid.columns == 0
                        spacing: 0

                        AssetInfo {
                            implicitHeight: control.itemHeight
                            implicitWidth:  control.itemWidth
                            assetInfo:      model
                            visible:        !showSelected || control.selectedIds.indexOf(model.id) != -1
                            selected:       control.selectable && control.selectedIds.indexOf(model.id) != -1
                            opacity:        control.selectable && control.selectedIds.length != 0 ? (control.selectedIds.indexOf(model.id) != -1 ? 1 : 0.6) : 1
                            layer.enabled:  model.verified

                            onClicked: function () {
                                viewModel.addAssetToSelected(model.id)
                            }
                        }

                        Item {
                        Layout.fillWidth: true
                        visible: control.assetsCount > 1
                        }
                    }
                }

                Panel {
                    width:   scroll.width - control.hSpacing - control.itemWidth
                    height:  control.itemHeight
                    visible: showFaucetPromo && control.assetsCount == 1

                    content: RowLayout {
                        spacing: 14
                        SFText {
                            Layout.topMargin: -12
                            font.pixelSize:    14
                            color:             Style.content_main
                            //% "See the wallet in action. Get a small amount of Beams from the Faucet DApp."
                            text: qsTrId("faucet-promo")
                        }
                        Item {
                            Layout.preferredWidth: openFaucet.width + 6 + openFaucetIcon.width
                            height: 16
                            Layout.topMargin:   -12
                            Layout.rightMargin: 10
                            SvgImage {
                                id: openFaucetIcon
                                source: "qrc:/assets/icon-receive-skyblue.svg"
                            }
                            SFText {
                                id: openFaucet
                                x:  openFaucetIcon.width + 6
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
                            Layout.fillWidth: true
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
                                onClicked: function () {
                                    viewModel.showFaucetPromo = false;
                                }
                                hoverEnabled: true
                            }
                        }
                    }
                }
            }

            Row {
                width:       parent.width - (showFaucetPromo ? 0 : 5)
                visible:     showValidationPromo && (control.showFaucetPromo || control.assetsCount > 1)

                SeedValidationPanel {
                    canHideValidationPromo: viewModel.canHideValidationPromo
                    itemHeight: 75
                    onShowSeedValidationPromoOff: function() {
                        viewModel.showSeedValidationPromo = false
                    }
                    showFaucetPromo: viewModel.showFaucetPromo
                }
            }
        }
    }

    Row {
        width:       parent.width / 2 - 5
        leftPadding: itemWidth + 10
        visible:     showValidationPromo && !control.showFaucetPromo && control.assetsCount == 1

        SeedValidationPanel {
            canHideValidationPromo: viewModel.canHideValidationPromo
            itemHeight: 75
            onShowSeedValidationPromoOff: function() {
                viewModel.showSeedValidationPromo = false
            }
            showFaucetPromo: viewModel.showFaucetPromo
        }
    }
}
