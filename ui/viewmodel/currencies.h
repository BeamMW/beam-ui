// Copyright 2018 The Beam Team
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
#include <QObject>
#include "ui_helpers.h"

class OldWalletCurrency: public QObject
{
    Q_OBJECT
public:
    enum class OldCurrency {
        CurrStart = -1,
#define MACRO(name, label, slabel, subunut, feeLabel, dec) Curr##name,
        CURRENCY_MAP(MACRO)
#undef MACRO
        CurrEnd
    };
    Q_ENUM(OldCurrency)
};

beam::wallet::Currency convertCurrencyToExchangeCurrency(OldWalletCurrency::OldCurrency currency);
beam::wallet::AtomicSwapCoin convertCurrencyToSwapCoin(OldWalletCurrency::OldCurrency currency);
OldWalletCurrency::OldCurrency convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin);

beamui::Currencies convertCurrency(OldWalletCurrency::OldCurrency value);
bool isEthereumBased(OldWalletCurrency::OldCurrency currency);
