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

    Q_PROPERTY(int     offerExpires      READ getOfferExpires      WRITE  setOfferExpires     NOTIFY  offerExpiresChanged)
    Q_PROPERTY(QString comment           READ getComment           WRITE  setComment          NOTIFY  commentChanged)
    Q_PROPERTY(QString rate              READ getRate                                         NOTIFY  rateChanged)
    Q_PROPERTY(bool    canCreate         READ getCanCreate                                    NOTIFY  canCreateChanged)
    Q_PROPERTY(bool    isEnough          READ getIsEnough                                     NOTIFY  canCreateChanged)
    Q_PROPERTY(QString maxSendAmount     READ getMaxSendAmount                                NOTIFY  canCreateChanged)
    Q_PROPERTY(bool    isAssetsSame      READ getIsAssetsSame                                 NOTIFY  canCreateChanged)
    Q_PROPERTY(bool    commentValid      READ getCommentValid                                 NOTIFY  commentValidChanged)
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
    void rateChanged();
    void canCreateChanged();
    void commentValidChanged();
    // void transactionTokenChanged();

  private slots:
    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);
    void onCoinsSelected(const beam::wallet::CoinsSelectionInfo&);
    void onAssetsSwapParamsLoaded(const beam::ByteBuffer& params);

  private:
    QList<QMap<QString, QVariant>> getCurrenciesList() const;
    QList<QMap<QString, QVariant>> getMyCurrenciesList() const;

    QString getAmountToReceive() const;
    void setAmountToReceive(QString value);

    QString getAmountToSend() const;
    void setAmountToSend(QString value);

    uint getReceiveAssetIndex() const;
    void setReceiveAssetIndexImpl(int value);
    void setReceiveAssetIndex(int value);

    uint getSendAssetIndex() const;
    void setSendAssetIndex(int value);

    void setOfferExpires(int value);
    int  getOfferExpires() const;

    void setComment(const QString& value);
    QString getComment() const;

    QString getRate() const;

    bool getCanCreate() const;
    bool getIsEnough() const;
    QString getMaxSendAmount() const;
    bool getIsAssetsSame() const;
    bool getCommentValid() const;

    void saveLastOfferState();

    // void setTransactionToken(const QString& value);
    // QString getTransactionToken() const;


    WalletModel::Ptr _walletModel;
    AssetsManager::Ptr _amgr;
    QList<QMap<QString, QVariant>> _currenciesList;
    QList<QMap<QString, QVariant>> _myCurrenciesList;
    beam::wallet::WalletAddress _receiverAddress;

    beam::Amount _amountToReceiveGrothes = 0;
    beam::Amount _amountToSendGrothes = 0;

    beam::Asset::ID  _receiveAsset = 0;
    std::string      _receiveAssetSname;
    int              _receiveAssetIndex = 0;
    beam::Asset::ID  _sendAsset = 0;
    std::string      _sendAssetSname;
    int              _sendAssetIndex = 0;
    uint32_t         _offerExpires = 0;
    int              _offerExpiresIndex = 0;
    QString          _comment;
    bool             _isEnoughtToSend = false;
    beam::Amount     _maxAmountToSendGrothes = 0;
    // QString   _token;
};
