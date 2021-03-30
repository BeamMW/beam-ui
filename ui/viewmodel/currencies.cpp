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

beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(OldCurrency currency)
{
    switch (currency)
    {
    case OldCurrency::CurrBitcoin:
        return beam::wallet::AtomicSwapCoin::Bitcoin;
    case OldCurrency::CurrLitecoin:
        return beam::wallet::AtomicSwapCoin::Litecoin;
    case OldCurrency::CurrQtum:
        return beam::wallet::AtomicSwapCoin::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case OldCurrency::CurrBitcoinCash:
        return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
#endif // BITCOIN_CASH_SUPPORT
    case OldCurrency::CurrDash:
        return beam::wallet::AtomicSwapCoin::Dash;
    case OldCurrency::CurrDogecoin:
        return beam::wallet::AtomicSwapCoin::Dogecoin;
    case OldCurrency::CurrEthereum:
        return beam::wallet::AtomicSwapCoin::Ethereum;
    case OldCurrency::CurrDai:
        return beam::wallet::AtomicSwapCoin::Dai;
    case OldCurrency::CurrUsdt:
        return beam::wallet::AtomicSwapCoin::Usdt;
    case OldCurrency::CurrWrappedBTC:
        return beam::wallet::AtomicSwapCoin::WBTC;
    default:
        return beam::wallet::AtomicSwapCoin::Unknown;
    }
}

OldCurrency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin)
{
    switch (swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return OldCurrency::CurrBitcoin;
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return OldCurrency::CurrLitecoin;
    case beam::wallet::AtomicSwapCoin::Qtum:
        return OldCurrency::CurrQtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return OldCurrency::CurrBitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return OldCurrency::CurrDash;
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return OldCurrency::CurrDogecoin;
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return OldCurrency::CurrEthereum;
    case beam::wallet::AtomicSwapCoin::Dai:
        return OldCurrency::CurrDai;
    case beam::wallet::AtomicSwapCoin::Usdt:
        return OldCurrency::CurrUsdt;
    case beam::wallet::AtomicSwapCoin::WBTC:
        return OldCurrency::CurrWrappedBTC;
    default:
        return OldCurrency::CurrEnd;
    }
}

beamui::Currencies convertCurrency(OldCurrency value)
{
    switch (value)
    {
#define MACRO(name, label, slabel, subunite, feeLabel, dec) \
    case OldCurrency::Curr##name:\
        return beamui::Currencies::name;
        CURRENCY_MAP(MACRO)
#undef MACRO
    default:
        return beamui::Currencies::Unknown;
    }
}

bool isEthereumBased(OldCurrency currency)
{
    switch (currency)
    {
    case OldCurrency::CurrEthereum: return true;
    case OldCurrency::CurrDai: return true;
    case OldCurrency::CurrUsdt: return true;
    case OldCurrency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}
