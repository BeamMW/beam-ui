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
	}

	Component.onCompleted: {
		var currentTheme = themes[Theme.name()]
		if (!currentTheme) {
			currentTheme = themes['masternet'];
		}

		for (var propName in this) {
			if (typeof this[propName] != "function"
				&& propName != "objectName"
				&& propName != "themes") {
				this[propName] = currentTheme[propName]
			}
				
		}
	}
}
