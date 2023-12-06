#!/usr/bin/env bash
# set -e

# Notarize the dmg
notarize_dmg() {(
  echo "Uploading $BEAM_WALLET_UI_IN to notarization service"
  xcrun notarytool submit "$BEAM_WALLET_UI_IN" --apple-id "$MACOS_NOTARIZE_USER" --team-id "$MACOS_NOTARIZE_PROVIDER" --password "$MACOS_NOTARIZE_PASS" --verbose --wait
  xcrun stapler staple "$BEAM_WALLET_UI_IN"
)}

notarize_dmg
