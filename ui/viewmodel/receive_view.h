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
#include "notifications/exchange_rates_manager.h"
#include "wallet/assets_manager.h"

class ReceiveViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString    amount             READ getAmount             WRITE  setAmount             NOTIFY  amountChanged)
    Q_PROPERTY(QString    comment            READ getComment            WRITE  setComment            NOTIFY  commentChanged)
    Q_PROPERTY(int        assetId            READ getAssetId            WRITE  setAssetId            NOTIFY  assetIdChanged)
    Q_PROPERTY(QString    token              READ getToken              WRITE  setToken              NOTIFY  tokenChanged)
    Q_PROPERTY(QString    sbbsAddress        READ getSbbsAddress                                     NOTIFY  tokenChanged)
    Q_PROPERTY(bool       commentValid       READ getCommentValid                                    NOTIFY  commentValidChanged)
    Q_PROPERTY(bool       isMaxPrivacy       READ getIsMaxPrivacy       WRITE setIsMaxPrivacy        NOTIFY  isMaxPrivacyChanged)
    Q_PROPERTY(int        mpTimeLimit        READ getMPTimeLimit        CONSTANT)

    Q_PROPERTY(QList<QMap<QString, QVariant>> assetsList  READ getAssetsList  NOTIFY  assetsListChanged)
public:
    ReceiveViewModel();
    ~ReceiveViewModel() override = default;

signals:
    void amountChanged();
    void commentChanged();
    void assetIdChanged();
    void tokenChanged();
    void newAddressFailed();
    void commentValidChanged();
    void isMaxPrivacyChanged();
    void assetsListChanged();

public:
    Q_INVOKABLE void saveAddress();

private:
    void setAssetId(int id);
    [[nodiscard]] int getAssetId() const;

    void setAmount(const QString&);
    [[nodiscard]] QString getAmount() const;

    void setComment(const QString& value);
    [[nodiscard]] QString getComment() const;

    void setToken(const QString& value);
    [[nodiscard]] QString getToken() const;
    [[nodiscard]] QString getSbbsAddress() const;
    [[nodiscard]] bool getCommentValid() const;

    void setIsMaxPrivacy(bool value);
    [[nodiscard]] bool getIsMaxPrivacy() const;

    int getMPTimeLimit() const;
    QList<QMap<QString, QVariant>> getAssetsList() const;

private:
    void updateToken();

private:
    beam::Amount    _amount  = 0UL;
    beam::Asset::ID _assetId = beam::Asset::s_BeamID;
    bool            _maxp    = false;

    boost::optional<beam::wallet::WalletAddress> _receiverAddress;
    QString               _originalToken;
    WalletModel&          _walletModel;
    ExchangeRatesManager  _exchangeRatesManager;
    AssetsManager::Ptr    _amgr;
};
