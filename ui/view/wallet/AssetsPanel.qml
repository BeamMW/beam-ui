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

    readonly property real  itemHeight:  75

    readonly property real itemWidth: {
        if (assetsCount == 1) return (control.availableWidth - control.hSpacing) / (assetsCount + 1)
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

        implicitHeight: control.scrollViewHeight
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: control.hasScroll && hovered ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

        clip: true
        hoverEnabled: true

        Grid {
            id: grid

            Layout.fillWidth: true
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

                        onClicked: function () {
                            control.selectedId = control.selectedId == model.id ? -1 : model.id
                        }

                        Component.onCompleted: {
                            BeamGlobals.logInfo("Created for: " + model.id)
                        }
                    }

                    Item {
                       Layout.fillWidth: true
                       visible: control.assetsCount > 1
                    }
                }
            }
        }
    }
}
