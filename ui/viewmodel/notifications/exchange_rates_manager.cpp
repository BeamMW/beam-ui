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

#include "exchange_rates_manager.h"

#include "model/app_model.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"

// test
#include "utility/logger.h"

ExchangeRatesManager::ExchangeRatesManager()
    : m_walletModel(*AppModel::getInstance().getWalletModel())
    , m_settings(AppModel::getInstance().getSettings())
{

    qRegisterMetaType<std::vector<beam::wallet::ExchangeRate>>("std::vector<beam::wallet::ExchangeRate>");

    connect(&m_walletModel,
            SIGNAL(exchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&)),
            SLOT(onExchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&)));

    connect(&m_settings,
            SIGNAL(secondCurrencyChanged()),
            SLOT(onRateUnitChanged()));

    m_rateUnit = m_settings.getRateCurrency();
    if (m_rateUnit != beam::wallet::Currency::UNKNOWN())
    {
        m_walletModel.getAsync()->getExchangeRates();
    }
}

void ExchangeRatesManager::setRateUnit()
{
    auto newCurrency = m_settings.getRateCurrency();

    if (newCurrency == beam::wallet::Currency::UNKNOWN() && m_rateUnit != newCurrency)
    {
        m_walletModel.getAsync()->switchOnOffExchangeRates(false);
    }
    else
    {
        if (m_rateUnit == beam::wallet::Currency::UNKNOWN())
        {
            m_walletModel.getAsync()->switchOnOffExchangeRates(true);
        }
        if (m_rateUnit != newCurrency)
        {
            m_walletModel.getAsync()->getExchangeRates();
        }
    }
    m_rateUnit = newCurrency;
}

void ExchangeRatesManager::onExchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>& rates)
{
    if (m_rateUnit == beam::wallet::Currency::UNKNOWN()) return;  /// Second currency is turned OFF

    bool isActiveRateChanged = false;
    for (const auto& rate : rates)
    {
        {
            beam::wallet::PrintableAmount amount(rate.m_rate, true /*show decimal point*/);
            LOG_DEBUG() << "Exchange rate: 1 " << rate.m_from.m_value << " = "
                        << amount << " " << rate.m_to.m_value;
        }

        if (rate.m_to != m_rateUnit) continue;
        m_rates[rate.m_from] = rate.m_rate;
        isActiveRateChanged = true;
    }

    if (isActiveRateChanged) {
        emit activeRateChanged();
    }
}

void ExchangeRatesManager::onRateUnitChanged()
{
    setRateUnit();
    emit rateUnitChanged();
}

beam::wallet::Currency ExchangeRatesManager::getRateCurrency() const
{
    return m_rateUnit;
}

/**
 *  Get an exchange rate for specific @currency.
 */
beam::Amount ExchangeRatesManager::getRate(const beam::wallet::Currency& currency) const
{
    const auto it = m_rates.find(currency);
    return (it == std::cend(m_rates)) ? 0 : it->second;
}

beam::wallet::Currency ExchangeRatesManager::convertCurrencyToExchangeCurrency(OldWalletCurrency::OldCurrency uiCurrency)
{
    switch (uiCurrency)
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
