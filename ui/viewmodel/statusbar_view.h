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
#include <QTimer>
#include <QLocale>
#include "model/wallet_model.h"
#include "model/exchange_rates_manager.h"

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "model/swap_eth_client_model.h"
#include "model/swap_coin_client_model.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bridge.h"
#include "wallet/transactions/swaps/bridges/bitcoin/client.h"
#include "wallet/transactions/swaps/bridges/ethereum/bridge.h"
#include "wallet/transactions/swaps/bridges/ethereum/client.h"
#include "wallet/transactions/swaps/common.h"
#endif

class StatusbarViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isOnline                READ getIsOnline            NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isFailedStatus          READ getIsFailedStatus      NOTIFY isFailedStatusChanged)
    Q_PROPERTY(bool isFailedHww             READ getIsFailedHww         NOTIFY isFailedHwwChanged)
    Q_PROPERTY(bool isSyncInProgress        READ getIsSyncInProgress    NOTIFY isSyncInProgressChanged)
    Q_PROPERTY(bool isConnectionTrusted     READ getIsConnectionTrusted NOTIFY isConnectionTrustedChanged)
    Q_PROPERTY(bool isExchangeRatesUpdated  READ getIsExchangeRatesUpdated NOTIFY exchangeRatesUpdateStatusChanged)
    Q_PROPERTY(QString exchangeStatus       READ getExchangeStatus      NOTIFY exchangeRatesUpdateStatusChanged)
    Q_PROPERTY(int nodeSyncProgress         READ getNodeSyncProgress    NOTIFY nodeSyncProgressChanged)
    Q_PROPERTY(QString branchName           READ getBranchName          CONSTANT)
    Q_PROPERTY(QString walletError          READ getWalletError         NOTIFY walletErrorChanged)
    Q_PROPERTY(QString hwwError             READ getHwwError            NOTIFY hwwErrorChanged)

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    Q_PROPERTY(bool isCoinClientFailed      READ getCoinClientFailed    NOTIFY isCoinClientFailedChanged)
    Q_PROPERTY(QString coinClientErrorMsg   READ getCoinClientErrorMsg  NOTIFY coinClientErrorMsgChanged)
    #endif

    #ifdef BEAM_IPFS_SUPPORT
    Q_PROPERTY(QString ipfsStatus READ getIPFSStatus  NOTIFY IPFSStatusChanged)
    Q_PROPERTY(QString ipfsError  READ getIPFSError   NOTIFY IPFSStatusChanged)
    #endif

public:
    StatusbarViewModel();

    [[nodiscard]] bool getIsOnline() const;
    [[nodiscard]] bool getIsFailedStatus() const;
    [[nodiscard]] bool getIsSyncInProgress() const;
    [[nodiscard]] bool getIsConnectionTrusted() const;
    [[nodiscard]] bool getIsExchangeRatesUpdated() const;
    [[nodiscard]] int getNodeSyncProgress() const;
    [[nodiscard]] QString getBranchName() const;
    [[nodiscard]] QString getWalletError() const;
    [[nodiscard]] QString getExchangeStatus() const;
    [[nodiscard]] bool getIsFailedHww() const;
    [[nodiscard]] QString getHwwError() const;


    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    [[nodiscard]] bool getCoinClientFailed() const;
    [[nodiscard]] QString getCoinClientErrorMsg() const;
    [[nodiscard]] Q_INVOKABLE QString coinWithErrorLabel() const;
    #endif

    #ifdef BEAM_IPFS_SUPPORT
    [[nodiscard]] QString getIPFSStatus() const;
    [[nodiscard]] QString getIPFSError() const;
    #endif

    void setIsOnline(bool value);
    void setIsFailedStatus(bool value);
    void setIsSyncInProgress(bool value);
    void setIsConnectionTrusted(bool value);
    void setNodeSyncProgress(int value);
    void setWalletStatusErrorMsg(const QString& value);

public slots:
    void onNodeConnectionChanged(bool isNodeConnected);
    void onDevStateChanged(const QString& sErr, int state);
    void onGetWalletError(beam::wallet::ErrorType error);
    void onSyncProgressUpdated(int done, int total);
    void onNodeSyncProgressUpdated(int done, int total);
    void onExchangeRatesTimer();

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void onCoinClientStatusChanged();
    #endif

    #ifdef BEAM_IPFS_SUPPORT
    void onIPFSSettingsChanged();
    void onIPFSStatus(bool running, const QString& error, unsigned int peercnt);
    #endif

signals:
    void isOnlineChanged();
    void isFailedStatusChanged();
    void isFailedHwwChanged();
    void isSyncInProgressChanged();
    void isConnectionTrustedChanged();
    void nodeSyncProgressChanged();
    void walletErrorChanged();
    void hwwErrorChanged();
    void exchangeRatesUpdateStatusChanged();

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void isCoinClientFailedChanged();
    void coinClientErrorMsgChanged();
    #endif

    #ifdef BEAM_IPFS_SUPPORT
    void IPFSStatusChanged();
    #endif

private:
    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    std::string generateCoinClientErrorMsg();
    void connectCoinClients();
    void connectEthClient();
    #endif

    WalletModel::Ptr m_model;
    WalletSettings& m_settings;
    ExchangeRatesManager::Ptr m_exchangeRatesManager;

    bool m_isOnline;
    bool m_isSyncInProgress;
    bool m_isFailedStatus;
    bool m_isFailedHww;
    bool m_isConnectionTrusted;
    int m_nodeSyncProgress;

    int m_nodeDone;
    int m_nodeTotal;
    int m_done;
    int m_total;

    QString m_walletError;
    QString m_hwwError;

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    bool m_isCoinClientFailed = false;
    QString m_coinWithErrorLabel;
    QString m_coinClientErrorMsg;

    struct SwapClientStatus {
        SwapCoinClientModel::Ptr m_client;
        beam::bitcoin::Client::Status m_status;
        beam::wallet::AtomicSwapCoin m_coin;
    };
    std::vector<SwapClientStatus> m_coinClientStatuses;
    std::pair<SwapEthClientModel::Ptr, beam::ethereum::Client::Status> m_ethCleintStatus;
    #endif

    QTimer m_exchangeRatesTimer;
    QLocale m_locale;

    #ifdef BEAM_IPFS_SUPPORT
    bool m_ipfsRunning = false;
    uint32_t m_ipfsPeerCnt = 0;
    QString m_ipfsError;
    #endif
};
