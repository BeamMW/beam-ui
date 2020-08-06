// Copyright 2019 The Beam Team
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

#include "swap_coin_client_model.h"

#include "model/app_model.h"
#include "wallet/core/common.h"
#include "wallet/transactions/swaps/common.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bitcoin_core_017.h"
#include "wallet/transactions/swaps/bridges/bitcoin/settings_provider.h"

using namespace beam;

namespace
{
    const int kUpdateInterval = 10000;
    const int kInterval = 60 * 1000; // 1 minute
}

SwapCoinClientModel::SwapCoinClientModel(beam::bitcoin::IBridgeHolder::Ptr bridgeHolder,
    std::unique_ptr<beam::bitcoin::SettingsProvider> settingsProvider,
    io::Reactor& reactor)
    : bitcoin::Client(bridgeHolder, std::move(settingsProvider), reactor)
    , m_balanceTimer(this)
    , m_feeRateTimer(this)
{
    qRegisterMetaType<beam::bitcoin::Client::Status>("beam::bitcoin::Client::Status");
    qRegisterMetaType<beam::bitcoin::Client::Balance>("beam::bitcoin::Client::Balance");
    qRegisterMetaType<beam::bitcoin::IBridge::ErrorType>("beam::bitcoin::IBridge::ErrorType");

    connect(&m_balanceTimer, SIGNAL(timeout()), this, SLOT(requestBalance()));
    connect(&m_feeRateTimer, SIGNAL(timeout()), this, SLOT(requestEstimatedFeeRate()));

    // connect to myself for save values in UI(main) thread
    connect(this, SIGNAL(gotBalance(const beam::bitcoin::Client::Balance&)), this, SLOT(setBalance(const beam::bitcoin::Client::Balance&)));
    connect(this, SIGNAL(gotEstimatedFeeRate(beam::Amount)), this, SLOT(setEstimatedFeeRate(beam::Amount)));
    connect(this, SIGNAL(gotStatus(beam::bitcoin::Client::Status)), this, SLOT(setStatus(beam::bitcoin::Client::Status)));
    connect(this, SIGNAL(gotCanModifySettings(bool)), this, SLOT(setCanModifySettings(bool)));
    connect(this, SIGNAL(gotConnectionError(beam::bitcoin::IBridge::ErrorType)), this, SLOT(setConnectionError(beam::bitcoin::IBridge::ErrorType)));

    requestBalance();
    requestEstimatedFeeRate();

    m_balanceTimer.start(kUpdateInterval);
    m_feeRateTimer.start(kInterval);

    GetAsync()->GetStatus();
}

beam::Amount SwapCoinClientModel::getAvailable()
{
    return m_balance.m_available;
}

beam::Amount SwapCoinClientModel::getEstimatedFeeRate()
{
    return m_estimatedFeeRate;
}

void SwapCoinClientModel::OnStatus(Status status)
{
    emit gotStatus(status);
}

beam::bitcoin::Client::Status SwapCoinClientModel::getStatus() const
{
    return m_status;
}

bool SwapCoinClientModel::canModifySettings() const
{
    return m_canModifySettings;
}

beam::bitcoin::IBridge::ErrorType SwapCoinClientModel::getConnectionError() const
{
    return m_connectionError;
}

void SwapCoinClientModel::OnBalance(const bitcoin::Client::Balance& balance)
{
    emit gotBalance(balance);
}

void SwapCoinClientModel::OnEstimatedFeeRate(Amount feeRate)
{
    emit gotEstimateFeeRate(feeRate);
}

void SwapCoinClientModel::OnCanModifySettingsChanged(bool canModify)
{
    emit gotCanModifySettings(canModify);
}

void SwapCoinClientModel::OnChangedSettings()
{
    requestBalance();
    requestEstimatedFeeRate();
}

void SwapCoinClientModel::OnConnectionError(beam::bitcoin::IBridge::ErrorType error)
{
    emit gotConnectionError(error);
}

void SwapCoinClientModel::requestBalance()
{
    if (GetSettings().IsActivated())
    {
        // update balance
        GetAsync()->GetBalance();
    }
}

void SwapCoinClientModel::requestEstimatedFeeRate()
{
    if (GetSettings().IsActivated())
    {
        // update estimated fee rate
        GetAsync()->EstimateFeeRate();
    }
}

void SwapCoinClientModel::setBalance(const beam::bitcoin::Client::Balance& balance)
{
    if (m_balance != balance)
    {
        m_balance = balance;
        emit balanceChanged();
    }
}

void SwapCoinClientModel::setEstimatedFeeRate(const beam::Amount estimatedFeeRate)
{
    if (m_estimatedFeeRate != estimatedFeeRate)
    {
        m_estimatedFeeRate = estimatedFeeRate;
        emit estimatedFeeRateChanged();
    }
}

void SwapCoinClientModel::setStatus(beam::bitcoin::Client::Status status)
{
    if (m_status != status)
    {
        m_status = status;
        emit statusChanged();
    }
}

void SwapCoinClientModel::setCanModifySettings(bool canModify)
{
    if (m_canModifySettings != canModify)
    {
        m_canModifySettings = canModify;
        emit canModifySettingsChanged();
    }
}

void SwapCoinClientModel::setConnectionError(beam::bitcoin::IBridge::ErrorType error)
{
    if (m_connectionError != error)
    {
        m_connectionError = error;
        emit connectionErrorChanged();
    }
}
