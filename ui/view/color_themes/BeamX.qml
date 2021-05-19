import QtQuick 2.11

AbstractTheme {
    property string navigation_logo: "qrc:/assets/beamx-logo.svg"
    property string bottom_background: "qrc:/assets/bottom-logo-beamx.svg"

    property color content_main:          "#ffffff"  // white
    property color accent_outgoing:       "#da68f5"  // heliotrope
    property color accent_incoming:       "#0bccf7"  // bright-sky-blue
    property color accent_swap:           "#39fdf2"
    property color accent_fail:           "#ff746b"
    property color content_secondary:     "#8da1ad"  // bluey-grey
    property color content_disabled:      "#889da9"
    property color content_opposite:      "#032e49"
    property color validator_warning:     "#f4ce4a"
    property color validator_error:       "#ff625c"
    property color section:               Qt.rgba(255, 255, 255, 0.5)

    property color navigation_background:  "#000000"
    property color background_main:        "#000a16"
    property color background_main_top:    "#001f45"
    property color background_second:      Qt.rgba(1, 1, 1, 0.05)

    property color background_row_even:         Qt.rgba(1, 1, 1, 0.03)
    property color background_row_odd:          Qt.rgba(1, 1, 1, 0.05)
    property color background_row_details_even: Qt.rgba(132/255, 94/255, 255/255, 0.3)
    property color background_row_details_odd:  Qt.rgba(132/255, 94/255, 255/255, 0.3)

    property color background_details:     Qt.rgba(19/255, 24/255, 61/255, 0.75)
    property color background_button:      Qt.rgba(1, 1, 1, 0.1)
    property color background_popup:       "#0f1535"
    property color row_selected:           "#085469"
    property color separator:              "#33566b"
    property color table_header:           Qt.rgba(0, 28/255, 66/255, 0.34)

    property color active :               "#00f6d2"
    property color passive:               "#d6d9e0"
    property color caps_warning:          "#000000"
    property color online:                "#00f6d2"
    property string linkStyle:            "<style>a:link {color: '#00f6d2'; text-decoration: none;}</style>"

    property color coinPaneRight:     "#00458f"
    property color coinPaneLeft:      "#00f6d2"
    property color coinPaneBorder:    Qt.rgba(0, 246, 210, 0.15)

    property color swapStateIndicator:  "#ff746b"
    property color swapDisconnectNode:          "#f9605b"

    property int appsGradientOffset: -174
    property int appsGradientTop: 56
}
