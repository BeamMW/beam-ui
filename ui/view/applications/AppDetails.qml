import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Beam.Wallet 1.0
import "../controls"

CustomDialog {
    id: dialog

    property var app: undefined

    modal: true
    width: 600
    height: 400
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    parent: Overlay.overlay
    padding: 0

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    header: ColumnLayout {
        SFText {
            Layout.topMargin: 30
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 18
            font.styleName: "Bold";
            font.weight: Font.Bold
            color: Style.content_main
            //% "Application details"
            text: qsTrId("dapp-store-app-details")
        }
    }

    contentItem: ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        component FieldTitle : SFText {
            font.pixelSize: 14
            color: Style.content_secondary
        }

        component FieldValue : RowLayout {
            property alias text: label.text
            SFLabel {
                id: label
                Layout.fillWidth:   true
                copyMenuEnabled:    true
                font.pixelSize:     14
                color:              Style.content_main
                elide:              Text.ElideMiddle
                onCopyText:         BeamGlobals.copyToClipboard(text)
            }
            CustomToolButton {
                Layout.alignment:   Qt.AlignRight
                icon.source:        "qrc:/assets/icon-copy.svg"
                onClicked:          BeamGlobals.copyToClipboard(label.text)
                padding:            0
                background.implicitHeight: 16
                visible:            label.text.length > 0
            }
        }

        GridLayout {
            id: grid
            Layout.leftMargin: 30
            Layout.rightMargin: 30
            Layout.topMargin: 30 
            Layout.alignment: Qt.AlignTop
            columnSpacing: 40
            rowSpacing: 14
            columns: 2
            
            FieldTitle {
                //% "Name"
                text: qsTrId("dapp-details-name") + ":"
            }
            FieldValue {
                text:   !!app && !!app.name ? app.name:""
            }

            FieldTitle {
                //% "Description"
                text: qsTrId("dapp-details-description") + ":"
            }
            FieldValue {
                text:   !!app && !!app.description ? app.description:""
            }
            
            FieldTitle {
                //% "Version:" 
                text: qsTrId("dapps-store-dapp-version")
            }
            FieldValue {
                text: !!app && !!app.version ? app.version:""
            }

            FieldTitle {
                //% "Release date"
                text: qsTrId("dapp-details-release-date") + ":"
            }
            FieldValue {
                text:   !!app && !!app.release_date ? app.release_date:""
            }
            
            FieldTitle {
                //% "Guid:"
                text: qsTrId("dapps-store-dapp-guid") + ":"
            }
/*            FieldValue: {
                text: !!app && !!app.guid ? app.guid:""}
*/            
            FieldTitle {
                //% "Publisher key"
                text: qsTrId("dapp-details-publisher-key") + ":"
            }
            FieldValue {
                text:   !!app && !!app.publisher ? app.publisher : ""
            }

            FieldTitle {
                //% "Publisher name"
                text: qsTrId("dapp-details-publisher-name") + ":"
            }
            FieldValue {
                text:   !!app && !!app.publisherName ? app.publisherName : ""
            }

            FieldTitle {
                //% "Category"
                text: qsTrId("dapp-details-category") + ":"
            }
            FieldValue {
                text:   !!app && !!app.categoryName ? app.categoryName : ""
            }
//Name, "name") \
//Description, "description") \
//IpfsId, "ipfs_id") \
//Url, "url") \
//ApiVersion, "api_version") \
//MinApiVersion, "min_api_version") \
//Guid, "guid") \
//Id, "id") \
//PublisherKey, "publisher") \
//PublisherName, "publisherName") \
//Category, "category") \
//CategoryName, "categoryName") \
//CategoryColor, "categoryColor") \
//Supported, "supported") \
//NotInstalled, "notInstalled") \
//Icon, "icon") \
//Version, "version") \
//FullPath, "fullPath") \
//Appid, "appid") \
//Major, "major") \
//Minor, "minor") \
//Release, "release") \
//Build, "build") \
//DevApp, "devApp") \
//HasUpdate, "hasUpdate") \
//ReleaseDate, "release_date") \
//Local, "local") \

        }
    }

    footer: ColumnLayout {
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 30
            Layout.topMargin: 30
            spacing: 20

            CustomButton {
                //% "Close"
                text:               qsTrId("general-close")
                icon.source:        "qrc:/assets/icon-cancel-16.svg"
                onClicked:          dialog.close()
            }
        }
    }
}
