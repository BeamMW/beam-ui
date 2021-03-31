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
#include <QDateTime>
#include "model/wallet_model.h"
#include "notifications/exchange_rates_manager.h"
#include "currencies.h"

class SendSwapViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString       token            READ getToken            WRITE setToken       NOTIFY tokenChanged)
    Q_PROPERTY(bool          tokenValid       READ getTokenValid                            NOTIFY tokenChanged)
    Q_PROPERTY(bool          parametersValid  READ getParametersValid                       NOTIFY parametersChanged)
    Q_PROPERTY(QString       sendAmount       READ getSendAmount                            NOTIFY sendAmountChanged)
    Q_PROPERTY(unsigned int  sendFee          READ getSendFee          WRITE setSendFee     NOTIFY sendFeeChanged)
    Q_PROPERTY(QString       receiveAmount    READ getReceiveAmount                         NOTIFY receiveAmountChanged)
    Q_PROPERTY(unsigned int  receiveFee       READ getReceiveFee       WRITE setReceiveFee  NOTIFY receiveFeeChanged)
    Q_PROPERTY(QDateTime     offeredTime      READ getOfferedTime                           NOTIFY offeredTimeChanged)
    Q_PROPERTY(QDateTime     expiresTime      READ getExpiresTime                           NOTIFY expiresTimeChanged)
    Q_PROPERTY(bool          isEnough         READ isEnough                                 NOTIFY enoughChanged)
    Q_PROPERTY(bool          isEnoughToReceive READ isEnoughToReceive                       NOTIFY enoughToReceiveChanged)
    Q_PROPERTY(bool          canSend          READ canSend                                  NOTIFY canSendChanged)
    Q_PROPERTY(QString       comment          READ getComment          WRITE setComment     NOTIFY commentChanged)
    Q_PROPERTY(QString       receiverAddress  READ getReceiverAddress                       NOTIFY tokenChanged)
    Q_PROPERTY(bool          isSendFeeOK      READ isSendFeeOK                              NOTIFY isSendFeeOKChanged)
    Q_PROPERTY(bool          isReceiveFeeOK   READ isReceiveFeeOK                           NOTIFY isReceiveFeeOKChanged)
    Q_PROPERTY(bool          isSendBeam       READ isSendBeam                               NOTIFY tokenChanged)
    Q_PROPERTY(QString       rate             READ getRate                                  NOTIFY tokenChanged)

    Q_PROPERTY(QString       secondCurrencyUnitName            READ getSecondCurrencyUnitName             NOTIFY secondCurrencyUnitNameChanged)
    Q_PROPERTY(QString       secondCurrencySendRateValue    READ getSecondCurrencySendRateValue     NOTIFY secondCurrencyRateChanged)
    Q_PROPERTY(QString       secondCurrencyReceiveRateValue READ getSecondCurrencyReceiveRateValue  NOTIFY secondCurrencyRateChanged)

    Q_PROPERTY(OldWalletCurrency::OldCurrency  receiveCurrency  READ getReceiveCurrency  NOTIFY  receiveCurrencyChanged)
    Q_PROPERTY(OldWalletCurrency::OldCurrency  sendCurrency     READ getSendCurrency     NOTIFY  sendCurrencyChanged)
    Q_PROPERTY(QString       sentFeeTitle                 READ getSentFeeTitle     NOTIFY  sendCurrencyChanged)
    Q_PROPERTY(QString       receiveFeeTitle              READ getReceiveFeeTitle  NOTIFY  receiveCurrencyChanged)

    Q_PROPERTY(bool isTokenGeneratedByNewVersion            READ isTokenGeneratedByNewVersion       NOTIFY  tokenGeneratebByNewAppVersion)
    Q_PROPERTY(QString tokenGeneratedByNewVersionMessage    READ tokenGeneratedByNewVersionMessage  NOTIFY  tokenGeneratebByNewAppVersion)
    Q_PROPERTY(unsigned int minimalBeamFeeGrothes           READ getMinimalBeamFeeGrothes       NOTIFY minimalBeamFeeGrothesChanged)
    Q_PROPERTY(QList<QMap<QString, QVariant>> currList      READ getCurrList                    NOTIFY  currListChanged)

public:
    SendSwapViewModel();

    QString getToken() const;
    void setToken(const QString& value);
    bool getTokenValid() const;

    bool getParametersValid() const;

    QString getSendAmount() const;
    void setSendAmount(QString value);

    unsigned int getSendFee() const;
    void setSendFee(unsigned int amount);

    OldWalletCurrency::OldCurrency getSendCurrency() const;
    void setSendCurrency(OldWalletCurrency::OldCurrency value);

    QString getReceiveAmount() const;
    void setReceiveAmount(QString value);

    unsigned int getReceiveFee() const;
    void setReceiveFee(unsigned int amount);

    OldWalletCurrency::OldCurrency getReceiveCurrency() const;
    void setReceiveCurrency(OldWalletCurrency::OldCurrency value);

    void setComment(const QString& value);
    QString getComment() const;

    QDateTime getOfferedTime() const;
    void setOfferedTime(const QDateTime& time);

    QDateTime getExpiresTime() const;
    void setExpiresTime(const QDateTime& time);

    bool isEnough() const;
    bool isEnoughToReceive() const;
    bool canSend() const;
    bool isSendFeeOK() const;
    bool isReceiveFeeOK() const;

    QString getReceiverAddress() const;
    bool isSendBeam() const;
    QString getRate() const;

    QString getSecondCurrencyUnitName() const;
    QString getSecondCurrencySendRateValue() const;
    QString getSecondCurrencyReceiveRateValue() const;

    bool isTokenGeneratedByNewVersion() const;
    QString tokenGeneratedByNewVersionMessage() const;
    unsigned int getMinimalBeamFeeGrothes() const;

    QString getSentFeeTitle() const;
    QString getReceiveFeeTitle() const;
    QList<QMap<QString, QVariant>> getCurrList() const;

public:
    Q_INVOKABLE void setParameters(const QVariant& parameters);    /// used to pass TxParameters directly without Token generation
    Q_INVOKABLE void sendMoney();

signals:
    void tokenChanged();
    void parametersChanged();
    void canSendChanged();
    void sendCurrencyChanged();
    void receiveCurrencyChanged();
    void sendAmountChanged();
    void receiveAmountChanged();
    void sendFeeChanged();
    void receiveFeeChanged();
    void commentChanged();
    void offeredTimeChanged();
    void expiresTimeChanged();
    void enoughChanged();
    void enoughToReceiveChanged();
    void isSendFeeOKChanged();
    void isReceiveFeeOKChanged();
    void secondCurrencyUnitNameChanged();
    void secondCurrencyRateChanged();
    void tokenGeneratebByNewAppVersion();
    void minimalBeamFeeGrothesChanged();
    void currListChanged();

public slots:
    void onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId);
    void onCoinsSelectionCalculated(const beam::wallet::CoinsSelectionInfo&);

private:
    void fillParameters(const beam::wallet::TxParameters& parameters);
    void recalcAvailable();

    beam::Amount _sendAmountGrothes;
    beam::Amount _sendFeeGrothes;
    OldWalletCurrency::OldCurrency _sendCurrency;
    beam::Amount _receiveAmountGrothes;
    beam::Amount _receiveFeeGrothes;
    OldWalletCurrency::OldCurrency _receiveCurrency;
    beam::Amount _changeGrothes;
    QDateTime    _offeredTime;
    QDateTime    _expiresTime;
    QString      _comment;
    QString      _token;

    WalletModel& _walletModel;
    ExchangeRatesManager _exchangeRatesManager;
    beam::wallet::TxParameters _txParameters;
    bool _isBeamSide;

    QString _tokenGeneratebByNewAppVersionMessage = "";

    beam::Amount _minimalBeamFeeGrothes;
    bool _feeChangedByUI = false;
};
