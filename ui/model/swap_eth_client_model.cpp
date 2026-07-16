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

    // forwards ITokenInfoAsync calls onto the eth reactor thread, mirroring
    // Client's own EthereumClientBridge (client.cpp)
    struct TokenInfoAsyncBridge : public Bridge<ITokenInfoAsync>
    {
        BRIDGE_INIT(TokenInfoAsyncBridge);

        void RequestTokenInfo(std::string contractAddress) override
        {
            call_async(&ITokenInfoAsync::RequestTokenInfo, std::move(contractAddress));
        }
    };
}

SwapEthClientModel::SwapEthClientModel(beam::ethereum::IBridgeHolder::Ptr bridgeHolder,
    std::unique_ptr<beam::ethereum::SettingsProvider> settingsProvider,
    io::Reactor& reactor)
    : ethereum::Client(bridgeHolder, std::move(settingsProvider), reactor)
    , m_balanceTimer(this)
    , m_feeRateTimer(this)
    , m_tokenInfoBridgeHolder(bridgeHolder)
    , m_tokenInfoReactor(reactor)
    , m_tokenInfoAsync(std::make_shared<TokenInfoAsyncBridge>(*static_cast<ITokenInfoAsync*>(this), reactor))
{
    qRegisterMetaType<beam::ethereum::Client::Status>("beam::ethereum::Client::Status");
    qRegisterMetaType<beam::ethereum::IBridge::ErrorType>("beam::ethereum::IBridge::ErrorType");
    qRegisterMetaType<beam::Amount>("beam::Amount");
    qRegisterMetaType<beam::wallet::AtomicSwapCoin>("beam::wallet::AtomicSwapCoin");

    connect(&m_balanceTimer, SIGNAL(timeout()), this, SLOT(requestBalance()));
    connect(&m_feeRateTimer, SIGNAL(timeout()), this, SLOT(requestEstimatedFeeRate()));

    // connect to myself for save values in UI(main) thread
    connect(this, SIGNAL(gotBalance(beam::wallet::AtomicSwapCoin, beam::Amount)), this, SLOT(setBalance(beam::wallet::AtomicSwapCoin, beam::Amount)));
    connect(this, SIGNAL(gotEstimatedGasPrice(beam::Amount)), this, SLOT(setEstimatedGasPrice(beam::Amount)));
    connect(this, SIGNAL(gotStatus(beam::ethereum::Client::Status)), this, SLOT(setStatus(beam::ethereum::Client::Status)));
    connect(this, SIGNAL(gotCanModifySettings(bool)), this, SLOT(setCanModifySettings(bool)));
    connect(this, SIGNAL(gotConnectionError(beam::ethereum::IBridge::ErrorType)), this, SLOT(setConnectionError(beam::ethereum::IBridge::ErrorType)));
    connect(this, &SwapEthClientModel::gotTokenBalance, this, &SwapEthClientModel::setTokenBalance);

    requestBalance();
    requestEstimatedFeeRate();

    m_balanceTimer.start(kBalanceUpdateInterval);
    m_feeRateTimer.start(kFeeRateUpdateInterval);

    GetAsync()->GetStatus();
}

beam::Amount SwapEthClientModel::getAvailable(beam::wallet::AtomicSwapCoin swapCoin) const
{
    auto iter = m_balances.find(swapCoin);
    if (iter != m_balances.end())
    {
        return iter->second;
    }
    return 0;
}

beam::Amount SwapEthClientModel::getGasPrice() const
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

void SwapEthClientModel::validateEndpoint()
{
    GetAsync()->ValidateEndpoint();
}

void SwapEthClientModel::OnBalance(wallet::AtomicSwapCoin swapCoin, Amount balance)
{
    emit gotBalance(swapCoin, balance);
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

void SwapEthClientModel::OnEndpointValidated(uint64_t chainID, uint64_t blockNumber, const beam::ethereum::IBridge::Error& error)
{
    emit endpointValidated(chainID, blockNumber,
                           error.m_type == beam::ethereum::IBridge::None,
                           error.m_type == beam::ethereum::IBridge::InvalidNetwork,
                           QString::fromStdString(error.m_message));
}

void SwapEthClientModel::requestBalance()
{
    if (GetSettings().IsActivated())
    {
        // update balances
        GetAsync()->GetBalance(wallet::AtomicSwapCoin::Ethereum);

        for (auto token : beam::wallet::kEthTokens)
        {
            GetAsync()->GetBalance(token);
        }

        // same cadence, for user-stored custom ERC-20 tokens
        for (const auto& token : AppModel::getInstance().getSettings().getEthCustomTokens())
        {
            const auto contract = token.value("contract").toString().toStdString();
            const auto decimals = static_cast<uint8_t>(token.value("decimals").toUInt());
            if (!contract.empty())
            {
                GetAsync()->GetTokenBalance(contract, decimals);
            }
        }
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

void SwapEthClientModel::setBalance(wallet::AtomicSwapCoin swapCoin, Amount balance)
{
    auto iter = m_balances.find(swapCoin);

    if (m_balances.end() == iter)
    {
        m_balances.emplace(swapCoin, balance);
        emit balanceChanged();
    }
    else if (iter->second != balance)
    {
        iter->second = balance;
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

void SwapEthClientModel::requestTokenInfo(const std::string& contractAddress)
{
    m_tokenInfoAsync->RequestTokenInfo(contractAddress);
}

void SwapEthClientModel::RequestTokenInfo(std::string contractAddress)
{
    // runs on the eth reactor thread
    auto bridge = m_tokenInfoBridgeHolder->Get(m_tokenInfoReactor, *this);
    if (!bridge)
    {
        //% "Cannot connect to node. Please check your network connection."
        emit gotTokenInfo(QString::fromStdString(contractAddress), QString(), 0, qtTrId("swap-connection-error"));
        return;
    }

    auto weak = weak_from_this();
    bridge->getTokenInfo(contractAddress,
        [this, weak, contractAddress](const ethereum::IBridge::Error& error, const std::string& symbol, uint8_t decimals)
    {
        auto sp = weak.lock(); // keeps *this alive across the emit below
        if (!sp)
        {
            return;
        }

        emit gotTokenInfo(QString::fromStdString(contractAddress),
                           QString::fromStdString(symbol),
                           decimals,
                           error.m_type == ethereum::IBridge::None ? QString() : QString::fromStdString(error.m_message));
    });
}

void SwapEthClientModel::setConnectionError(beam::ethereum::IBridge::ErrorType error)
{
    if (m_connectionError != error)
    {
        m_connectionError = error;
        emit connectionErrorChanged();
    }
}

void SwapEthClientModel::OnTokenBalance(const std::string& tokenContract, beam::Amount balance)
{
    emit gotTokenBalance(QString::fromStdString(tokenContract), balance);
}

beam::Amount SwapEthClientModel::getTokenBalance(const QString& contract) const
{
    auto it = m_tokenBalances.find(contract.toLower());
    return it != m_tokenBalances.end() ? it->second : 0;
}

void SwapEthClientModel::setTokenBalance(const QString& contract, beam::Amount balance)
{
    auto key = contract.toLower();
    auto it = m_tokenBalances.find(key);
    if (it == m_tokenBalances.end())
    {
        m_tokenBalances.emplace(key, balance);
        emit tokenBalancesChanged();
    }
    else if (it->second != balance)
    {
        it->second = balance;
        emit tokenBalancesChanged();
    }
}
