pragma Singleton
import QtQuick

// Maps a beam::wallet::slatepack::Error code to a translated, user-facing reason.
// Keep in sync with the enum in wallet/core/slatepack.h.
QtObject {
    readonly property int none:               0
    readonly property int notSlatepack:       1
    readonly property int badEncoding:        2
    readonly property int badChecksum:        3
    readonly property int unsupportedVersion: 4
    readonly property int unknownType:        5
    readonly property int badPayload:         6
    readonly property int unsupportedType:    7
    readonly property int notForThisWallet:   8
    readonly property int readOnlyWallet:     9
    readonly property int handlerAddress:     10
    readonly property int noPendingImport:    11

    function text(code) {
        switch (code) {
        case notSlatepack:
            //% "This text does not contain a Slatepack."
            return qsTrId("slatepack-error-not-slatepack")
        case badEncoding:
        case badPayload:
            //% "This Slatepack is damaged and cannot be read. Ask the sender for a fresh copy."
            return qsTrId("slatepack-error-damaged")
        case badChecksum:
            //% "This Slatepack failed its integrity check. It was probably truncated or altered in transit."
            return qsTrId("slatepack-error-checksum")
        case unsupportedVersion:
            //% "This Slatepack was created by an incompatible wallet version."
            return qsTrId("slatepack-error-version")
        case unknownType:
        case unsupportedType:
            //% "This wallet does not support this kind of Slatepack."
            return qsTrId("slatepack-error-unsupported-type")
        case notForThisWallet:
            //% "This Slatepack is addressed to a different wallet."
            return qsTrId("slatepack-error-not-yours")
        case readOnlyWallet:
            //% "This wallet cannot decrypt Slatepacks because it has no private key."
            return qsTrId("slatepack-error-read-only")
        case handlerAddress:
            //% "This Slatepack cannot be reviewed before it is applied."
            return qsTrId("slatepack-error-not-reviewable")
        case noPendingImport:
            //% "This Slatepack is no longer waiting for confirmation. Please paste it again."
            return qsTrId("slatepack-error-no-pending")
        }
        //% "This Slatepack could not be imported."
        return qsTrId("slatepack-error-generic")
    }
}
