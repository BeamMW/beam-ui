import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    AssetsViewModel {
        id: viewModel
    }

    property real   hSpacing:       10
    property real   vSpacing:       10
    property int    maxVisibleRows: 3
    property int    selectedId:     -1
    property int    selectedIdx:    -1
    property alias  folded:         viewModel.folded

    /* Binding {
        target:   viewModel
        property: "folded"
        value:    control.folded
    }
    */

    readonly property int   assetsCount:     viewModel.assets.rowCount()
    readonly property bool  connectVisible:  assetsCount < 3
    readonly property real itemHeight:       67

    readonly property real itemWidth: {
        if (assetsCount == 1) return (control.availableWidth - control.hSpacing) / (assetsCount + 1)
        return 220
    }

    readonly property real connectWidth: {
        return control.availableWidth - (control.itemWidth + control.hSpacing) * control.assetsCount
    }

    readonly property int gridColumns: {
        if (control.connectVisible) {
            return control.assetsCount + 1
        }

        var avail = control.availableWidth
        var cnt = 0

        while (avail >= control.itemWidth) {
            avail -= control.itemWidth + control.hSpacing
            cnt++
        }

        return cnt
    }

    readonly property int gridRows: {
        var modelLength = viewModel.assets.rowCount()
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

    AlphaTip {
        id: tip

        property alias text: tipText.text

        visible: false
        defBkColor: Qt.rgba(55 / 255, 93  / 255, 123 / 255, 0.75)
        defTextColor: Qt.rgba(Style.content_main.r, Style.content_main.g, Style.content_main.b, 0.8)
        z: 100

        contentItem: SFText {
            id:             tipText
            font.pixelSize: 12
            font.styleName: "Light"
            font.weight:    Font.Light
            color:          tip.defTextColor
        }
    }

    contentItem: FoldablePanel {
        id: foldable

        //% "Assets"
        title: qsTrId("wallet-assets-title")

        padding:         0
        background:      null
        folded:          false
        minHeaderHeight: searchBox.implicitHeight

        headerContent: SearchBox {
           id: searchBox
           //% "Asset name"
           placeholderText: qsTrId("wallet-search-asset")
        }

        content: ScrollView {
            id: scroll

            implicitHeight: control.scrollViewHeight
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: control.hasScroll && hovered ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

            clip: true
            hoverEnabled: true

            GridLayout {
                id: grid

                Layout.fillWidth: true
                columnSpacing:    control.hSpacing
                rowSpacing:       control.vSpacing
                columns:          control.gridColumns

                Repeater {
                    model: SortFilterProxyModel {
                        source: viewModel.assets
                        filterRole: "search"
                        filterString: searchBox.text
                        filterSyntax: SortFilterProxyModel.Wildcard
                        filterCaseSensitivity: Qt.CaseInsensitive
                    }

                    delegate: RowLayout {
                        Layout.fillWidth: (model.index +  1) % grid.columns == 0
                        spacing: 0

                        AssetInfo {
                            implicitHeight: control.itemHeight
                            implicitWidth:  control.itemWidth

                            inTxCnt:      model.inTxCnt
                            outTxCnt:     model.outTxCnt
                            amount:       model.amount
                            unitName:     model.unitName
                            selected:     model.index == control.selectedIdx
                            icon:         model.icon
                            color:        model.color
                            borderColor:  model.selectionColor
                            unitName2:    model.unitName2
                            rate:         model.rate
                            opacity:      control.selectedIdx < 0  ? 1 : (model.index == control.selectedIdx ? 1 : 0.6)

                            onTip: function (show, text, iRight, iBtm) {
                                tip.visible = show
                                tip.text    = text

                                tip.x       = iRight - tip.width
                                tip.y       = iBtm + 6

                                var pr = this
                                while (pr != tip.parent) {
                                    tip.x += pr.x
                                    tip.y += pr.y
                                    pr = pr.parent
                                }
                            }

                            onClicked: function () {
                                if (control.selectedIdx == model.index) {
                                    control.selectedIdx = -1
                                    control.selectedId = -1
                                } else {
                                    control.selectedIdx = model.index
                                    control.selectedId = model.id
                                }
                            }
                        }

                        Item {
                           Layout.fillWidth: true
                           visible: viewModel.assets.rowCount() > 1
                        }
                    }
                }

                AssetsConnect {
                    Layout.preferredWidth:   control.connectWidth
                    Layout.preferredHeight:  control.itemHeight
                    visible:                 control.connectVisible

                    onClicked: function () {
                        BeamGlobals.showMessage("'Connect asset' action is not implemented yet")
                    }
                }
            }
        }
    }
}
