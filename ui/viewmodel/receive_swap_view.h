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
#include "viewmodel/notifications/exchange_rates_manager.h"
#include "currencies.h"

class ReceiveSwapViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString       amountToReceive          READ getAmountToReceive    WRITE  setAmountToReceive   NOTIFY  amountReceiveChanged)
    Q_PROPERTY(QString       amountSent               READ getAmountSent         WRITE  setAmountSent        NOTIFY  amountSentChanged)
    Q_PROPERTY(unsigned int  receiveFee               READ getReceiveFee         WRITE  setReceiveFee        NOTIFY  receiveFeeChanged)
    Q_PROPERTY(unsigned int  sentFee                  READ getSentFee            WRITE  setSentFee           NOTIFY  sentFeeChanged)
    Q_PROPERTY(int           offerExpires             READ getOfferExpires       WRITE  setOfferExpires      NOTIFY  offerExpiresChanged)
    Q_PROPERTY(QString       addressComment           READ getAddressComment     WRITE  setAddressComment    NOTIFY  addressCommentChanged)
    Q_PROPERTY(QString       receiverAddress          READ getReceiverAddress                                NOTIFY  receiverAddressChanged)
    Q_PROPERTY(QString       transactionToken         READ getTransactionToken   WRITE  setTransactionToken  NOTIFY  transactionTokenChanged)
    Q_PROPERTY(bool          commentValid             READ getCommentValid                                   NOTIFY  commentValidChanged)
    Q_PROPERTY(bool          isEnough                 READ isEnough                                          NOTIFY  enoughChanged)
    Q_PROPERTY(bool          isEnoughToReceive        READ isEnoughToReceive                                 NOTIFY  enoughToReceiveChanged)
    Q_PROPERTY(bool          isSendFeeOK              READ isSendFeeOK                                       NOTIFY  isSendFeeOKChanged)
    Q_PROPERTY(bool          isReceiveFeeOK           READ isReceiveFeeOK                                    NOTIFY  isReceiveFeeOKChanged)
    Q_PROPERTY(bool          isSendBeam               READ isSendBeam                                        NOTIFY  transactionTokenChanged)
    Q_PROPERTY(QString       rate                     READ getRate                                           NOTIFY  rateChanged)
    Q_PROPERTY(QString       secondCurrencyUnitName         READ getSecondCurrencyUnitName                   NOTIFY secondCurrencyUnitNameChanged)
    Q_PROPERTY(QString       secondCurrencySendRateValue    READ getSecondCurrencySendRateValue              NOTIFY secondCurrencyRateChanged)
    Q_PROPERTY(QString       secondCurrencyReceiveRateValue READ getSecondCurrencyReceiveRateValue           NOTIFY secondCurrencyRateChanged)
    Q_PROPERTY(QString       sentFeeTitle                   READ getSentFeeTitle                             NOTIFY sentCurrencyChanged)
    Q_PROPERTY(QString       receiveFeeTitle                READ getReceiveFeeTitle                          NOTIFY receiveCurrencyChanged)

    Q_PROPERTY(OldWalletCurrency::OldCurrency  receiveCurrency    READ getReceiveCurrency    WRITE  setReceiveCurrency  NOTIFY  receiveCurrencyChanged)
    Q_PROPERTY(OldWalletCurrency::OldCurrency  sentCurrency       READ getSentCurrency       WRITE  setSentCurrency     NOTIFY  sentCurrencyChanged)
    Q_PROPERTY(unsigned int minimalBeamFeeGrothes           READ getMinimalBeamFeeGrothes       NOTIFY  minimalBeamFeeGrothesChanged)
    Q_PROPERTY(QList<QMap<QString, QVariant>> currList      READ getCurrList                    NOTIFY  currListChanged)

public:
    ReceiveSwapViewModel();

signals:
    void amountReceiveChanged();
    void amountSentChanged();
    void receiveFeeChanged();
    void sentFeeChanged();
    void receiveCurrencyChanged();
    void sentCurrencyChanged();
    void offerExpiresChanged();
    void addressCommentChanged();
    void receiverAddressChanged();
    void transactionTokenChanged();
    void newAddressFailed();
    void commentValidChanged();
    void enoughChanged();
    void enoughToReceiveChanged();
    void isSendFeeOKChanged();
    void isReceiveFeeOKChanged();
    void rateChanged();
    void secondCurrencyRateChanged();
    void secondCurrencyUnitNameChanged();
    void minimalBeamFeeGrothesChanged();
    void currListChanged();

public:
    Q_INVOKABLE void generateNewAddress();
    Q_INVOKABLE void saveAddress();
    Q_INVOKABLE void startListen();
    Q_INVOKABLE void publishToken();

private:
    QString getAmountToReceive() const;
    void   setAmountToReceive(QString value);

    QString getAmountSent() const;
    void   setAmountSent(QString value);

    unsigned int getReceiveFee() const;
    void setReceiveFee(unsigned int value);

    unsigned int getSentFee() const;
    void setSentFee(unsigned int value);

    OldWalletCurrency::OldCurrency  getReceiveCurrency() const;
    void setReceiveCurrency(OldWalletCurrency::OldCurrency value);

    OldWalletCurrency::OldCurrency  getSentCurrency() const;
    void setSentCurrency(OldWalletCurrency::OldCurrency value);

    void setOfferExpires(int value);
    int  getOfferExpires() const;

    void setAddressComment(const QString& value);
    QString getAddressComment() const;

    QString getReceiverAddress() const;

    void setTransactionToken(const QString& value);
    QString getTransactionToken() const;

    bool getCommentValid() const;
    bool isEnough() const;
    bool isEnoughToReceive() const;
    bool isSendFeeOK() const;
    bool isReceiveFeeOK() const;

    void updateTransactionToken();
    void loadSwapParams();
    void storeSwapParams();

    bool isSendBeam() const;
    QString getRate() const;

    QString getSecondCurrencyUnitName() const;
    QString getSecondCurrencySendRateValue() const;
    QString getSecondCurrencyReceiveRateValue() const;
    QString getSentFeeTitle() const;
    QString getReceiveFeeTitle() const;

    unsigned int getMinimalBeamFeeGrothes() const;
    QList<QMap<QString, QVariant>> getCurrList() const;

private slots:
    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);
    void onSwapParamsLoaded(const beam::ByteBuffer& token);
    void onCoinsSeleced(const beam::wallet::CoinsSelectionInfo&);

private:
    beam::Amount _amountToReceiveGrothes;
    beam::Amount _amountSentGrothes;
    beam::Amount _receiveFeeGrothes;
    beam::Amount _sentFeeGrothes;

    OldWalletCurrency::OldCurrency  _receiveCurrency;
    OldWalletCurrency::OldCurrency  _sentCurrency;
    int       _offerExpires;
    QString   _addressComment;
    QString   _token;
    bool      _saveParamsAllowed;

    beam::wallet::WalletAddress _receiverAddress;
    WalletModel& _walletModel;
    ExchangeRatesManager _exchangeRatesManager;
    beam::wallet::TxParameters _txParameters;
    bool _isBeamSide;

    beam::Amount _minimalBeamFeeGrothes;
    bool _feeChangedByUI = false;
};
