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
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"
#include "wallet/transactions/swaps/bridges/ethereum/common.h"

namespace swapui
{
    QString getSwapFeeTitle(OldWalletCurrency::OldCurrency currency)
    {
        if (currency == OldWalletCurrency::OldCurrency::CurrBeam)
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

        auto append = [&result] (OldWalletCurrency::OldCurrency curr, const QString& icon) {
            QMap<QString, QVariant> info;

            info.insert("isBEAM",         curr == OldWalletCurrency::OldCurrency::CurrBeam);
            info.insert("unitName",       QMLGlobals::getCurrencyUnitName(curr));
            info.insert("unitNameWithId", QMLGlobals::getCurrencyUnitName(curr));
            info.insert("icon",           icon);
            info.insert("iconWidth",      22);
            info.insert("iconHeight",     22);
            info.insert("decimals",       QMLGlobals::getCurrencyDecimals(curr));

            result.push_back(info);
        };

        append(OldWalletCurrency::OldCurrency::CurrBeam,       "qrc:/assets/icon-beam.svg");
        append(OldWalletCurrency::OldCurrency::CurrBitcoin,    "qrc:/assets/icon-btc.svg");
        append(OldWalletCurrency::OldCurrency::CurrLitecoin,   "qrc:/assets/icon-ltc.svg");
        append(OldWalletCurrency::OldCurrency::CurrQtum,       "qrc:/assets/icon-qtum.svg");
        // TODO disable BCH
        //append(WalletCurrency::Currency::CurrBitcoinCash,    "qrc:/assets/icon-bch.svg");
        append(OldWalletCurrency::OldCurrency::CurrDogecoin,   "qrc:/assets/icon-doge.svg");
        append(OldWalletCurrency::OldCurrency::CurrDash,       "qrc:/assets/icon-dash.svg");
        append(OldWalletCurrency::OldCurrency::CurrEthereum,   "qrc:/assets/icon-eth.svg");
        append(OldWalletCurrency::OldCurrency::CurrDai,        "qrc:/assets/icon-dai.svg");
        append(OldWalletCurrency::OldCurrency::CurrUsdt,       "qrc:/assets/icon-usdt.svg");
        append(OldWalletCurrency::OldCurrency::CurrWrappedBTC, "qrc:/assets/icon-wbtc.svg");

        return result;
    }

    QString erc20Symbol(const beam::wallet::TxParameters& params)
    {
        std::string symbol;
        params.GetParameter(beam::wallet::TxParameterID::AtomicSwapTokenSymbol, symbol);
        return symbol.empty() ? QString("ERC20") : QString::fromStdString(symbol);
    }

    QString erc20AmountString(const beam::wallet::TxParameters& params, beam::Amount value, bool withSymbol)
    {
        uint8_t onChainDecimals = 0;
        params.GetParameter(beam::wallet::TxParameterID::AtomicSwapTokenDecimals, onChainDecimals);
        // exact-decimals variant: tokenWalletDecimals(0) is a valid 0, which the
        // unitName overload would silently replace with the BEAM default
        auto amountStr = beamui::AmountToUIStringExactDecimals(value, beamui::tokenWalletDecimals(onChainDecimals));
        if (!withSymbol)
        {
            return amountStr;
        }
        std::string symbol;
        params.GetParameter(beam::wallet::TxParameterID::AtomicSwapTokenSymbol, symbol);
        return symbol.empty() ? amountStr : amountStr + " " + QString::fromStdString(symbol);
    }

    bool isTokenSide(OldWalletCurrency::OldCurrency currency, const QString& tokenContract)
    {
        return currency == OldWalletCurrency::OldCurrency::CurrEthereum && !tokenContract.isEmpty();
    }

    uint8_t effectiveSwapDecimals(OldWalletCurrency::OldCurrency currency, const QString& tokenContract, uint32_t tokenDecimals)
    {
        if (isTokenSide(currency, tokenContract))
        {
            return beamui::tokenWalletDecimals(tokenDecimals);
        }
        return beamui::getCurrencyDecimals(convertCurrency(currency));
    }

    void connectFeeRateClients(QObject* receiver, const std::function<void()>& onChanged)
    {
        if (auto ethClient = AppModel::getInstance().getSwapEthClient(); ethClient)
        {
            QObject::connect(ethClient.get(), &SwapEthClientModel::estimatedFeeRateChanged, receiver, onChanged);
        }
        for (auto coin : {beam::wallet::AtomicSwapCoin::Bitcoin, beam::wallet::AtomicSwapCoin::Litecoin,
                          beam::wallet::AtomicSwapCoin::Qtum, beam::wallet::AtomicSwapCoin::Dogecoin,
                          beam::wallet::AtomicSwapCoin::Dash, beam::wallet::AtomicSwapCoin::Bitcoin_Cash})
        {
            if (auto client = AppModel::getInstance().getSwapCoinClient(coin); client)
            {
                QObject::connect(client.get(), &SwapCoinClientModel::estimatedFeeRateChanged, receiver, onChanged);
            }
        }
    }

    bool enoughEthForTokenLock(beam::Amount feeRate)
    {
        // no live balance for arbitrary custom tokens: only check the ETH
        // that pays the lock gas (incl. the approve calls, kApproveTxGasLimit)
        const beam::Amount lockFee = feeRate *
            (beam::ethereum::kLockTxGasLimit + 2 * beam::ethereum::kApproveTxGasLimit);
        return AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) >= lockFee;
    }
}
