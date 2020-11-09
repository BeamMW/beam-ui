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

#include "swap_eth_client_model.h"

#include "model/app_model.h"
#include "wallet/core/common.h"
#include "wallet/transactions/swaps/common.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bitcoin_core_017.h"
#include "wallet/transactions/swaps/bridges/bitcoin/settings_provider.h"

using namespace beam;

namespace
{
    const int kBalanceUpdateInterval = 10 * 1000; // 10 seconds
    const int kFeeRateUpdateInterval = 60 * 1000; // 1 minute
}

SwapEthClientModel::SwapEthClientModel(beam::ethereum::IBridgeHolder::Ptr bridgeHolder,
    std::unique_ptr<beam::ethereum::SettingsProvider> settingsProvider,
    io::Reactor& reactor)
    : ethereum::Client(bridgeHolder, std::move(settingsProvider), reactor)
    , m_balanceTimer(this)
    , m_feeRateTimer(this)
{
    qRegisterMetaType<beam::ethereum::Client::Status>("beam::ethereum::Client::Status");
    qRegisterMetaType<beam::ethereum::IBridge::ErrorType>("beam::ethereum::IBridge::ErrorType");
    qRegisterMetaType<beam::Amount>("beam::Amount");

    connect(&m_balanceTimer, SIGNAL(timeout()), this, SLOT(requestBalance()));
    connect(&m_feeRateTimer, SIGNAL(timeout()), this, SLOT(requestEstimatedFeeRate()));

    // connect to myself for save values in UI(main) thread
    connect(this, SIGNAL(gotBalance(beam::Amount)), this, SLOT(setBalance(beam::Amount)));
    connect(this, SIGNAL(gotEstimatedGasPrice(beam::Amount)), this, SLOT(setEstimatedGasPrice(beam::Amount)));
    connect(this, SIGNAL(gotStatus(beam::ethereum::Client::Status)), this, SLOT(setStatus(beam::ethereum::Client::Status)));
    connect(this, SIGNAL(gotCanModifySettings(bool)), this, SLOT(setCanModifySettings(bool)));
    connect(this, SIGNAL(gotConnectionError(beam::ethereum::IBridge::ErrorType)), this, SLOT(setConnectionError(beam::ethereum::IBridge::ErrorType)));

    requestBalance();
    requestEstimatedFeeRate();

    m_balanceTimer.start(kBalanceUpdateInterval);
    m_feeRateTimer.start(kFeeRateUpdateInterval);

    GetAsync()->GetStatus();
}

beam::Amount SwapEthClientModel::getAvailable()
{
    return m_balance;
}

beam::Amount SwapEthClientModel::getGasPrice()
{
    return m_gasPrice;
}

void SwapEthClientModel::OnStatus(Status status)
{
    emit gotStatus(status);
}

beam::ethereum::Client::Status SwapEthClientModel::getStatus() const
{
    return m_status;
}

bool SwapEthClientModel::canModifySettings() const
{
    return m_canModifySettings;
}

beam::ethereum::IBridge::ErrorType SwapEthClientModel::getConnectionError() const
{
    return m_connectionError;
}

void SwapEthClientModel::OnBalance(Amount balance)
{
    emit gotBalance(balance);
}

void SwapEthClientModel::OnEstimatedGasPrice(Amount gasPrice)
{
    emit gotEstimatedGasPrice(gasPrice);
}

void SwapEthClientModel::OnCanModifySettingsChanged(bool canModify)
{
    emit gotCanModifySettings(canModify);
}

void SwapEthClientModel::OnChangedSettings()
{
    requestBalance();
    requestEstimatedFeeRate();
}

void SwapEthClientModel::OnConnectionError(beam::ethereum::IBridge::ErrorType error)
{
    emit gotConnectionError(error);
}

void SwapEthClientModel::requestBalance()
{
    if (GetSettings().IsActivated())
    {
        // update balance
        GetAsync()->GetBalance();
    }
}

void SwapEthClientModel::requestEstimatedFeeRate()
{
    if (GetSettings().IsActivated())
    {
        // update estimated fee rate
        GetAsync()->EstimateGasPrice();
    }
}

void SwapEthClientModel::setBalance(Amount balance)
{
    if (m_balance != balance)
    {
        m_balance = balance;
        emit balanceChanged();
    }
}

void SwapEthClientModel::setEstimatedGasPrice(const beam::Amount gasPrice)
{
    if (m_gasPrice != gasPrice)
    {
        m_gasPrice = gasPrice;
        emit estimatedFeeRateChanged();
    }
}

void SwapEthClientModel::setStatus(beam::ethereum::Client::Status status)
{
    if (m_status != status)
    {
        m_status = status;
        emit statusChanged();
    }
}

void SwapEthClientModel::setCanModifySettings(bool canModify)
{
    if (m_canModifySettings != canModify)
    {
        m_canModifySettings = canModify;
        emit canModifySettingsChanged();
    }
}

void SwapEthClientModel::setConnectionError(beam::ethereum::IBridge::ErrorType error)
{
    if (m_connectionError != error)
    {
        m_connectionError = error;
        emit connectionErrorChanged();
    }
}
