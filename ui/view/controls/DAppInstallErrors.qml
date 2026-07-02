pragma Singleton

import QtQuick
import Beam.Wallet 1.0

// Maps AppsViewModel::DAppInstallError codes to translatable, user-facing text.
// Shared by the manual install dialog (DnDdappInstallDialog) and the DApp store
// install popup so both paths show the same specific reasons.
QtObject {
    function text(errorCode) {
        switch (errorCode) {
        case ApplicationsViewModel.CantOpenFile:
            //% "Couldn't open the DApp file. It may be corrupted."
            return qsTrId("dapp-install-err-cant-open")
        case ApplicationsViewModel.CantReadManifest:
            //% "Couldn't read the DApp manifest."
            return qsTrId("dapp-install-err-cant-read-manifest")
        case ApplicationsViewModel.Unsupported:
            //% "This DApp is not supported by your wallet version."
            return qsTrId("dapp-install-err-unsupported")
        case ApplicationsViewModel.InvalidFile:
            //% "Invalid DApp file: the manifest is missing or incomplete."
            return qsTrId("dapp-install-err-invalid")
        case ApplicationsViewModel.AlreadyInstalled:
            //% "This DApp is already installed."
            return qsTrId("dapp-install-err-already-installed")
        case ApplicationsViewModel.FolderPrepFailed:
            //% "Couldn't prepare the installation folder."
            return qsTrId("dapp-install-err-folder")
        case ApplicationsViewModel.ExtractFailed:
            //% "Couldn't extract the DApp files."
            return qsTrId("dapp-install-err-extract")
        default:
            //% "The DApp installation error."
            return qsTrId("dnd-install-fail")
        }
    }
}
