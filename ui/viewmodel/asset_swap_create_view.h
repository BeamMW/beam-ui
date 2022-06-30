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
    Q_PROPERTY(QString amountSend        READ getAmountSend        WRITE setAmountSend        NOTIFY  amountSendChanged)
    Q_PROPERTY(int     receiveAssetIndex READ getReceiveAssetIndex WRITE setReceiveAssetIndex NOTIFY  receiveAssetIndexChanged)
    Q_PROPERTY(int     sendAssetIndex    READ getSendAssetIndex    WRITE setSendAssetIndex    NOTIFY  sendAssetIndexChanged)

    // Q_PROPERTY(unsigned int  fee                      READ getFee                WRITE  setFee               NOTIFY  receiveFeeChanged)
    Q_PROPERTY(int           offerExpires             READ getOfferExpires       WRITE  setOfferExpires      NOTIFY  offerExpiresChanged)
    Q_PROPERTY(QString       comment                  READ getComment            WRITE  setComment           NOTIFY  commentChanged)
    // Q_PROPERTY(QString       transactionToken         READ getTransactionToken   WRITE  setTransactionToken  NOTIFY  transactionTokenChanged)

  public:
    AssetSwapCreateViewModel();

  signals:
    void currenciesListChanged();
    void myCurrenciesListChanged();
    void amountReceiveChanged();
    void amountSendChanged();
    void receiveAssetIndexChanged();
    void sendAssetIndexChanged();

    // void receiveFeeChanged();
    void offerExpiresChanged();
    void commentChanged();
    // void transactionTokenChanged();

  private:
    QList<QMap<QString, QVariant>> getCurrenciesList() const;
    QList<QMap<QString, QVariant>> getMyCurrenciesList() const;

    QString getAmountToReceive() const;
    void setAmountToReceive(QString value);

    QString getAmountSend() const;
    void setAmountSend(QString value);

    // unsigned int getFee() const;
    // void setFee(unsigned int value);

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


    AssetsManager::Ptr _amgr;

    beam::Amount _amountToReceiveGrothes = 0;
    beam::Amount _amountSendGrothes = 0;
    // beam::Amount _feeGrothes = 0;

    beam::Asset::ID  _receiveAsset;
    uint             _receiveAssetIndex = 0;
    beam::Asset::ID  _sendAsset;
    uint             _sendAssetIndex = 0;
    int       _offerExpires;
    QString   _comment;
    // QString   _token;
};
