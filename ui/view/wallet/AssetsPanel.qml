import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Beam.Wallet 1.0
import "../controls"

Control {
    id: control

    AssetsViewModel {
        id: viewModel
    }

    property int itemHeight:     67
    property int itemWidth:      220
    property int hSpacing:       10
    property int vSpacing:       10
    property int maxVisibleRows: 3

    function gridColumns () {
        var avail = control.availableWidth
        var cnt = 0

        while (avail >= control.itemWidth) {
            avail -= control.itemWidth + control.hSpacing
            cnt++
        }

        return cnt
    }

    function gridRows () {
        var modelLength = viewModel.assets.rowCount()
        var gridCols = gridColumns()
        return Math.floor(modelLength / gridCols) + (modelLength % gridCols ? 1 : 0)
    }

    function hasScroll () {
        return gridRows() > control.maxVisibleRows
    }

    function scrollViewHeight () {
        return hasScroll() ? control.itemHeight * 3 + control.vSpacing * 2 : grid.implicitHeight
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

            implicitHeight: scrollViewHeight()
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: hasScroll() && hovered ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

            clip: true
            hoverEnabled: true

            GridLayout {
                id: grid

                Layout.fillWidth: true
                columnSpacing:    control.hSpacing
                rowSpacing:       control.vSpacing
                columns:          gridColumns()

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
                            height: control.itemHeight
                            width:  control.itemWidth

                            inTxCnt:   model.inTxCnt
                            outTxCnt:  model.outTxCnt
                            amount:    model.amount
                            symbol:    model.name

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

                            rate:      "0.25"
                            symbol2:   "USD"
                            icon:      "qrc:/assets/icon-beam.svg"
                         }

                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
