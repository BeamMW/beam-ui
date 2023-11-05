import QtQuick 2.11
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Beam.Wallet 1.0
import "."
import "../controls"
import "../utils.js" as Utils

Item {
    function next() {
        if (nextButton.enabled) {
            nextButton.clicked();
        }
    }
    Keys.onReturnPressed: {
        next();
    }
    Keys.onEnterPressed:{
        next();
    }

    Rectangle {
        id: selectBackground
        color: Style.background_main
        anchors.fill: parent
    }

    ColumnLayout {
        id: selectDBColumn
        anchors.fill: parent
        anchors.topMargin: 50

        SFText {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Qt.AlignHCenter
            //% "Select the wallet database file"
            text: qsTrId("general-select-db")
            color: Style.content_main
            font.pixelSize: 36
        }

        CustomTableView {
            id: tableView
            mainBackgroundRect: selectBackground
            property int rowHeight: 44
            property int minWidth: 894
            property int textLeftMargin: 20
            Layout.alignment: Qt.AlignHCenter 
            Layout.topMargin: 50
            Layout.bottomMargin: 9
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: headerHeight + 3*rowHeight
            Layout.maximumHeight: headerHeight + 5*rowHeight
            Layout.minimumWidth: minWidth
            Layout.maximumWidth: minWidth
            frameVisible: false
            selectionMode: SelectionMode.SingleSelection
            backgroundVisible: false
            model: viewModel.walletDBpaths

            TableViewColumn {
                role: "fullPath"
                //% "Name"
                title: qsTrId("start-select-db-thead-name")
                width: 350
                movable: false
                delegate: Item {
                    width: parent.width
                    height: tableView.rowHeight
                    clip:true
                                

                    SFLabel {
                        id: pathLabel
                        property bool isPreferred: (viewModel.walletDBpaths && viewModel.walletDBpaths[styleData.row]) ? viewModel.walletDBpaths[styleData.row].isPreferred : false
                        property string preferredLabelFormat: "<style>span {color: '#00f6d2';}</style><span>%1</span>"
                        //: start screen, select db for migration, best match label 
                        //% "(best match)"
                        property string bestMatchStr: qsTrId("start-select-db-best-match-label")

                        font.pixelSize: 14
                        anchors.left: parent.left
                        anchors.leftMargin: tableView.textLeftMargin
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        textFormat: Text.RichText 
                        text: elidedText(styleData.value, isPreferred) + (isPreferred ? " " + preferredLabelFormat.arg(bestMatchStr) : " ")
                        color: Style.content_main
                        copyMenuEnabled: true
                        onCopyText: BeamGlobals.copyToClipboard(text)
                        Component.onCompleted: {
                            if (isPreferred) {
                                tableView.selection.select(styleData.row);
                                tableView.currentRow = styleData.row;
                            }
                        }
                        function elidedText(str, isPreferred){
                            var textMetricsTemplate = 'import QtQuick 2.11; TextMetrics{font{family: "Proxima Nova";styleName: "Regular";weight: Font.Normal;pixelSize: 14;}elide: Text.ElideLeft;elideWidth: parent.width - tableView.textLeftMargin;text: "%1"}';
                            var fullTextStr = isPreferred ? str + " " + pathLabel.bestMatchStr: str;
                            var textMetrics= Qt.createQmlObject(
                                    textMetricsTemplate.arg(fullTextStr),
                                    pathLabel,
                                    "textMetrics");
                            var elidedCount = fullTextStr.length - textMetrics.elidedText.length;
                            return elidedCount ? "…" + str.substr(elidedCount + 3, str.length) : str;
                        }
                    }
                }
            }

            TableViewColumn {
                role: "fileSize"
                //% "Size"
                title: qsTrId("start-select-db-thead-size")
                width: 120
                movable: false
                delegate: Item {
                    width: parent.width
                    height: tableView.rowHeight
                    clip:true

                    SFLabel {
                        font.pixelSize: 14
                        anchors.left: parent.left
                        anchors.leftMargin: tableView.textLeftMargin
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        //% "kb"
                        text: Math.round(styleData.value / 1024) + " " + qsTrId("kb-unit")
                        color: Style.content_main
                    }
                }
            }

            TableViewColumn {
                role: "creationDateString"
                //: start screen, select db for migration, Date created column title
                //% "Date created"
                title: qsTrId("start-select-db-thead-created")
                width: 145 
                movable: false
            }

            TableViewColumn {
                role: "lastWriteDateString"
                //: start screen, select db for migration, Date modified column title
                //% "Date modified"
                title: qsTrId("start-select-db-thead-modified")
                width: 145 
                movable: false
            }

            TableViewColumn {
                id: actionColumn
                role: "fullPath"
                width: tableView.getAdjustedColumnWidth(actionColumn)
                movable: false
                delegate: Item {
                    width: parent.width
                    height: tableView.rowHeight
                    clip:true

                    LinkButton {
                        fontSize: 14
                        anchors.left: parent.left
                        anchors.leftMargin: tableView.textLeftMargin
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        //% "Show in folder"
                        text: qsTrId("general-show-in-folder")
                        onClicked: {
                            viewModel.openFolder(styleData.value);
                        }
                    }
                }
            }

            rowDelegate: Item {
                height: tableView.rowHeight
                anchors.left: parent.left
                anchors.right: parent.right

                Rectangle {
                    anchors.fill: parent
                    color: styleData.selected ? Style.row_selected :
                            (styleData.alternate ? Style.background_row_even : Style.background_row_odd)
                }
            }

            itemDelegate: TableItem {
                elide: Text.ElideRight
                clip:true

                SFLabel {
                    font.pixelSize: 14
                    anchors.left: parent.left
                    anchors.leftMargin: tableView.textLeftMargin
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: styleData.value
                    color: Style.content_main
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 16
        }

        LinkButton {
            Layout.alignment: Qt.AlignHCenter
            //% "Find the wallet database file manually"
            text: qsTrId("restore-find-db")
            fontSize: 14
            onClicked: {
                var path = viewModel.selectCustomWalletDB();
                if (path.length > 0) {
                    migrateWalletDB(path);
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: 32
        }

        Row {
            id: buttons
            Layout.alignment: Qt.AlignHCenter
            spacing: 30

            CustomButton {
                text: qsTrId("general-back")
                icon.source: "qrc:/assets/icon-back.svg"
                visible: startWizzardView.depth > 1
                onClicked: {
                    startWizzardView.pop();
                }
            }

            PrimaryButton {
                id: nextButton
                //% "Next"
                text: qsTrId("general-next")
                icon.source: "qrc:/assets/icon-next-blue.svg"
                enabled: tableView.currentRow >= 0
                onClicked: {
                    migrateWalletDB(viewModel.walletDBpaths[tableView.currentRow].fullPath);
                }
            }
        }

        Item {
            Layout.minimumHeight: 30
            Layout.preferredHeight: 100
        }

        Item {
            Layout.fillHeight: true
            Layout.minimumHeight: Utils.isSqueezedHeight(selectDBColumn.height) ? 40 : 60
            Layout.maximumHeight: Utils.isSqueezedHeight(selectDBColumn.height) ? 70 : 90
        }

        VersionFooter {}
    }
}