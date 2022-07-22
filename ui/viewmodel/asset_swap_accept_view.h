// Copyright 2022 The Beam Team
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

#include "model/assets_manager.h"
#include "model/wallet_model.h"
#include <QObject>
#include <QLocale>

class AssetSwapAcceptViewModel: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString amountToReceive   READ getAmountToReceive   NOTIFY  orderChanged)
    Q_PROPERTY(QString amountToSend      READ getAmountToSend      NOTIFY  orderChanged)
    Q_PROPERTY(QString fee               READ getFee               NOTIFY  orderChanged)
    Q_PROPERTY(QString offerCreated      READ getOfferCreated      NOTIFY  orderChanged)
    Q_PROPERTY(QString offerExpires      READ getOfferExpires      NOTIFY  orderChanged)
    Q_PROPERTY(QString comment           READ getComment           WRITE   setComment   NOTIFY  commentChanged)
    Q_PROPERTY(QString rate              READ getRate              NOTIFY  orderChanged)
    Q_PROPERTY(QString orderId           READ getOrderId           WRITE   setOrderId)

    Q_PROPERTY(QList<QMap<QString, QVariant>> sendCurrencies       READ getSendCurrencies     NOTIFY  orderChanged)
    Q_PROPERTY(QList<QMap<QString, QVariant>> receiveCurrencies    READ getReceiveCurrencies  NOTIFY  orderChanged)

  public:
    AssetSwapAcceptViewModel();
    Q_INVOKABLE void startSwap();

  signals:
    void orderChanged();
    void commentChanged();

  private slots:
    void onDexOrdersFinded(const beam::wallet::DexOrder& order);

  private:
    QString getAmountToReceive() const;
    QString getAmountToSend() const;
    QString getFee() const;
    QString getOfferCreated() const;
    QString getOfferExpires() const;

    QString getComment() const;
    void setComment(QString value);

    QString getRate() const;
    QString getOrderId() const;
    void setOrderId(QString value);

    QList<QMap<QString, QVariant>> getSendCurrencies() const;
    QList<QMap<QString, QVariant>> getReceiveCurrencies() const;
    QList<QMap<QString, QVariant>> getCurrenciesList(beam::Asset::ID assetId, const std::string& assetSname) const;

    WalletModel::Ptr _walletModel;
    AssetsManager::Ptr _amgr;
    QLocale _locale;
    beam::wallet::DexOrderID _orderId;
    beam::wallet::WalletID   _sbbsID;

    beam::Amount _amountToReceiveGrothes = 0;
    beam::Amount _amountToSendGrothes = 0;

    beam::Asset::ID  _receiveAsset = 0;
    std::string      _receiveAssetSname;
    beam::Asset::ID  _sendAsset = 0;
    std::string      _sendAssetSname;
    beam::Timestamp  _offerCreated = 0;
    beam::Timestamp  _offerExpires = 0;
    QString   _comment;
    QString   _errorStr;
};
