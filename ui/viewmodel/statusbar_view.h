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
#include "model/wallet_model.h"
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "model/swap_eth_client_model.h"
#include "model/swap_coin_client_model.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bridge.h"
#include "wallet/transactions/swaps/bridges/bitcoin/client.h"
#include "wallet/transactions/swaps/bridges/ethereum/bridge.h"
#include "wallet/transactions/swaps/bridges/ethereum/client.h"
#include "wallet/transactions/swaps/common.h"
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

class StatusbarViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isOnline                READ getIsOnline            NOTIFY isOnlineChanged)
    Q_PROPERTY(bool isFailedStatus          READ getIsFailedStatus      NOTIFY isFailedStatusChanged)
    Q_PROPERTY(bool isSyncInProgress        READ getIsSyncInProgress    NOTIFY isSyncInProgressChanged)
    Q_PROPERTY(bool isConnectionTrusted     READ getIsConnectionTrusted NOTIFY isConnectionTrustedChanged)
    Q_PROPERTY(int nodeSyncProgress         READ getNodeSyncProgress    NOTIFY nodeSyncProgressChanged)
    Q_PROPERTY(QString branchName           READ getBranchName          CONSTANT)
    Q_PROPERTY(QString walletStatusErrorMsg READ getWalletStatusErrorMsg NOTIFY statusErrorChanged)
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    Q_PROPERTY(bool isCoinClientFailed      READ getCoinClientFailed    NOTIFY isCoinClientFailedChanged)
    Q_PROPERTY(QString coinClientErrorMsg   READ getCoinClientErrorMsg  NOTIFY coinClientErrorMsgChanged)
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

public:

    StatusbarViewModel();

    bool getIsOnline() const;
    bool getIsFailedStatus() const;
    bool getIsSyncInProgress() const;
    bool getIsConnectionTrusted() const;
    int getNodeSyncProgress() const;
    QString getBranchName() const;
    QString getWalletStatusErrorMsg() const;
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    bool getCoinClientFailed() const;
    QString getCoinClientErrorMsg() const;
    Q_INVOKABLE QString coinWithErrorLabel() const;
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

    void setIsOnline(bool value);
    void setIsFailedStatus(bool value);
    void setIsSyncInProgress(bool value);
    void setIsConnectionTrusted(bool value);
    void setNodeSyncProgress(int value);
    void setWalletStatusErrorMsg(const QString& value);

public slots:
    void onNodeConnectionChanged(bool isNodeConnected);
    void onGetWalletError(beam::wallet::ErrorType error);
    void onSyncProgressUpdated(int done, int total);
    void onNodeSyncProgressUpdated(int done, int total);
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void onCoinClientStatusChanged();
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

signals:

    void isOnlineChanged();
    void isFailedStatusChanged();
    void isSyncInProgressChanged();
    void isConnectionTrustedChanged();
    void nodeSyncProgressChanged();
    void statusErrorChanged();
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void isCoinClientFailedChanged();
    void coinClientErrorMsgChanged();
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

private:
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    std::string generateCoinClientErrorMsg() const;
    void connectCoinClients();
    void connectEthClient();
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
    WalletModel& m_model;

    bool m_isOnline;
    bool m_isSyncInProgress;
    bool m_isFailedStatus;
    bool m_isConnectionTrusted;
    int m_nodeSyncProgress;

    int m_nodeDone;
    int m_nodeTotal;
    int m_done;
    int m_total;

    QString m_errorMsg;
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    bool m_isCoinClientFailed = false;
    mutable QString m_coinWithErrorLabel;
    // mutable size_t m_errorsCount = 0;

    struct SwapClientStatus {
        SwapCoinClientModel::Ptr m_client;
        beam::bitcoin::Client::Status m_status;
        beam::wallet::AtomicSwapCoin m_coin;
    };
    std::vector<SwapClientStatus> m_coinClientStatuses;
    std::pair<SwapEthClientModel::Ptr, beam::ethereum::Client::Status> m_ethCleintStatus;
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
};
