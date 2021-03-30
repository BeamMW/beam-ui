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
#pragma once

#include <string>
#include <QObject>
#include <QQmlListProperty>
#include "model/wallet_model.h"
#include "model/swap_coin_client_model.h"
#include "model/swap_eth_client_model.h"
#include "swap_offers_list.h"
#include "swap_tx_object_list.h"
#include "viewmodel/currencies.h"

class SwapCoinClientWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString                      available        READ getAvailableStr     NOTIFY availableChanged)
    Q_PROPERTY(bool                         isConnected      READ getIsConnected      NOTIFY statusChanged)
    Q_PROPERTY(bool                         isConnecting     READ getIsConnecting     NOTIFY statusChanged)
    Q_PROPERTY(bool                         hasActiveTx      READ hasActiveTx         NOTIFY activeTxChanged)
    Q_PROPERTY(QString                      coinLabel        READ getCoinLabel        CONSTANT)
    Q_PROPERTY(OldCurrency                  currency         READ getCurrency         CONSTANT)

public:
    SwapCoinClientWrapper() = default;
    SwapCoinClientWrapper(beam::wallet::AtomicSwapCoin swapCoin);

    void incrementActiveTxCounter();
    void decrementActiveTxCounter();
    int getActiveTxCounter() const;
    void resetActiveTxCounter();

    QString getAvailableStr() const;
    bool getIsConnected() const;
    bool getIsConnecting() const;
    bool hasActiveTx() const;
    QString getCoinLabel() const;
    OldCurrency getCurrency() const;

    beam::wallet::AtomicSwapCoin getSwapCoin() const;
    uint16_t getLockTxMinConfirmations() const;
    uint16_t getWithdrawTxMinConfirmations() const;
    double getBlocksPerHour() const;
    beam::Amount getAvailable() const;

signals:
    void activeTxChanged();
    void availableChanged();
    void statusChanged();

private:
    beam::wallet::AtomicSwapCoin m_swapCoin;
    std::weak_ptr<SwapCoinClientModel> m_coinClient;
    std::weak_ptr<SwapEthClientModel> m_ethClient;
    int m_activeTxCounter = 0;
    uint16_t m_lockTxMinConfirmations = 0;
    uint16_t m_withdrawTxMinConfirmations = 0;
    double m_blocksPerHour = 0;
};

class SwapOffersViewModel : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QAbstractItemModel*                       transactions        READ getTransactions        NOTIFY allTransactionsChanged)
    Q_PROPERTY(QAbstractItemModel*                       allOffers           READ getAllOffers           NOTIFY allOffersChanged)
    Q_PROPERTY(QAbstractItemModel*                       allOffersFitBalance READ getAllOffersFitBalance NOTIFY allOffersFitBalanceChanged)
    Q_PROPERTY(QString                                   beamAvailable       READ beamAvailable          NOTIFY beamAvailableChanged)
    Q_PROPERTY(bool                                      showBetaWarning     READ showBetaWarning)
    Q_PROPERTY(bool                                      isOffersLoaded      READ isOffersLoaded         NOTIFY offersLoaded)
    Q_PROPERTY(int                                       activeTxCount       READ getActiveTxCount       NOTIFY allTransactionsChanged)
    Q_PROPERTY(QQmlListProperty<SwapCoinClientWrapper>   swapClientList      READ getSwapClients         CONSTANT)

public:
    SwapOffersViewModel();
    virtual ~SwapOffersViewModel();

    QAbstractItemModel* getTransactions();
    QAbstractItemModel* getAllOffers();
    QAbstractItemModel* getAllOffersFitBalance();
    QString beamAvailable() const;
    bool showBetaWarning() const;
    bool isOffersLoaded() const;
    int getActiveTxCount() const;
    QQmlListProperty<SwapCoinClientWrapper> getSwapClients();

    Q_INVOKABLE void cancelOffer(const QVariant& variantTxID);
    Q_INVOKABLE void cancelTx(const QVariant& variantTxID);
    Q_INVOKABLE void deleteTx(const QVariant& variantTxID);
    Q_INVOKABLE PaymentInfoItem* getPaymentInfo(const QVariant& variantTxID);

public slots:
    void onTransactionsDataModelChanged(
        beam::wallet::ChangeAction action,
        const std::vector<beam::wallet::TxDescription>& transactions);
    void onSwapOffersDataModelChanged(
        beam::wallet::ChangeAction action,
        const std::vector<beam::wallet::SwapOffer>& offers);
    void resetAllOffersFitBalance();

signals:
    void allTransactionsChanged();
    void allOffersChanged();
    void allOffersFitBalanceChanged();
    void beamAvailableChanged();
    void offerRemovedFromTable(QVariant variantTxID);
    void offersLoaded();

private:
    void monitorAllOffersFitBalance();
    bool isOfferFitBalance(const SwapOfferItem& offer);
    void insertAllOffersFitBalance(
        const std::vector<std::shared_ptr<SwapOfferItem>>& offers);
    void removeAllOffersFitBalance(
        const std::vector<std::shared_ptr<SwapOfferItem>>& offers);
    bool hasActiveTx(const std::string& swapCoin) const;
    void InitSwapClientWrappers();

    SwapCoinClientWrapper* getSwapCoinClientWrapper(beam::wallet::AtomicSwapCoin swapCoinType) const;
    uint32_t getLockTxMinConfirmations(beam::wallet::AtomicSwapCoin swapCoinType) const;
    uint32_t getWithdrawTxMinConfirmations(beam::wallet::AtomicSwapCoin swapCoinType) const;
    double getBlocksPerHour(beam::wallet::AtomicSwapCoin swapCoinType) const;
    void incrementActiveTxCounter(beam::wallet::AtomicSwapCoin swapCoinType);
    void decrementActiveTxCounter(beam::wallet::AtomicSwapCoin swapCoinType);
    void resetActiveTxCounters();
    void setIsOffersLoaded(bool isOffersLoaded);

    WalletModel& m_walletModel;

    SwapTxObjectList m_transactionsList;
    SwapOffersList m_offersList;
    SwapOffersList m_offersListFitBalance;
    QList<SwapCoinClientWrapper*> m_swapClientWrappers;

    int m_activeTxCount = 0;
    std::map<beam::wallet::TxID, beam::wallet::AtomicSwapCoin> m_activeTx;
    bool m_isOffersLoaded = false;
};
