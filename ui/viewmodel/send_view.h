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

class SendViewModel: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int      selectedAsset   READ getSelectedAsset   WRITE setSelectedAsset   NOTIFY assetChanged)
    Q_PROPERTY(QString  sendUnit        READ getSendUnitName                             NOTIFY assetChanged)
    Q_PROPERTY(QString  assetAvailable  READ getAssetAvailable                           NOTIFY availableChanged)
    Q_PROPERTY(QString  beamAvailable   READ getBeamAvailable                            NOTIFY availableChanged)
    Q_PROPERTY(QString  assetMissing    READ getAssetMissing                             NOTIFY availableChanged)
    Q_PROPERTY(QString  changeBeam      READ getChangeBeam                               NOTIFY availableChanged)
    Q_PROPERTY(QString  changeAsset     READ getChangeAsset                              NOTIFY availableChanged)

    Q_PROPERTY(unsigned int  feeGrothes   READ getFeeGrothes   WRITE setFeeGrothes  NOTIFY feeGrothesChanged)
    Q_PROPERTY(QString       sendAmount   READ getSendAmount   WRITE setSendAmount  NOTIFY sendAmountChanged)
    Q_PROPERTY(QString       comment      READ getComment      WRITE setComment     NOTIFY commentChanged)

    // TA = Transaction or Address
    Q_PROPERTY(QString  receiverTA         READ getReceiverTA         WRITE setReceiverTA              NOTIFY receiverTAChanged)
    Q_PROPERTY(bool     receiverTAValid    READ getRreceiverTAValid                                    NOTIFY receiverTAChanged)
    Q_PROPERTY(bool     isShieldedTx       READ isShieldedTx          WRITE setIsShieldedTx            NOTIFY isShieldedTxChanged)
    Q_PROPERTY(bool     isOffline          READ isOffline             WRITE setIsOffline               NOTIFY isOfflineChanged)
    Q_PROPERTY(bool     isMaxPrivacy       READ isMaxPrivacy          WRITE setIsMaxPrivacy            NOTIFY isMaxPrivacyChanged)
    Q_PROPERTY(bool     isPermanentAddress READ isPermanentAddress    WRITE setIsPermanentAddress      NOTIFY isPermanentAddressChanged)
    Q_PROPERTY(int      offlinePayments    READ getOfflinePayments    WRITE setOfflinePayments         NOTIFY offlinePaymentsChanged)

    Q_PROPERTY(QString  receiverAddress    READ getReceiverAddress                                     NOTIFY receiverAddressChanged)
    Q_PROPERTY(QString  receiverIdentity   READ getReceiverIdentity                                    NOTIFY receiverIdentityChanged)
    Q_PROPERTY(QString  fee                READ getFee                                                 NOTIFY feeGrothesChanged)
    Q_PROPERTY(bool     isZeroBalance      READ isZeroBalance                                          NOTIFY availableChanged)
    Q_PROPERTY(bool     isEnough           READ isEnough                                               NOTIFY isEnoughChanged)
    Q_PROPERTY(bool     canSend            READ canSend                                                NOTIFY canSendChanged)
    Q_PROPERTY(bool     isToken            READ isToken                                                NOTIFY tokenChanged)
    Q_PROPERTY(bool     hasAddress         READ hasAddress                                             NOTIFY hasAddressChanged)
    Q_PROPERTY(bool     isOwnAddress       READ isOwnAddress                                           NOTIFY receiverAddressChanged)

    Q_PROPERTY(QString  rateUnit     READ getRateUnit     NOTIFY rateChanged)
    Q_PROPERTY(QString  rate         READ getRate         NOTIFY rateChanged)
    Q_PROPERTY(QString  feeRateUnit  READ getFeeRateUnit  NOTIFY feeRateChanged)
    Q_PROPERTY(QString  feeRate      READ getFeeRate      NOTIFY feeRateChanged)

    Q_PROPERTY(bool     isNewToken   READ getIsNewToken   NOTIFY tokenChanged)
    Q_PROPERTY(QString  newTokenMsg  READ getNewTokenMsg  NOTIFY tokenChanged)

    Q_PROPERTY(bool         isNeedExtractShieldedCoins READ isNeedExtractShieldedCoins NOTIFY isNeedExtractShieldedCoinsChanged)
    Q_PROPERTY(unsigned int minFee                     READ getMinFee                  NOTIFY minFeeChanged)

public:
    SendViewModel();

    int getSelectedAsset() const;
    void setSelectedAsset(int);
    QString getSendUnitName();

    unsigned int getFeeGrothes() const;
    unsigned int getMinFee() const;
    void setFeeGrothes(unsigned int amount);

    void setComment(const QString& value);
    QString getComment() const;

    QString getSendAmount() const;
    void setSendAmount(QString value);

    QString getReceiverTA() const;
    void    setReceiverTA(const QString& value);
    bool    getRreceiverTAValid() const;
    QString getReceiverAddress() const;
    QString getReceiverIdentity() const;
    bool    isShieldedTx() const;
    void    setIsShieldedTx(bool value);
    bool isPermanentAddress() const;
    void setIsPermanentAddress(bool value);
    int getOfflinePayments() const;
    void setOfflinePayments(int value);

    bool isOffline() const;
    void setIsOffline(bool value);

    QString getAssetAvailable() const;
    QString getBeamAvailable() const;
    QString getAssetMissing() const;
    QString getBeamMissing() const;
    QString getChangeBeam() const;
    QString getChangeAsset() const;
    bool isMaxPrivacy() const;
    void setIsMaxPrivacy(bool value);
    QString getFee() const;

    bool isZeroBalance() const;
    bool isEnough() const;
    bool canSend() const;
    bool isToken() const;
    void setIsToken(bool value);
    bool isOwnAddress() const;

    QString getRateUnit() const;
    QString getRate() const;
    QString getFeeRateUnit() const;
    QString getFeeRate() const;

    bool isNeedExtractShieldedCoins() const;

    bool getIsNewToken() const;
    QString getNewTokenMsg() const;

    bool hasAddress() const;
    void setWalletAddress(const boost::optional<beam::wallet::WalletAddress>& value);

public:
    Q_INVOKABLE void setMaxAvailableAmount();
    Q_INVOKABLE void sendMoney();
    Q_INVOKABLE void saveReceiverAddress(const QString& name);

signals:
    void assetChanged();
    void feeGrothesChanged();
    void minFeeChanged();
    void commentChanged();
    void sendAmountChanged();
    void receiverTAChanged();
    void isPermanentAddressChanged();
    void offlinePaymentsChanged();
    void isOfflineChanged();
    void isMaxPrivacyChanged();
    void availableChanged();
    void sendMoneyVerified();
    void cantSendToExpired();
    void canSendChanged();
    void isEnoughChanged();
    void rateChanged();
    void feeRateChanged();
    void receiverAddressChanged();
    void receiverIdentityChanged();
    void tokenChanged();
    void hasAddressChanged();
    void isShieldedTxChanged();
    void isNeedExtractShieldedCoinsChanged();

public slots:
    void onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId);
    void onShieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes);
    void onNeedExtractShieldedCoins(bool val);
    void onGetAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments);
    void onAssetInfo(beam::Asset::ID assetId);

private:
    void extractParameters();
    void resetMinimalFee();
    void resetAddress();

    beam::Amount _fee;
    beam::Amount _shieldedFee = 0;
    beam::Amount _sendAmount  = 0;
    beam::Amount _changeBeam  = 0;
    beam::Amount _changeAsset = 0; // for non-beam transactions only
    beam::Asset::ID _selectedAssetId = beam::Asset::s_BeamID;

    QString _comment;
    QString _receiverTA;
    QString _receiverAddress;

    beam::wallet::WalletID _receiverWalletID = beam::Zero;
    beam::wallet::PeerID   _receiverIdentity = beam::Zero;

    bool _isPermanentAddress = false;

    bool _isOffline = false;
    bool _isMaxPrivacy = false;
    bool _isToken = false;
    boost::optional<beam::wallet::WalletAddress> _receiverWalletAddress;
    int _offlinePayments = 0;

    WalletModel&               _walletModel;
    ExchangeRatesManager       _exchangeRatesManager;
    beam::wallet::TxParameters _txParameters;
    QString                    _newTokenMsg;
    AssetsManager              _amgr;

    bool _isShielded = false;
    bool _isNeedExtractShieldedCoins = false;
    beam::Amount _minFee;
    bool _feeChangedByUi = false;
    bool _maxAvailable   = false;
};
