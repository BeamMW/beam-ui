// Copyright 2019 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "swap_utils.h"
#include "viewmodel/qml_globals.h"

namespace swapui
{
    QString getSwapFeeTitle(OldCurrency currency)
    {
        if (currency == OldCurrency::CurrBeam)
        {
            //% "BEAM Transaction fee"
            return qtTrId("beam-transaction-fee");
        }

        const auto label = QMLGlobals::getCurrencyName(currency);
        //% "%1 Transaction fee rate"
        return qtTrId("general-fee-rate").arg(label);
    }

    QList<QMap<QString, QVariant>> getUICurrList()
    {
        QList<QMap<QString, QVariant>> result;

        auto append = [&result] (OldCurrency curr, const QString& icon) {
            QMap<QString, QVariant> info;

            info.insert("isBEAM",     curr == OldCurrency::CurrBeam);
            info.insert("unitName",    QMLGlobals::getCurrencyUnitName(curr));
            info.insert("icon",        icon);
            info.insert("iconWidth",  22);
            info.insert("iconHeight", 22);
            info.insert("decimals",    QMLGlobals::getCurrencyDecimals(curr));

            result.push_back(info);
        };

        append(OldCurrency::CurrBeam,       "qrc:/assets/icon-beam.svg");
        append(OldCurrency::CurrBitcoin,    "qrc:/assets/icon-btc.svg");
        append(OldCurrency::CurrLitecoin,   "qrc:/assets/icon-ltc.svg");
        append(OldCurrency::CurrQtum,       "qrc:/assets/icon-qtum.svg");
        // TODO disable BCH
        //append(WalletCurrency::Currency::CurrBitcoinCash,    "qrc:/assets/icon-bch.svg");
        append(OldCurrency::CurrDogecoin,   "qrc:/assets/icon-doge.svg");
        append(OldCurrency::CurrDash,       "qrc:/assets/icon-dash.svg");
        append(OldCurrency::CurrEthereum,   "qrc:/assets/icon-eth.svg");
        append(OldCurrency::CurrDai,        "qrc:/assets/icon-dai.svg");
        append(OldCurrency::CurrUsdt,       "qrc:/assets/icon-usdt.svg");
        append(OldCurrency::CurrWrappedBTC, "qrc:/assets/icon-wbtc.svg");

        return result;
    }
}
