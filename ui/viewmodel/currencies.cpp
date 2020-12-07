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

#include "currencies.h"

beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(Currency currency)
{
    switch (currency)
    {
    case Currency::CurrBitcoin:
        return beam::wallet::AtomicSwapCoin::Bitcoin;
    case Currency::CurrLitecoin:
        return beam::wallet::AtomicSwapCoin::Litecoin;
    case Currency::CurrQtum:
        return beam::wallet::AtomicSwapCoin::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case Currency::CurrBitcoinCash:
        return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
#endif // BITCOIN_CASH_SUPPORT
    case Currency::CurrDash:
        return beam::wallet::AtomicSwapCoin::Dash;
    case Currency::CurrDogecoin:
        return beam::wallet::AtomicSwapCoin::Dogecoin;
    case Currency::CurrEthereum:
        return beam::wallet::AtomicSwapCoin::Ethereum;
    case Currency::CurrDai:
        return beam::wallet::AtomicSwapCoin::Dai;
    case Currency::CurrTether:
        return beam::wallet::AtomicSwapCoin::Tether;
    case Currency::CurrWrappedBTC:
        return beam::wallet::AtomicSwapCoin::WBTC;
    default:
        return beam::wallet::AtomicSwapCoin::Unknown;
    }
}

Currency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin)
{
    switch (swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return Currency::CurrBitcoin;
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return Currency::CurrLitecoin;
    case beam::wallet::AtomicSwapCoin::Qtum:
        return Currency::CurrQtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return Currency::CurrBitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return Currency::CurrDash;
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return Currency::CurrDogecoin;
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return Currency::CurrEthereum;
    case beam::wallet::AtomicSwapCoin::Dai:
        return Currency::CurrDai;
    case beam::wallet::AtomicSwapCoin::Tether:
        return Currency::CurrTether;
    case beam::wallet::AtomicSwapCoin::WBTC:
        return Currency::CurrWrappedBTC;
    default:
        return Currency::CurrEnd;
    }
}

beamui::Currencies convertCurrency(Currency value)
{
    switch (value)
    {
#define MACRO(name, label, slabel, subunite, feeLabel, dec) \
    case Currency::Curr##name:\
        return beamui::Currencies::name;
        CURRENCY_MAP(MACRO)
#undef MACRO
    default:
        return beamui::Currencies::Unknown;
    }
}

bool isEthereumBased(Currency currency)
{
    switch (currency)
    {
    case Currency::CurrEthereum: return true;
    case Currency::CurrDai: return true;
    case Currency::CurrTether: return true;
    case Currency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}