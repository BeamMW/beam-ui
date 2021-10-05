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
#include "settings_helpers.h"
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

using namespace beam::wallet;

StatusbarViewModel::StatusbarViewModel()
    : m_model(*AppModel::getInstance().getWalletModel())
    , m_isOnline(false)
    , m_isSyncInProgress(!m_model.isSynced())
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
    connectCoinClients();
    connectEthClient();
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

    connect(&m_exchangeRatesTimer, SIGNAL(timeout()), SLOT(onExchangeRatesTimer()));
    connect(&m_exchangeRatesManager, SIGNAL(updateTimeChanged()), SLOT(onExchangeRatesTimer()));


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

bool StatusbarViewModel::getIsExchangeRatesUpdated() const
{
    return m_exchangeRatesManager.isUpToDate();
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

QString StatusbarViewModel::getExchangeStatus() const
{
    //% " (exchange rate to %1 was not updated since %2)"
    return qtTrId("status-online-stale-rates").arg(beamui::getCurrencyUnitName(m_exchangeRatesManager.getRateCurrency()))
                                              .arg(m_exchangeRatesManager.getUpdateTime().toString(m_locale.dateTimeFormat(QLocale::ShortFormat)));
}

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
bool StatusbarViewModel::getCoinClientFailed() const
{
    return m_isCoinClientFailed;
}

QString StatusbarViewModel::getCoinClientErrorMsg() const
{
    return getCoinClientFailed() ? m_coinClientErrorMsg : "";
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
        emit exchangeRatesUpdateStatusChanged();
        if (value)
        {
            m_exchangeRatesTimer.start(60 * 1000);
        }
        else
        {
            m_exchangeRatesTimer.stop();
        }
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
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
        m_coinWithErrorLabel = beamui::getCurrencyUnitName(beamui::Currencies::Beam);
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
        emit statusErrorChanged();
    }
}

void StatusbarViewModel::onNodeConnectionChanged(bool isNodeConnected)
{
    setIsConnectionTrusted(m_model.isConnectionTrusted());

    if (isNodeConnected == getIsOnline())
    {
        return;
    }

    if (isNodeConnected)
    {
        setIsFailedStatus(false);
        setIsOnline(true);
        setIsSyncInProgress(!m_model.isSynced());
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
        if (m_isCoinClientFailed)
        {
            m_coinClientErrorMsg = QString::fromStdString(generateCoinClientErrorMsg());
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
        m_coinClientErrorMsg = QString::fromStdString(generateCoinClientErrorMsg());
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
void StatusbarViewModel::onCoinClientStatusChanged()
{
    size_t failedClientsCount = 0;
    size_t changedStatusesCount = 0;

    for(auto& it : m_coinClientStatuses)
    {
        auto status = it.m_client->getStatus();
        if(status != it.m_status)
        {
            it.m_status = status;
            ++changedStatusesCount;
        }
        if (status == beam::bitcoin::Client::Status::Failed) ++failedClientsCount;
    }

    {
        auto status = m_ethCleintStatus.first->getStatus();
        if(status != m_ethCleintStatus.second)
        {
            m_ethCleintStatus.second = status;
            ++changedStatusesCount;
        }
        if (status == beam::ethereum::Client::Status::Failed) ++failedClientsCount;
    }

    if (!m_isCoinClientFailed && failedClientsCount)
    {
        m_isCoinClientFailed = true;
        emit isCoinClientFailedChanged();
    } else if (m_isCoinClientFailed && failedClientsCount == 0)
    {
        m_isCoinClientFailed = false;
        emit isCoinClientFailedChanged();
    }

    if (changedStatusesCount)
    {
        m_coinClientErrorMsg = QString::fromStdString(generateCoinClientErrorMsg());
        emit coinClientErrorMsgChanged();
    }
}

std::string StatusbarViewModel::generateCoinClientErrorMsg()
{
    m_coinWithErrorLabel.clear();

    std::vector<std::pair<AtomicSwapCoin, std::string>> failedClients;
    for (const auto& cs : m_coinClientStatuses)
    {
        if (cs.m_status == beam::bitcoin::Client::Status::Failed)
        {
            const auto& settings = cs.m_client->GetSettings();
            std::string addr;
            if (settings.IsElectrumActivated())
            {
                auto connectionsOptions = settings.GetElectrumConnectionOptions();
                addr = connectionsOptions.m_address;
            }
            else {
                auto connectionsOptions = settings.GetConnectionOptions();
                addr = connectionsOptions.m_address.str();
            }
            failedClients.emplace_back(std::make_pair(cs.m_coin, addr));
        }
    }

    bool ethClientFailed = m_ethCleintStatus.second == beam::ethereum::Client::Status::Failed;

    size_t errorsCount = failedClients.size();
    if (ethClientFailed) ++errorsCount;
    if (m_isFailedStatus) ++errorsCount;


    std::stringstream ss;
    if (errorsCount > 1)
    {
        std::string failedNodes;
        if (m_isFailedStatus) failedNodes += (beamui::getCurrencyUnitName(beamui::Currencies::Beam).toStdString() + ", ");
        for (const auto& p: failedClients)
        {
            failedNodes += (std::to_string(p.first) + ", ");
        }
        if (ethClientFailed) failedNodes += (std::to_string(AtomicSwapCoin::Ethereum) + ", ");
        failedNodes.erase(failedNodes.end() - 2, failedNodes.end());

        std::size_t found = failedNodes.find(',');
        if (found != std::string::npos)
        {
            m_coinWithErrorLabel = QString::fromStdString(failedNodes.substr(0, found));
        }

        //% "Connection to %1 nodes lost"
        ss << qtTrId("status-bar-view-not-connected").arg(QString::fromStdString(failedNodes)).toStdString();
    }
    else if (ethClientFailed)
    {
        m_coinWithErrorLabel = beamui::getCurrencyUnitName(beamui::Currencies::Ethereum);
        ss << qtTrId("wallet-model-connection-refused-error").arg("Ethereum").toStdString();
    }
    else if(errorsCount == 1 && m_isCoinClientFailed){
        ss << qtTrId("wallet-model-connection-refused-error")
              .arg(getCoinTitle(failedClients[0].first)).toStdString();
        ss << " : " << failedClients[0].second;
        m_coinWithErrorLabel = QString::fromStdString(std::to_string(failedClients[0].first));
    }

    return  ss.str();
}

void StatusbarViewModel::connectCoinClients()
{
    for (int32_t i = static_cast<int32_t>(AtomicSwapCoin::Bitcoin);
         i < static_cast<int32_t>(AtomicSwapCoin::Unknown);
         ++i)
    {
        AtomicSwapCoin coin = static_cast<AtomicSwapCoin>(i);
        auto coinClient = AppModel::getInstance().getSwapCoinClient(coin);
        if (coinClient)
        {
            connect(coinClient.get(), SIGNAL(statusChanged()), this, SLOT(onCoinClientStatusChanged()));
            auto status = coinClient->getStatus();
            if (status == beam::bitcoin::Client::Status::Failed) m_isCoinClientFailed = true;
            m_coinClientStatuses.push_back({coinClient, status, coin});

            const auto& settings = coinClient->GetSettings();
            if (settings.IsActivated())
            {
                coinClient->GetAsync()->GetStatus();
            }
        }
    }
}

void StatusbarViewModel::connectEthClient()
{
    auto ethClient = AppModel::getInstance().getSwapEthClient();
    if (ethClient)
    {
        connect(ethClient.get(), SIGNAL(statusChanged()), this, SLOT(onCoinClientStatusChanged()));
        auto status = ethClient->getStatus();
        if (status == beam::ethereum::Client::Status::Failed) m_isCoinClientFailed = true;
        m_ethCleintStatus = std::make_pair(ethClient, status);
        const auto& settings = ethClient->GetSettings();
        if (settings.IsActivated())
        {
            ethClient->GetAsync()->GetStatus();
        }
    }
}

#endif  // BEAM_ATOMIC_SWAP_SUPPORT

void  StatusbarViewModel::onExchangeRatesTimer()
{
    emit exchangeRatesUpdateStatusChanged();
}
