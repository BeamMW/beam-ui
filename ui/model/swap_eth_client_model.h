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
#include <QTimer>
#include "wallet/transactions/swaps/bridges/ethereum/client.h"

class SwapEthClientModel
    : public QObject
    , public beam::ethereum::Client
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<SwapEthClientModel>;

    SwapEthClientModel(beam::ethereum::IBridgeHolder::Ptr bridgeHolder,
        std::unique_ptr<beam::ethereum::SettingsProvider> settingsProvider,
        beam::io::Reactor& reactor);

    beam::Amount getAvailable(beam::wallet::AtomicSwapCoin swapCoin) const;
    beam::Amount getGasPrice() const;
    beam::ethereum::Client::Status getStatus() const;
    bool canModifySettings() const;
    beam::ethereum::IBridge::ErrorType getConnectionError() const;

signals:
    void gotStatus(beam::ethereum::Client::Status status);
    void gotBalance(beam::wallet::AtomicSwapCoin swapCoin, beam::Amount balance);
    void gotEstimatedGasPrice(beam::Amount estimatedFeeRate);
    void gotCanModifySettings(bool canModify);
    void gotConnectionError(const beam::ethereum::IBridge::ErrorType& error);

    void canModifySettingsChanged();
    void balanceChanged();
    void estimatedFeeRateChanged();
    void statusChanged();
    void connectionErrorChanged();

private:
    void OnStatus(Status status) override;
    void OnBalance(beam::wallet::AtomicSwapCoin swapCoin, beam::Amount balance) override;
    void OnEstimatedGasPrice(beam::Amount gasPrice) override;
    void OnCanModifySettingsChanged(bool canModify) override;
    void OnChangedSettings() override;
    void OnConnectionError(beam::ethereum::IBridge::ErrorType error) override;

private slots:
    void requestBalance();
    void requestEstimatedFeeRate();
    void setBalance(beam::wallet::AtomicSwapCoin swapCoin, beam::Amount balance);
    void setEstimatedGasPrice(beam::Amount gasPrice);
    void setStatus(beam::ethereum::Client::Status status);
    void setCanModifySettings(bool canModify);
    void setConnectionError(beam::ethereum::IBridge::ErrorType error);

private:
    QTimer m_balanceTimer;
    QTimer m_feeRateTimer;
    std::map<beam::wallet::AtomicSwapCoin, beam::Amount> m_balances;
    beam::Amount m_gasPrice = 0;
    Status m_status = Status::Unknown;
    bool m_canModifySettings = true;
    beam::ethereum::IBridge::ErrorType m_connectionError = beam::ethereum::IBridge::ErrorType::None;
};