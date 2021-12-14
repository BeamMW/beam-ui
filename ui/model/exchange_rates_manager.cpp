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

#include "model/exchange_rates_manager.h"

#include "model/app_model.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"

// test
#include "utility/logger.h"

ExchangeRatesManager::ExchangeRatesManager(WalletModel::Ptr wallet, WalletSettings& settings)
    : _wallet(std::move(wallet))
    , _settings(settings)
    , m_updateTime(0)
{

    qRegisterMetaType<std::vector<beam::wallet::ExchangeRate>>("std::vector<beam::wallet::ExchangeRate>");

    connect(_wallet.get(),
            SIGNAL(exchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&)),
            SLOT(onExchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&)));

    connect(&_settings,
            SIGNAL(secondCurrencyChanged()),
            SLOT(onRateUnitChanged()));

    m_rateUnit = _settings.getRateCurrency();
    if (m_rateUnit != beam::wallet::Currency::UNKNOWN())
    {
        _wallet->getAsync()->getExchangeRates();
    }
}

void ExchangeRatesManager::setRateUnit()
{
    auto newCurrency = _settings.getRateCurrency();
    if (m_rateUnit == newCurrency)
        return;

    auto turnedOn = newCurrency != beam::wallet::Currency::UNKNOWN();
    _wallet->getAsync()->switchOnOffExchangeRates(turnedOn);
    if (turnedOn)
    {
        _wallet->getAsync()->getExchangeRates();
    }
    m_rateUnit = newCurrency;
    setUpdateTime(0);
}

void ExchangeRatesManager::setUpdateTime(beam::Timestamp value)
{
    if (m_updateTime != value)
    {
        m_updateTime = value;
        emit updateTimeChanged();
    }
}

bool ExchangeRatesManager::isUpToDate() const
{
    auto diff = QDateTime::currentSecsSinceEpoch() - m_updateTime;
    return diff < 10 * 60; // 10 minutes
}

void ExchangeRatesManager::onExchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>& rates)
{
    if (m_rateUnit == beam::wallet::Currency::UNKNOWN()) return;  /// Second currency is turned OFF

    bool isActiveRateChanged = false;
    for (const auto& rate : rates)
    {
        if (rate.m_to != m_rateUnit) 
            continue;

        m_rates[rate.m_from] = rate.m_rate;
        if (m_updateTime < rate.m_updateTime)
        {
            m_updateTime = rate.m_updateTime;
            emit updateTimeChanged();
        }
        isActiveRateChanged = true;
    }

    if (isActiveRateChanged)
    {
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

QDateTime ExchangeRatesManager::getUpdateTime() const
{
    QDateTime datetime;
    datetime.setTime_t(m_updateTime);
    return datetime;
}

/**
 *  Get an exchange rate for specific @currency.
 */
beam::Amount ExchangeRatesManager::getRate(const beam::wallet::Currency& currency) const
{
    const auto it = m_rates.find(currency);
    return (it == std::cend(m_rates)) ? 0 : it->second;
}
