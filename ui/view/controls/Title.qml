import QtQuick 2
import QtQuick.Window 2
import QtQuick.Layouts 1
import "."

RowLayout {
    default property alias content: contentLayout.data
    property alias text: title.text

    Layout.fillHeight:   false

    implicitWidth:       title.implicitWidth + logoutButton.implicitWidth
    implicitHeight:      title.implicitHeight + logoutButton.implicitHeight

    SFText {
        id:                 title
        Layout.leftMargin:  10
        font.pixelSize:     36
        color:              Style.content_main
    }
    RowLayout {
        id:                 contentLayout
        Layout.fillWidth:   true
        Layout.alignment:   Qt.AlignVCenter
    }
    LogoutButton{
        id: logoutButton
    }
}