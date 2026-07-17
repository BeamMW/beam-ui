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
#include "model/exchange_rates_manager.h"
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

    Q_PROPERTY(bool    isErc20Swap        READ isErc20Swap        NOTIFY tokenChanged)
    Q_PROPERTY(QString tokenContract      READ getTokenContract   NOTIFY tokenChanged)
    Q_PROPERTY(QString tokenSymbol        READ getTokenSymbol     NOTIFY tokenChanged)
    Q_PROPERTY(uint    tokenDecimals      READ getTokenDecimals   NOTIFY tokenChanged)
    Q_PROPERTY(bool    isBeamAssetSwap    READ isBeamAssetSwap    NOTIFY tokenChanged)
    Q_PROPERTY(uint    beamAssetId        READ getBeamAssetId     NOTIFY tokenChanged)
    Q_PROPERTY(QString beamAssetUnitName  READ getBeamAssetUnitName NOTIFY tokenChanged)
    Q_PROPERTY(bool    needsBeamForRedeemFee READ needsBeamForRedeemFee NOTIFY tokenChanged)
    // bumped on estimatedFeeRateChanged so QML fee bindings re-evaluate
    Q_PROPERTY(unsigned int feeRatesRevision  READ getFeeRatesRevision NOTIFY feeRatesRevisionChanged)

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

    bool isErc20Swap() const;
    QString getTokenContract() const;
    QString getTokenSymbol() const;
    uint getTokenDecimals() const;
    bool isBeamAssetSwap() const;
    uint getBeamAssetId() const;
    QString getBeamAssetUnitName() const;
    bool needsBeamForRedeemFee() const;
    unsigned int getFeeRatesRevision() const;

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
    void feeRatesRevisionChanged();

public slots:
    void onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId);
    void onCoinsSelected(const beam::wallet::CoinsSelectionInfo&);

private:
    void fillParameters(const beam::wallet::TxParameters& parameters);
    void recalcAvailable();

    // true when @currency is the pair side the offer's custom ERC-20 token rides on
    bool isTokenSide(OldWalletCurrency::OldCurrency currency) const;
    // token side -> min(token decimals, 9) (WalletUnitsPerToken rule), otherwise the classic table
    uint8_t effectiveDecimals(OldWalletCurrency::OldCurrency currency) const;

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

    WalletModel::Ptr _walletModel;
    ExchangeRatesManager::Ptr _rates;
    beam::wallet::TxParameters _txParameters;
    bool _isBeamSide;

    QString _tokenGeneratebByNewAppVersionMessage = "";

    beam::Amount _minimalBeamFeeGrothes;
    bool _feeChangedByUI = false;
    unsigned int _feeRatesRevision = 0;

    // extended-offer fields (params 41-45), empty/0 when the offer is a
    // classic swap
    QString _tokenContract;
    QString _tokenSymbol;
    uint8_t _tokenDecimals = 0;
    beam::Asset::ID _beamAssetId = 0;
    QString _beamAssetUnitName;
};
