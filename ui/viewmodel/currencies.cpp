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

beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(OldWalletCurrency::OldCurrency currency)
{
    switch (currency)
    {
    case OldWalletCurrency::OldCurrency::CurrBitcoin:
        return beam::wallet::AtomicSwapCoin::Bitcoin;
    case OldWalletCurrency::OldCurrency::CurrLitecoin:
        return beam::wallet::AtomicSwapCoin::Litecoin;
    case OldWalletCurrency::OldCurrency::CurrQtum:
        return beam::wallet::AtomicSwapCoin::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case OldWalletCurrency::OldCurrency::CurrBitcoinCash:
        return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
#endif // BITCOIN_CASH_SUPPORT
    case OldWalletCurrency::OldCurrency::CurrDash:
        return beam::wallet::AtomicSwapCoin::Dash;
    case OldWalletCurrency::OldCurrency::CurrDogecoin:
        return beam::wallet::AtomicSwapCoin::Dogecoin;
    case OldWalletCurrency::OldCurrency::CurrEthereum:
        return beam::wallet::AtomicSwapCoin::Ethereum;
    case OldWalletCurrency::OldCurrency::CurrDai:
        return beam::wallet::AtomicSwapCoin::Dai;
    case OldWalletCurrency::OldCurrency::CurrUsdt:
        return beam::wallet::AtomicSwapCoin::Usdt;
    case OldWalletCurrency::OldCurrency::CurrWrappedBTC:
        return beam::wallet::AtomicSwapCoin::WBTC;
    default:
        return beam::wallet::AtomicSwapCoin::Unknown;
    }
}

OldWalletCurrency::OldCurrency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin)
{
    switch (swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return OldWalletCurrency::OldCurrency::CurrBitcoin;
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return OldWalletCurrency::OldCurrency::CurrLitecoin;
    case beam::wallet::AtomicSwapCoin::Qtum:
        return OldWalletCurrency::OldCurrency::CurrQtum;
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return OldWalletCurrency::OldCurrency::CurrBitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return OldWalletCurrency::OldCurrency::CurrDash;
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return OldWalletCurrency::OldCurrency::CurrDogecoin;
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return OldWalletCurrency::OldCurrency::CurrEthereum;
    case beam::wallet::AtomicSwapCoin::Dai:
        return OldWalletCurrency::OldCurrency::CurrDai;
    case beam::wallet::AtomicSwapCoin::Usdt:
        return OldWalletCurrency::OldCurrency::CurrUsdt;
    case beam::wallet::AtomicSwapCoin::WBTC:
        return OldWalletCurrency::OldCurrency::CurrWrappedBTC;
    default:
        return OldWalletCurrency::OldCurrency::CurrEnd;
    }
}

beamui::Currencies convertCurrency(OldWalletCurrency::OldCurrency value)
{
    switch (value)
    {
#define MACRO(name, label, slabel, subunite, feeLabel, dec) \
    case OldWalletCurrency::OldCurrency::Curr##name:\
        return beamui::Currencies::name;
        CURRENCY_MAP(MACRO)
#undef MACRO
    default:
        return beamui::Currencies::Unknown;
    }
}

bool isEthereumBased(OldWalletCurrency::OldCurrency currency)
{
    switch (currency)
    {
    case OldWalletCurrency::OldCurrency::CurrEthereum: return true;
    case OldWalletCurrency::OldCurrency::CurrDai: return true;
    case OldWalletCurrency::OldCurrency::CurrUsdt: return true;
    case OldWalletCurrency::OldCurrency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}

beam::wallet::Currency convertCurrencyToExchangeCurrency(OldWalletCurrency::OldCurrency currency)
{
    switch (currency)
    {
        case OldWalletCurrency::OldCurrency::CurrBeam:
            return beam::wallet::Currency::BEAM();
        case OldWalletCurrency::OldCurrency::CurrBitcoin:
            return beam::wallet::Currency::BTC();
        case OldWalletCurrency::OldCurrency::CurrLitecoin:
            return beam::wallet::Currency::LTC();
        case OldWalletCurrency::OldCurrency::CurrQtum:
            return beam::wallet::Currency::QTUM();
        case OldWalletCurrency::OldCurrency::CurrDash:
            return beam::wallet::Currency::DASH();
        case OldWalletCurrency::OldCurrency::CurrDogecoin:
            return beam::wallet::Currency::DOGE();
        case OldWalletCurrency::OldCurrency::CurrEthereum:
            return beam::wallet::Currency::ETH();
        case OldWalletCurrency::OldCurrency::CurrDai:
            return beam::wallet::Currency::DAI();
        case OldWalletCurrency::OldCurrency::CurrUsdt:
            return beam::wallet::Currency::USDT();
        case OldWalletCurrency::OldCurrency::CurrWrappedBTC:
            return beam::wallet::Currency::WBTC();
        default:
            return beam::wallet::Currency::UNKNOWN();
    }
}
