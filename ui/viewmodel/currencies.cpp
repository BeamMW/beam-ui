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

beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(WalletCurrency::Currency currency)
{
    switch (currency)
    {
    case WalletCurrency::Currency::CurrBitcoin:
        return beam::wallet::AtomicSwapCoin::Bitcoin;
    case WalletCurrency::Currency::CurrLitecoin:
        return beam::wallet::AtomicSwapCoin::Litecoin;
    case WalletCurrency::Currency::CurrQtum:
        return beam::wallet::AtomicSwapCoin::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case WalletCurrency::Currency::CurrBitcoinCash:
        return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
#endif // BITCOIN_CASH_SUPPORT
    case WalletCurrency::Currency::CurrDash:
        return beam::wallet::AtomicSwapCoin::Dash;
    case WalletCurrency::Currency::CurrDogecoin:
        return beam::wallet::AtomicSwapCoin::Dogecoin;
    case WalletCurrency::Currency::CurrEthereum:
        return beam::wallet::AtomicSwapCoin::Ethereum;
    case WalletCurrency::Currency::CurrDai:
        return beam::wallet::AtomicSwapCoin::Dai;
    case WalletCurrency::Currency::CurrUsdt:
        return beam::wallet::AtomicSwapCoin::Usdt;
    case WalletCurrency::Currency::CurrWrappedBTC:
        return beam::wallet::AtomicSwapCoin::WBTC;
    default:
        return beam::wallet::AtomicSwapCoin::Unknown;
    }
}

WalletCurrency::Currency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin)
{
    switch (swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return WalletCurrency::Currency::CurrBitcoin;
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return WalletCurrency::Currency::CurrLitecoin;
    case beam::wallet::AtomicSwapCoin::Qtum:
        return WalletCurrency::Currency::CurrQtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return Currency::CurrBitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return WalletCurrency::Currency::CurrDash;
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return WalletCurrency::Currency::CurrDogecoin;
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return WalletCurrency::Currency::CurrEthereum;
    case beam::wallet::AtomicSwapCoin::Dai:
        return WalletCurrency::Currency::CurrDai;
    case beam::wallet::AtomicSwapCoin::Usdt:
        return WalletCurrency::Currency::CurrUsdt;
    case beam::wallet::AtomicSwapCoin::WBTC:
        return WalletCurrency::Currency::CurrWrappedBTC;
    default:
        return WalletCurrency::Currency::CurrEnd;
    }
}

beamui::Currencies convertCurrency(WalletCurrency::Currency value)
{
    switch (value)
    {
#define MACRO(name, label, slabel, subunite, feeLabel, dec) \
    case WalletCurrency::Currency::Curr##name:\
        return beamui::Currencies::name;
        CURRENCY_MAP(MACRO)
#undef MACRO
    default:
        return beamui::Currencies::Unknown;
    }
}

bool isEthereumBased(WalletCurrency::Currency currency)
{
    switch (currency)
    {
    case WalletCurrency::Currency::CurrEthereum: return true;
    case WalletCurrency::Currency::CurrDai: return true;
    case WalletCurrency::Currency::CurrUsdt: return true;
    case WalletCurrency::Currency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}
