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
#pragma once

#include <QString>
#include <functional>
#include "viewmodel/currencies.h"
#include "wallet/core/common.h"

namespace swapui
{
    QString getSwapFeeTitle(OldWalletCurrency::OldCurrency currency);
    QList<QMap<QString, QVariant>> getUICurrList();

    // ERC-20 symbol from the offer/tx parameters, "ERC20" when absent
    QString erc20Symbol(const beam::wallet::TxParameters& params);
    // token amount in wallet units, optionally suffixed with the symbol
    QString erc20AmountString(const beam::wallet::TxParameters& params, beam::Amount value, bool withSymbol);
    // true when @currency is the pair side a custom ERC-20 token rides on
    bool isTokenSide(OldWalletCurrency::OldCurrency currency, const QString& tokenContract);
    // token side -> tokenWalletDecimals, otherwise the classic per-currency table
    uint8_t effectiveSwapDecimals(OldWalletCurrency::OldCurrency currency, const QString& tokenContract, uint32_t tokenDecimals);
    // connect every swap-coin client's estimatedFeeRateChanged to @onChanged
    void connectFeeRateClients(QObject* receiver, const std::function<void()>& onChanged);
    // the ETH balance covers a token lock's gas (incl. the approve calls) at @feeRate
    bool enoughEthForTokenLock(beam::Amount feeRate);
}
