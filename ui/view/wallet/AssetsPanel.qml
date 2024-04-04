import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
    property bool   showSettingLink:false

    property real   assetsFilterRowHeight: 40

    property bool  showValidationPromo: viewModel.showSeedValidationPromo && !seedValidationHelper.isSeedValidated

    property int minimalItemWidth:  220

    readonly property real itemWidth: {
        var assetsCount = control.showSelected ? control.selectedIds.length : control.assetsCount
        if (assetsCount == 1 && !showFaucetPromo) return (control.availableWidth - control.hSpacing) / (assetsCount + 1)
        if (assetsCount == 1) return minimalItemWidth
        let colums = control.gridColumns
        return ((control.availableWidth + control.hSpacing) / colums) - control.hSpacing
    }

    readonly property int gridColumns: {
        let extraWidth = control.availableWidth + control.hSpacing
        let count = Math.floor(extraWidth / (minimalItemWidth + control.hSpacing))
        return count
    }

    readonly property int gridRows: {
        var modelLength = control.showSelected ? control.selectedIds.length : control.assetsCount
        var gridCols    = control.gridColumns
        var rowsCnt     = Math.floor(modelLength / gridCols) + (modelLength % gridCols ? 1 : 0)
        return rowsCnt
    }

    readonly property bool hasScroll: {
        return control.gridRows > control.maxVisibleRows
    }

    readonly property real scrollContentHeight: {
        return grid.implicitHeight + (control.showValidationPromo && (showFaucetPromo || control.assetsCount > 1) ? 95 : 0)
    }

    readonly property real scrollViewHeight: {
        return control.hasScroll
            ? control.itemHeight * control.maxVisibleRows + control.vSpacing * (control.maxVisibleRows - 1)
            : control.scrollContentHeight
    }

    topPadding: assetsFilterRowHeight
    RowLayout {
        id: assetsFilter
        width: parent.width
        spacing: 0

        SFText {
            font {
                pixelSize: 14
                letterSpacing: 3.11
                styleName: "DemiBold"; weight: Font.DemiBold
                capitalization: Font.AllUppercase
            }

            opacity: 0.5
            color: Style.content_main
            //% "Assets"
            text: qsTrId("wallet-assets-title")
        }

        LinkButton {
            Layout.leftMargin:  30
            Layout.alignment:   Qt.AlignVCenter
            visible: showSettingLink
            //% "Assets settings"
            text: qsTrId("assets-settings")
            onClicked: {
                main.openSettings("CA");
            }
        }

        Item {
            Layout.fillWidth:   true
            Layout.fillHeight:  true
        }

        SvgImage {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/assets/icon-cancel-white.svg"
            sourceSize: Qt.size(16, 16)
            visible: control.selectedIds.length
            MouseArea {
                anchors.fill:      parent
                acceptedButtons:   Qt.LeftButton
                onClicked:         {
                    control.clearSelectedAssets()
                    showSelected = false
                }
                hoverEnabled:      true
                cursorShape: Qt.PointingHandCursor
            }
        }

        LinkButton {
            Layout.leftMargin: 15
            Layout.rightMargin: 32
            //% "remove filter"
            text: qsTrId("wallet-remove-filter-button")
            linkColor: Style.content_main
            visible: control.selectedIds.length
            bold: true
            onClicked: {
                control.clearSelectedAssets()
                showSelected = false
            }
        }

        SFText {
            //% "Selected"
            text: qsTrId("wallet-selected-assets-checkbox")
            color: showSelected ? Style.active : Style.content_secondary
            font.pixelSize: 14
            opacity: control.selectedIds.length == 0 ? 0.2 : 1
        }

        CustomSwitch {
            id: assetsFilterSwitch
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            checkable: control.selectedIds.length != 0
            checked: control.selectedIds.length == 0 ? true : !showSelected
            alwaysGreen: true
            leftPadding: 0
            rightPadding: 0
            spacing: 0

            Binding {
                target: control
                property: "showSelected"
                value: !assetsFilterSwitch.checked
            }
        }

        SFText {
            Layout.rightMargin: 10
            //% "All"
            text: qsTrId("wallet-all-assets-checkbox")
            color: showSelected ? Style.content_secondary : Style.active
            font.pixelSize: 14
        }
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
            height: grid.implicitHeight + (control.showValidationPromo ? 85 : 0)
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
            }

            Row {
                width:       scroll.width - 5
                visible:     control.showValidationPromo && control.assetsCount > 1

                SeedValidationPanel {
                    canHideValidationPromo: viewModel.canHideValidationPromo
                    itemHeight: 75
                    onShowSeedValidationPromoOff: function() {
                        viewModel.showSeedValidationPromo = false
                    }
                }
            }
        }
    }

    Row {
        width:       parent.width / 2 - 5
        leftPadding: itemWidth + 10
        topPadding:  50
        visible:     control.showValidationPromo && control.assetsCount == 1

        SeedValidationPanel {
            canHideValidationPromo: viewModel.canHideValidationPromo
            itemHeight: 75
            onShowSeedValidationPromoOff: function() {
                viewModel.showSeedValidationPromo = false
            }
        }
    }
}