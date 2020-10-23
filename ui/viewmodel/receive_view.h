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

class ReceiveViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool     isPermanentAddress           READ isPermanentAddress    WRITE  setIsPermanentAddress  NOTIFY  isPermanentAddressChanged)
    Q_PROPERTY(QString  amountToReceive              READ getAmountToReceive    WRITE  setAmountToReceive     NOTIFY  amountReceiveChanged)
    Q_PROPERTY(int      addressExpires               READ getAddressExpires     WRITE  setAddressExpires      NOTIFY  addressExpiresChanged)
    Q_PROPERTY(QString  addressComment               READ getAddressComment     WRITE  setAddressComment      NOTIFY  addressCommentChanged)
    Q_PROPERTY(QString  receiverAddress              READ getReceiverAddress                                  NOTIFY  receiverAddressChanged)
    Q_PROPERTY(QString  receiverAddressForExchange   READ getReceiverAddressForExchange                       NOTIFY  receiverAddressForExchangeChanged)
    Q_PROPERTY(QString  transactionToken             READ getTransactionToken   WRITE  setTranasctionToken    NOTIFY  transactionTokenChanged)
    Q_PROPERTY(QString  offlineToken                 READ getOfflineToken       WRITE  setOfflineToken        NOTIFY  offlineTokenChanged)
    Q_PROPERTY(bool     commentValid                 READ getCommentValid                                     NOTIFY  commentValidChanged)
    Q_PROPERTY(QString  rateUnit                     READ getRateUnit                                         NOTIFY  rateChanged)
    Q_PROPERTY(QString  rate                         READ getRate                                             NOTIFY  rateChanged)
    Q_PROPERTY(bool     isShieldedTx                 READ isShieldedTx          WRITE setIsShieldedTx         NOTIFY  isShieldedTxChanged)

public:
    ReceiveViewModel();
    ~ReceiveViewModel() override;

signals:
    void amountReceiveChanged();
    void addressExpiresChanged();
    void receiverAddressChanged();
    void receiverAddressForExchangeChanged();
    void addressCommentChanged();
    void transactionTokenChanged();
    void offlineTokenChanged();
    void newAddressFailed();
    void commentValidChanged();
    void isShieldedTxChanged();
    void isPermanentAddressChanged();
    void rateChanged();

public:
    Q_INVOKABLE void initialize(const QString& address);
    Q_INVOKABLE void generateNewReceiverAddress();
    Q_INVOKABLE void saveReceiverAddress();
    Q_INVOKABLE void saveExchangeAddress();

private:
    QString getAmountToReceive() const;
    void    setAmountToReceive(QString value);

    void setAddressExpires(int value);
    int  getAddressExpires() const;

    QString getReceiverAddress() const;
    QString getReceiverAddressForExchange() const;

    void setAddressComment(const QString& value);
    QString getAddressComment() const;

    void setTranasctionToken(const QString& value);
    QString getTransactionToken() const;
    QString getOfflineToken() const;
    void setOfflineToken(const QString& value);

    bool getCommentValid() const;

    void updateTransactionToken();

    QString getRateUnit() const;
    QString getRate() const;

    bool    isShieldedTx() const;
    void    setIsShieldedTx(bool value);

    bool isPermanentAddress() const;
    void setIsPermanentAddress(bool value);

    void onGeneratedReceiverAddress(const beam::wallet::WalletAddress& addr);
    void onGeneratedExchangeAddress(const beam::wallet::WalletAddress& addr);
    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);
    void onGetAddressReturned(const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments);
private:
    beam::Amount _amountToReceiveGrothes;
    int          _addressExpires;
    QString      _addressComment;
    QString      _token;
    QString      _offlineToken;
    beam::wallet::WalletAddress _receiverAddress;
    beam::wallet::WalletAddress _receiverAddressForExchange;
    bool _isShieldedTx = false;
    bool _isPermanentAddress = false;
    WalletModel& _walletModel;
    ExchangeRatesManager _exchangeRatesManager;
    beam::wallet::TxParameters _txParameters;
};
