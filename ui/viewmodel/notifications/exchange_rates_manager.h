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
#pragma once

#include <QObject>
#include <QDateTime>
#include "model/wallet_model.h"
#include "model/settings.h"
#include "wallet/client/extensions/news_channels/interface.h"

class ExchangeRatesManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime updateTime READ getUpdateTime  NOTIFY updateTimeChanged)
public:
    ExchangeRatesManager();

    [[nodiscard]] beam::Amount getRate(const beam::wallet::Currency&) const;
    [[nodiscard]] beam::wallet::Currency getRateCurrency() const;
    [[nodiscard]] QDateTime getUpdateTime() const;
    [[nodiscard]] bool isUpToDate() const;

public slots:
    void onExchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>& rates);
    void onRateUnitChanged();

signals:
    void rateUnitChanged();
    void activeRateChanged();
    void updateTimeChanged();

private:
    void setRateUnit();
    void setUpdateTime(beam::Timestamp value);

    WalletModel& m_walletModel;
    WalletSettings& m_settings;

    beam::wallet::Currency m_rateUnit = beam::wallet::Currency::UNKNOWN();
    std::map<beam::wallet::Currency, beam::Amount> m_rates;
    beam::Timestamp m_updateTime;
};
