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

#include "statusbar_view.h"
#include "model/app_model.h"
#include "version.h"
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/transactions/swaps/common.h"
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

using namespace beam::wallet;

StatusbarViewModel::StatusbarViewModel()
    : m_model(*AppModel::getInstance().getWalletModel())
    , m_isOnline(false)
    , m_isSyncInProgress(false)
    , m_isFailedStatus(false)
    , m_isConnectionTrusted(false)
    , m_nodeSyncProgress(0)
    , m_nodeDone(0)
    , m_nodeTotal(0)
    , m_done(0)
    , m_total(0)
    , m_errorMsg{}

{
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    recheckCoinClients(true);
    recheckEthClient(true);
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

    connect(&m_model, SIGNAL(nodeConnectionChanged(bool)),
        SLOT(onNodeConnectionChanged(bool)));

    connect(&m_model, SIGNAL(walletError(beam::wallet::ErrorType)),
        SLOT(onGetWalletError(beam::wallet::ErrorType)));

    connect(&m_model, SIGNAL(syncProgressUpdated(int, int)),
        SLOT(onSyncProgressUpdated(int, int)));

    connect(&AppModel::getInstance().getNode(), SIGNAL(syncProgressUpdated(int, int)),
            SLOT(onNodeSyncProgressUpdated(int, int)));
    
    connect(&AppModel::getInstance().getNode(), SIGNAL(failedToSyncNode(beam::wallet::ErrorType)),
            SLOT(onGetWalletError(beam::wallet::ErrorType)));

    m_model.getAsync()->getNetworkStatus();
}

bool StatusbarViewModel::getIsOnline() const
{
    return m_isOnline;
}

bool StatusbarViewModel::getIsFailedStatus() const
{
    return m_isFailedStatus;
}

bool StatusbarViewModel::getIsSyncInProgress() const
{
    return m_isSyncInProgress;
}

bool StatusbarViewModel::getIsConnectionTrusted() const
{
    return m_isConnectionTrusted;
}

int StatusbarViewModel::getNodeSyncProgress() const
{
    return m_nodeSyncProgress;
}

QString StatusbarViewModel::getBranchName() const
{
#ifdef BEAM_MAINNET
    return QString();
#else
    if (BRANCH_NAME.empty())
        return QString();

    return QString::fromStdString(" (" + BRANCH_NAME + ")");
#endif
}

QString StatusbarViewModel::getWalletStatusErrorMsg() const
{
    return m_errorMsg;
}

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
bool StatusbarViewModel::getCoinClientFailed() const
{
    return m_isCoinClientFailed;
}

QString StatusbarViewModel::getCoinClientErrorMsg() const
{
    return m_isCoinClientFailed ? QString::fromStdString(generateCoinClientErrorMsg()) : "";
}

QString StatusbarViewModel::coinWithErrorLabel() const
{
    return m_coinWithErrorLabel;
}
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

void StatusbarViewModel::setIsOnline(bool value)
{
    if (m_isOnline != value)
    {
        m_isOnline = value;
        emit isOnlineChanged();
    }
}

void StatusbarViewModel::setIsFailedStatus(bool value)
{
    if (m_isFailedStatus != value)
    {
        m_isFailedStatus = value;
        emit isFailedStatusChanged();
    }
}

void StatusbarViewModel::setNodeSyncProgress(int value)
{
    if (m_nodeSyncProgress != value)
    {
        m_nodeSyncProgress = value;
        emit nodeSyncProgressChanged();
    }
}

void StatusbarViewModel::setIsSyncInProgress(bool value)
{
    if (m_isSyncInProgress != value)
    {
        m_isSyncInProgress = value;
        emit isSyncInProgressChanged();
    }
}

void StatusbarViewModel::setIsConnectionTrusted(bool value)
{
    if (m_isConnectionTrusted != value)
    {
        m_isConnectionTrusted = value;
        emit isConnectionTrustedChanged();
    }
}

void StatusbarViewModel::setWalletStatusErrorMsg(const QString& value)
{
    if (m_errorMsg != value)
    {
        m_errorMsg = value;
        emit statusErrorChanged();
    }
}

void StatusbarViewModel::onNodeConnectionChanged(bool isNodeConnected)
{
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    setIsConnectionTrusted(m_model.isConnectionTrusted() && !m_isCoinClientFailed);
#else
    setIsConnectionTrusted(m_model.isConnectionTrusted());
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

    if (isNodeConnected == getIsOnline())
    {
        return;
    }

    if (isNodeConnected)
    {
        setIsFailedStatus(false);
        setIsOnline(true);
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
        if (m_isCoinClientFailed)
        {
            emit coinClientErrorMsgChanged();
        }
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
        return;
    }

    setIsOnline(false);

    if (!m_isFailedStatus)
    {
        setWalletStatusErrorMsg(qtTrId("wallet-model-connection-refused-error").arg("BEAM"));
        setIsFailedStatus(true);
    }
}

void StatusbarViewModel::onGetWalletError(beam::wallet::ErrorType error)
{
    setIsOnline(false);
    setWalletStatusErrorMsg(m_model.GetErrorString(error));
    setIsFailedStatus(true);
    setIsConnectionTrusted(false);

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    if (m_isCoinClientFailed)
    {
        emit coinClientErrorMsgChanged();
    }
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
}

void StatusbarViewModel::onSyncProgressUpdated(int done, int total)
{
    m_done = done;
    m_total = total;
    setIsSyncInProgress(!((m_done + m_nodeDone) == (m_total + m_nodeTotal)));
}

void StatusbarViewModel::onNodeSyncProgressUpdated(int done, int total)
{
    m_nodeDone = done;
    m_nodeTotal = total;

    if (total > 0)
    {
        setNodeSyncProgress(static_cast<int>(done * 100) / total);
    }

    setIsSyncInProgress(!((m_done + m_nodeDone) == (m_total + m_nodeTotal)));
}

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
void StatusbarViewModel::onGetCoinClientStatus(beam::bitcoin::Client::Status status)
{
    onGetCoinClientStatusImpl(status);
}

void StatusbarViewModel::onGetCoinClientStatus(beam::ethereum::Client::Status status)
{
    onGetCoinClientStatusImpl(status);
}

std::string StatusbarViewModel::generateCoinClientErrorMsg() const
{
    m_coinWithErrorLabel.clear();
    std::vector<std::pair<AtomicSwapCoin, std::string>> failedClients;
    for (int32_t i = static_cast<int32_t>(AtomicSwapCoin::Bitcoin);
         i < static_cast<int32_t>(AtomicSwapCoin::Unknown);
         ++i)
    {
        AtomicSwapCoin coinT = static_cast<AtomicSwapCoin>(i);
        auto coinClient = AppModel::getInstance().getSwapCoinClient(coinT);
        if (coinClient)
        {
            auto error = coinClient->getConnectionError();
            if (error != beam::bitcoin::IBridge::ErrorType::None)
            {
                const auto& settings = coinClient->GetSettings();
                auto connectionsOptions = settings.GetConnectionOptions();
                failedClients.emplace_back(std::make_pair(coinT, connectionsOptions.m_address.str()));
            }
        }
    }

    bool ethClientFailed = false;
    auto ethClient = AppModel::getInstance().getSwapEthClient();
    if (ethClient)
    {
        ethClientFailed = ethClient->getConnectionError() != beam::ethereum::IBridge::ErrorType::None;
    }

    size_t errorsCount = failedClients.size();
    if (ethClientFailed) ++errorsCount;
    if (m_isFailedStatus) ++errorsCount;


    std::stringstream ss;
    if (errorsCount > 1)
    {
        //% "Lost connection to nodes: "
        ss << qtTrId("status-bar-view-not-connected").toStdString();
        if (m_isFailedStatus) ss << beamui::getCurrencyUnitName(beamui::Currencies::Beam).toStdString() << ", ";
        if (ethClientFailed) ss << std::to_string(AtomicSwapCoin::Ethereum) << ", ";
        for (const auto& p: failedClients)
        {
            ss << std::to_string(p.first) << ", ";
        }
        ss.seekp(-2, std::ios_base::end);
        ss << '\0';
    }
    else if (ethClientFailed)
    {
        ss << qtTrId("wallet-model-connection-refused-error")
              .arg(beamui::getCurrencyUnitName(beamui::Currencies::Ethereum)).toStdString();
    }
    else if (m_isFailedStatus)
    {
        ss << getWalletStatusErrorMsg().toStdString();
    }
    else if(errorsCount){
        ss << qtTrId("wallet-model-connection-refused-error")
              .arg(QString::fromStdString(std::to_string(failedClients[0].first))).toStdString();
        ss << " : " << failedClients[0].second;
        m_coinWithErrorLabel = QString::fromStdString(std::to_string(failedClients[0].first));
    }

    return  ss.str();
}

void StatusbarViewModel::recheckCoinClients(bool reconnect /* = false*/)
{
    for (int32_t i = static_cast<int32_t>(AtomicSwapCoin::Bitcoin);
         i < static_cast<int32_t>(AtomicSwapCoin::Unknown);
         ++i)
    {
        auto coinClient = AppModel::getInstance().getSwapCoinClient(static_cast<AtomicSwapCoin>(i));
        if (coinClient)
        {
            if (reconnect)
            {
                connect(coinClient.get(), SIGNAL(gotStatus(beam::bitcoin::Client::Status)), this,
                        SLOT(onGetCoinClientStatus(beam::bitcoin::Client::Status)));
            }

            const auto& settings = coinClient->GetSettings();
            if (settings.IsActivated())
            {
                coinClient->GetAsync()->GetStatus();
            }
        }
    }
}

void StatusbarViewModel::recheckEthClient(bool reconnect /* = false*/)
{
    auto ethClient = AppModel::getInstance().getSwapEthClient();
    if (ethClient)
    {
        if (reconnect)
        {
            connect(ethClient.get(), SIGNAL(gotStatus(beam::ethereum::Client::Status)),
                    SLOT(onGetCoinClientStatus(beam::ethereum::Client::Status)));
        }
        const auto& settings = ethClient->GetSettings();
        if (settings.IsActivated())
        {
            ethClient->GetAsync()->GetStatus();
        }
    }
}

#endif  // BEAM_ATOMIC_SWAP_SUPPORT
