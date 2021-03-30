// Copyright 2020 The Beam Team
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

#include "fee_helpers.h"

#include "model/app_model.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bitcoin_side.h"
#include "wallet/transactions/swaps/bridges/litecoin/litecoin_side.h"
#include "wallet/transactions/swaps/bridges/qtum/qtum_side.h"
#if defined(BITCOIN_CASH_SUPPORT)
#include "wallet/transactions/swaps/bridges/bitcoin_cash/bitcoin_cash_side.h"
#endif // BITCOIN_CASH_SUPPORT
#include "wallet/transactions/swaps/bridges/dash/dash_side.h"
#include "wallet/transactions/swaps/bridges/dogecoin/dogecoin_side.h"
#include "wallet/transactions/swaps/bridges/ethereum/ethereum_side.h"
#include "wallet/transactions/swaps/utils.h"

beam::Amount minFeeBeam(bool isShielded)
{
    beam::Height h = AppModel::getInstance().getWalletModel()->getCurrentHeight();
    auto& fs = beam::Transaction::FeeSettings::get(h);

    return isShielded ? fs.get_DefaultShieldedOut() : fs.get_DefaultStd();
}

bool isFeeOK(beam::Amount fee, OldCurrency currency, bool isShielded)
{
    switch (currency)
    {
    case OldCurrency::CurrBeam: return fee >= minFeeBeam(isShielded);
    case OldCurrency::CurrBitcoin:  return true;
    case OldCurrency::CurrLitecoin:  return true;
    case OldCurrency::CurrQtum: return true;
#if defined(BITCOIN_CASH_SUPPORT)
    case OldCurrency::CurrBitcoinCash: return true;
#endif // BITCOIN_CASH_SUPPORT
    case OldCurrency::CurrDash: return true;
    case OldCurrency::CurrDogecoin: return true;
    case OldCurrency::CurrEthereum: return true;
    case OldCurrency::CurrDai: return true;
    case OldCurrency::CurrUsdt: return true;
    case OldCurrency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}

bool isSwapFeeOK(beam::Amount amount, beam::Amount fee, OldCurrency currency)
{
    if (OldCurrency::CurrBeam == currency)
    {
        return amount > fee && fee >= minFeeBeam();
    }

    bool result = false;

    try
    {
        result = beam::wallet::IsLockTxAmountValid(convertCurrencyToSwapCoin(currency), amount, fee);
    }
    catch (const std::runtime_error& err)
    {
        LOG_ERROR() << err.what();
    }
    
    return result;
}

beam::Amount minimalFee(OldCurrency currency, bool isShielded)
{
    if (OldCurrency::CurrBeam == currency)
    {
        return minFeeBeam(isShielded);
    }

    if (isEthereumBased(currency))
    {
        return AppModel::getInstance().getSwapEthClient()->GetSettings().GetMinFeeRate();
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->GetSettings().GetMinFeeRate();
}

beam::Amount maximumFee(OldCurrency currency)
{
    if (OldCurrency::CurrBeam == currency)
    {
        // TODO roman.strilets need to investigate
        return 0u;
    }

    if (isEthereumBased(currency))
    {
        return AppModel::getInstance().getSwapEthClient()->GetSettings().GetMaxFeeRate();
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->GetSettings().GetMaxFeeRate();
}

QString calcWithdrawTxFee(OldCurrency currency, beam::Amount feeRate)
{
    switch (currency) {
    case OldCurrency::CurrBeam: {
        return QString::fromStdString(std::to_string(feeRate));
    }
    case OldCurrency::CurrBitcoin: {
        auto total = beam::wallet::BitcoinSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
    case OldCurrency::CurrLitecoin: {
        auto total = beam::wallet::LitecoinSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " ph";
    }
    case OldCurrency::CurrQtum: {
        auto total = beam::wallet::QtumSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " qsat";
    }
#if defined(BITCOIN_CASH_SUPPORT)
    case OldCurrency::CurrBitcoinCash: {
        auto total = beam::wallet::BitcoinCashSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
#endif // BITCOIN_CASH_SUPPORT
    case OldCurrency::CurrDash: {
        auto total = beam::wallet::DashSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " duff";
    }
    case OldCurrency::CurrDogecoin: {
        auto total = beam::wallet::DogecoinSide::CalcWithdrawTxFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
    case OldCurrency::CurrEthereum:
    case OldCurrency::CurrDai:
    case OldCurrency::CurrUsdt:
    case OldCurrency::CurrWrappedBTC: {
        auto swapCoin = convertCurrencyToSwapCoin(currency);
        auto total = beam::wallet::EthereumSide::CalcWithdrawTxFee(feeRate, swapCoin);
        return QString::fromStdString(std::to_string(total)) + " gwei";
    }
    default: {
        return QString();
    }
    }
}