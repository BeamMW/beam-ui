import QtQuick 2.11
import QtQuick.Controls 1.2
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import Beam.Wallet 1.0
import "."

SettingsFoldable {
    id: caBlock
    property var viewModel

    //% "Confidential assets"
    title: qsTrId("settings-ca")

    content: ColumnLayout {
        spacing: 10

        RowLayout {
            Layout.preferredHeight: 16
            Layout.fillWidth: true
            SFText {
                Layout.fillWidth: true

                //% "Confidential assets allowed to receive"
                text: qsTrId("settings-ca-descr")
                color: Style.content_secondary
                font.pixelSize: 14
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            clip: true
            model: viewModel.currenciesList
            implicitHeight: count > 6 ? Math.min(220, contentHeight) : contentHeight

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: RowLayout {
                width: listView.width
                height: 36
                SvgImage {
                    source: modelData["icon"]
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.rightMargin: 10
                    Layout.preferredWidth: modelData["iconWidth"]
                    Layout.preferredHeight: modelData["iconHeight"]

                    SvgImage {
                        source: "qrc:/assets/icon-verified-asset.svg";
                        visible: modelData["verified"]

                        x: parent.width - width / 1.6
                        y: - height / 3.6

                        width:  18 * parent.width / 26
                        height: 18 * parent.width / 26
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignLeft
                    width: 80
                    height: 16
                    SFText {
                        text: modelData["unitNameWithId"]
                        font.pixelSize: 14
                        color: Style.content_main
                        height: parent.height
                        width: parent.width
                        elide: Text.ElideMiddle
                    }
                }
                Item {
                    Layout.alignment: Qt.AlignLeft
                    height: 16
                    SFText {
                        text: modelData["emission"]
                        font.pixelSize: 12
                        color: Style.content_secondary
                        height: parent.height
                        width: parent.width
                    }
                }
                Item {
                    Layout.fillWidth: true
                    height: 16
                }

                CustomSwitch {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 10
                    checked: modelData["allowed"]
                    onCheckedChanged: {
                        if (checked) {
                            viewModel.allowAssetId(modelData["assetId"]);
                        } else {
                            viewModel.disallowAssetId(modelData["assetId"]);
                        }
                    }
                }
            }
        }
    }
}
