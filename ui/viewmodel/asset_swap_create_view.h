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

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include "model/assets_manager.h"

class AssetSwapCreateViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QMap<QString, QVariant>> currenciesList READ getCurrenciesList NOTIFY currenciesListChanged)
    Q_PROPERTY(QList<QMap<QString, QVariant>> myCurrenciesList READ getMyCurrenciesList NOTIFY myCurrenciesListChanged)
  
    Q_PROPERTY(QString amountToReceive   READ getAmountToReceive   WRITE setAmountToReceive   NOTIFY  amountReceiveChanged)
    Q_PROPERTY(QString amountToSend      READ getAmountToSend      WRITE setAmountToSend      NOTIFY  amountSendChanged)
    Q_PROPERTY(int     receiveAssetIndex READ getReceiveAssetIndex WRITE setReceiveAssetIndex NOTIFY  receiveAssetIndexChanged)
    Q_PROPERTY(int     sendAssetIndex    READ getSendAssetIndex    WRITE setSendAssetIndex    NOTIFY  sendAssetIndexChanged)

    Q_PROPERTY(int           offerExpires             READ getOfferExpires       WRITE  setOfferExpires      NOTIFY  offerExpiresChanged)
    Q_PROPERTY(QString       comment                  READ getComment            WRITE  setComment           NOTIFY  commentChanged)
    // Q_PROPERTY(QString       transactionToken         READ getTransactionToken   WRITE  setTransactionToken  NOTIFY  transactionTokenChanged)

  public:
    AssetSwapCreateViewModel();
    Q_INVOKABLE void publishOffer();

  signals:
    void currenciesListChanged();
    void myCurrenciesListChanged();
    void amountReceiveChanged();
    void amountSendChanged();
    void receiveAssetIndexChanged();
    void sendAssetIndexChanged();

    void offerExpiresChanged();
    void commentChanged();
    // void transactionTokenChanged();

  private slots:
    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);

  private:
    QList<QMap<QString, QVariant>> getCurrenciesList() const;
    QList<QMap<QString, QVariant>> getMyCurrenciesList() const;

    QString getAmountToReceive() const;
    void setAmountToReceive(QString value);

    QString getAmountToSend() const;
    void setAmountToSend(QString value);

    uint getReceiveAssetIndex() const;
    void setReceiveAssetIndex(uint value);

    uint getSendAssetIndex() const;
    void setSendAssetIndex(uint value);

    void setOfferExpires(int value);
    int  getOfferExpires() const;

    void setComment(const QString& value);
    QString getComment() const;

    // void setTransactionToken(const QString& value);
    // QString getTransactionToken() const;


    WalletModel::Ptr _walletModel;
    AssetsManager::Ptr _amgr;
    QList<QMap<QString, QVariant>> _currenciesList;
    QList<QMap<QString, QVariant>> _myCurrenciesList;
    beam::wallet::WalletAddress _receiverAddress;

    beam::Amount _amountToReceiveGrothes = 0;
    beam::Amount _amountSendGrothes = 0;

    beam::Asset::ID  _receiveAsset = 0;
    std::string      _receiveAssetSname;
    uint             _receiveAssetIndex = 0;
    beam::Asset::ID  _sendAsset = 0;
    std::string      _sendAssetSname;
    uint             _sendAssetIndex = 0;
    int       _offerExpires;
    QString   _comment;
    // QString   _token;
};
