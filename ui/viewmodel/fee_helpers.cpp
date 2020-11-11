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
#include "wallet/transactions/swaps/bridges/bitcoin_cash/bitcoin_cash_side.h"
#include "wallet/transactions/swaps/bridges/dash/dash_side.h"
#include "wallet/transactions/swaps/bridges/dogecoin/dogecoin_side.h"
#include "wallet/transactions/swaps/utils.h"

beam::Amount minFeeBeam(bool isShielded)
{
    return isShielded ? beam::wallet::kShieldedTxMinFeeInGroth : beam::wallet::kMinFeeInGroth;
}

bool isFeeOK(beam::Amount fee, Currency currency, bool isShielded)
{
    switch (currency)
    {
    case Currency::CurrBeam: return fee >= minFeeBeam(isShielded);
    case Currency::CurrBitcoin:  return true;
    case Currency::CurrLitecoin:  return true;
    case Currency::CurrQtum: return true;
    case Currency::CurrBitcoinCash: return true;
    case Currency::CurrDash: return true;
    case Currency::CurrDogecoin: return true;
    default:
        return false;
    }
}

bool isSwapFeeOK(beam::Amount amount, beam::Amount fee, Currency currency)
{
    if (Currency::CurrBeam == currency)
    {
        return amount > fee && fee >= minFeeBeam();
    }

    // TODO roman.strilets maybe need to process exception?
    return beam::wallet::IsSwapAmountValid(convertCurrencyToSwapCoin(currency), amount, fee);
}

beam::Amount minimalFee(Currency currency, bool isShielded)
{
    if (Currency::CurrBeam == currency)
    {
        return minFeeBeam(isShielded);
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->GetSettings().GetMinFeeRate();
}

QString calcTotalFee(Currency currency, beam::Amount feeRate)
{
    switch (currency) {
    case Currency::CurrBeam: {
        return QString::fromStdString(std::to_string(feeRate));
    }
    case Currency::CurrBitcoin: {
        auto total = beam::wallet::BitcoinSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
    case Currency::CurrLitecoin: {
        auto total = beam::wallet::LitecoinSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " ph";
    }
    case Currency::CurrQtum: {
        auto total = beam::wallet::QtumSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " qsat";
    }
    case Currency::CurrBitcoinCash: {
        auto total = beam::wallet::BitcoinCashSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
    case Currency::CurrDash: {
        auto total = beam::wallet::DashSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " duff";
    }
    case Currency::CurrDogecoin: {
        auto total = beam::wallet::DogecoinSide::CalcTotalFee(feeRate);
        return QString::fromStdString(std::to_string(total)) + " sat";
    }
    default: {
        return QString();
    }
    }
}