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
#include "model/exchange_rates_manager.h"
#include "model/assets_manager.h"

class SendViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QMap<QString, QVariant>> assetsList READ getAssetsList NOTIFY assetsListChanged)

    Q_PROPERTY(int      assetId           READ getAssetId           WRITE setAssetId             NOTIFY assetIdChanged)
    Q_PROPERTY(QString  assetAvailable    READ getAssetAvailable                                 NOTIFY balanceChanged)
    Q_PROPERTY(QString  assetRemaining    READ getAssetRemaining                                 NOTIFY balanceChanged)
    Q_PROPERTY(QString  beamRemaining     READ getBeamRemaining                                  NOTIFY balanceChanged)
    Q_PROPERTY(QString  changeBeam        READ getChangeBeam                                     NOTIFY balanceChanged)
    Q_PROPERTY(QString  changeAsset       READ getChangeAsset                                    NOTIFY balanceChanged)
    Q_PROPERTY(QString  fee               READ getFee                                            NOTIFY balanceChanged)
    Q_PROPERTY(QString  feeRateUnit       READ getFeeRateUnit                                    NOTIFY feeRateChanged)
    Q_PROPERTY(QString  feeRate           READ getFeeRate                                        NOTIFY feeRateChanged)
    Q_PROPERTY(QString  comment           READ getComment           WRITE setComment             NOTIFY commentChanged)
    Q_PROPERTY(QString  sendAmount        READ getSendAmount        WRITE setSendAmount          NOTIFY balanceChanged)
    Q_PROPERTY(bool     isEnough          READ getIsEnough                                       NOTIFY balanceChanged)
    Q_PROPERTY(bool     isEnoughAmount    READ getIsEnoughAmount                                 NOTIFY balanceChanged)
    Q_PROPERTY(bool     isEnoughFee       READ getIsEnoughFee                                    NOTIFY balanceChanged)
    Q_PROPERTY(bool     canSend           READ canSend                                           NOTIFY canSendChanged)
    Q_PROPERTY(QString  maxSendAmount     READ getMaxSendAmount                                  NOTIFY balanceChanged)
    Q_PROPERTY(bool     tokenValid        READ getTokenValid                                     NOTIFY tokenChanged)
    Q_PROPERTY(QString  token             READ getToken             WRITE setToken               NOTIFY tokenChanged)
    Q_PROPERTY(QString  newTokenMsg       READ getNewTokenMsg                                    NOTIFY tokenChanged)
    Q_PROPERTY(bool     canChoose         READ getCanChoose                                      NOTIFY tokenChanged)
    Q_PROPERTY(bool     choiceOffline     READ getChoiceOffline     WRITE setChoiceOffline       NOTIFY choiceChanged)
    Q_PROPERTY(QString  sendType          READ getSendType                                       NOTIFY choiceChanged)
    Q_PROPERTY(bool     sendTypeOnline    READ getSendTypeOnline                                 NOTIFY choiceChanged)
    Q_PROPERTY(QString  tokenTip          READ getTokenTip                                       NOTIFY tokenTipChanged)
    Q_PROPERTY(QString  tokenTip2         READ getTokenTip2                                      NOTIFY tokenTipChanged)

public:
    SendViewModel();

    [[nodiscard]] QList<QMap<QString, QVariant>> getAssetsList() const;

    [[nodiscard]] int getAssetId() const;
    void setAssetId(int);

    [[nodiscard]] QString getComment() const;
    void setComment(const QString& value);

    [[nodiscard]] QString getSendAmount() const;
    void setSendAmount(const QString& value);

    [[nodiscard]] QString getToken() const;
    void setToken(const QString& value);

    [[nodiscard]] bool getChoiceOffline() const;
    void setChoiceOffline(bool value);

    [[nodiscard]] QString getAssetAvailable() const;
    [[nodiscard]] QString getAssetRemaining() const;
    [[nodiscard]] QString getBeamRemaining() const;
    [[nodiscard]] QString getFee() const;
    [[nodiscard]] QString getFeeRateUnit() const;
    [[nodiscard]] QString getFeeRate() const;
    [[nodiscard]] QString getChangeBeam() const;
    [[nodiscard]] QString getChangeAsset() const;
    [[nodiscard]] QString getMaxSendAmount() const;
    [[nodiscard]] QString getNewTokenMsg() const;
    [[nodiscard]] QString getSendType() const;
    [[nodiscard]] QString getTokenTip() const;
    [[nodiscard]] QString getTokenTip2() const;
    [[nodiscard]] bool getIsEnough() const;
    [[nodiscard]] bool getIsEnoughAmount() const;
    [[nodiscard]] bool getIsEnoughFee() const;
    [[nodiscard]] bool getTokenValid() const;
    [[nodiscard]] bool canSend() const;
    [[nodiscard]] bool getSendTypeOnline() const;
    [[nodiscard]] bool getCanChoose() const;

public:
    Q_INVOKABLE void setMaxPossibleAmount();
    Q_INVOKABLE void sendMoney();
    Q_INVOKABLE void saveReceiverAddress(const QString& name);

signals:
    void assetsListChanged();
    void assetIdChanged();
    void balanceChanged();
    void feeRateChanged();
    void commentChanged();
    void canSendChanged();
    void tokenChanged();
    void tokenTipChanged();
    void choiceChanged();
    void sendMoneyVerified();
    void cantSendToExpired();

public slots:
    void onCoinsSelected(const beam::wallet::CoinsSelectionInfo&);
    void onPublicAddress(const QString&);

private:
    void onGetAddressReturned(const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments);
    void extractParameters();


private:
    [[nodiscard]] beam::Amount getTotalSpend() const;
    void RefreshCsiAsync();

    beam::wallet::CoinsSelectionInfo m_Csi;
    beam::wallet::WalletID     _receiverWalletID;
    beam::PeerID               _receiverIdentity;
    QString                    _comment;
    WalletModel::Ptr           _walletModel;
    ExchangeRatesManager::Ptr  _rates;
    WalletSettings&            _settings;
    AssetsManager::Ptr         _amgr;
    QString                    _token;
    QString                    _newTokenMsg;
    QString                    _publicOfflineAddr;
    bool                       _choiceOffline = false;
    beam::wallet::TxParameters _txParameters;
    bool                       _maxPossible = false;
    size_t                     _vouchersLeft = 0;
};
