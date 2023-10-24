pragma Singleton
import QtQuick 2.11
import Beam.Wallet 1.0
import "../color_themes"

AbstractTheme {

	property var themes: QtObject {
		property AbstractTheme masternet: Masternet{}
		property AbstractTheme testnet: Testnet{}
		property AbstractTheme mainnet: Mainnet{}
		property AbstractTheme beamx: BeamX{}
		property AbstractTheme dappnet: Dappnet{}
	}

	property var currentTheme : themes[Theme.name]

	Component.onCompleted: {
		for (var propName in this) {
			if (typeof this[propName] != "function"
				&& propName != "objectName"
				&& propName != "themes"
				&& propName != "currentTheme"
				&& propName != "currentThemeChanged") {
				this[propName] = Qt.binding(makeBinding(propName))
			}
		}
	}

	function makeBinding(prop) {
		const p = prop;
		return function () {
			return currentTheme[p]
		}
	}
}
