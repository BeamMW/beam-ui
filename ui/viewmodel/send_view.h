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

class SendViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(unsigned int  feeGrothes         READ getFeeGrothes         WRITE setFeeGrothes       NOTIFY feeGrothesChanged)
    Q_PROPERTY(QString       sendAmount         READ getSendAmount         WRITE setSendAmount       NOTIFY sendAmountChanged)
    Q_PROPERTY(QString       comment            READ getComment            WRITE setComment          NOTIFY commentChanged)

    // TA = Transaction or Address
    Q_PROPERTY(QString  receiverTA         READ getReceiverTA         WRITE setReceiverTA              NOTIFY receiverTAChanged)
    Q_PROPERTY(bool     receiverTAValid    READ getRreceiverTAValid                                    NOTIFY receiverTAChanged)
    Q_PROPERTY(bool     isShieldedTx       READ isShieldedTx          WRITE setIsShieldedTx            NOTIFY isShieldedTxChanged)
    Q_PROPERTY(bool     isNonInteractive   READ isNonInteractive      WRITE setIsNonInteractive        NOTIFY isNonInteractiveChanged)
    Q_PROPERTY(bool     isPermanentAddress READ isPermanentAddress    WRITE setIsPermanentAddress      NOTIFY isPermanentAddressChanged)
    Q_PROPERTY(bool     canChangeTxType    READ canChangeTxType       WRITE setCanChangeTxType         NOTIFY canChangeTxTypeChanged)
    Q_PROPERTY(int      offlinePayments    READ getOfflinePayments    WRITE setOfflinePayments         NOTIFY offlinePaymentsChanged)

    Q_PROPERTY(QString  receiverAddress    READ getReceiverAddress                                     NOTIFY receiverAddressChanged)
    Q_PROPERTY(QString  receiverIdentity   READ getReceiverIdentity                                    NOTIFY receiverIdentityChanged)
    Q_PROPERTY(QString  available          READ getAvailable                                           NOTIFY availableChanged)
    Q_PROPERTY(QString  change             READ getChange                                              NOTIFY availableChanged)
    Q_PROPERTY(QString  fee                READ getFee                                                 NOTIFY feeGrothesChanged)
    Q_PROPERTY(QString  totalUTXO          READ getTotalUTXO                                           NOTIFY availableChanged)
    Q_PROPERTY(QString  missing            READ getMissing                                             NOTIFY availableChanged)
    Q_PROPERTY(bool     isZeroBalance      READ isZeroBalance                                          NOTIFY availableChanged)
    Q_PROPERTY(bool     isEnough           READ isEnough                                               NOTIFY isEnoughChanged)
    Q_PROPERTY(bool     canSend            READ canSend                                                NOTIFY canSendChanged)
    Q_PROPERTY(bool     isToken            READ isToken                                                NOTIFY isTokenChanged)
    Q_PROPERTY(bool     hasAddress         READ hasAddress                                             NOTIFY hasAddressChanged)
    Q_PROPERTY(bool     isOwnAddress       READ isOwnAddress                                           NOTIFY receiverAddressChanged)

    Q_PROPERTY(QString  secondCurrencyUnitName    READ getSecondCurrencyUnitName                  NOTIFY secondCurrencyUnitNameChanged)
    Q_PROPERTY(QString  secondCurrencyRate     READ getSecondCurrencyRate                    NOTIFY secondCurrencyRateChanged)

    Q_PROPERTY(bool     isTokenGeneratebByNewAppVersion      READ isTokenGeneratebByNewAppVersion      NOTIFY tokenGeneratebByNewAppVersion)
    Q_PROPERTY(QString  tokenGeneratebByNewAppVersionMessage READ tokenGeneratebByNewAppVersionMessage NOTIFY tokenGeneratebByNewAppVersion)

    Q_PROPERTY(bool         isNeedExtractShieldedCoins READ isNeedExtractShieldedCoins NOTIFY isNeedExtractShieldedCoinsChanged)
    Q_PROPERTY(unsigned int minimalFeeGrothes          READ getMinimalFeeGrothes       NOTIFY minimalFeeGrothesChanged)

public:
    SendViewModel();

    unsigned int getFeeGrothes() const;
    unsigned int getMinimalFeeGrothes() const;
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
    bool canChangeTxType() const;
    void setCanChangeTxType(bool value);
    int getOfflinePayments() const;
    void setOfflinePayments(int value);

    bool isNonInteractive() const;
    void setIsNonInteractive(bool value);

    QString getAvailable() const;
    QString getMissing() const;
    QString getChange() const;
    QString getFee() const;
    QString getTotalUTXO() const;

    bool isZeroBalance() const;
    bool isEnough() const;
    bool canSend() const;
    bool isToken() const;
    void setIsToken(bool value);
    bool isOwnAddress() const;

    QString getSecondCurrencyUnitName() const;
    QString getSecondCurrencyRate() const;

    bool isNeedExtractShieldedCoins() const;

    bool isTokenGeneratebByNewAppVersion() const;
    QString tokenGeneratebByNewAppVersionMessage() const;
    bool hasAddress() const;
    void setWalletAddress(const boost::optional<beam::wallet::WalletAddress>& value);

public:
    Q_INVOKABLE void setMaxAvailableAmount();
    Q_INVOKABLE void sendMoney();
    Q_INVOKABLE void saveReceiverAddress(const QString& name);

signals:
    void feeGrothesChanged();
    void minimalFeeGrothesChanged();
    void commentChanged();
    void sendAmountChanged();
    void receiverTAChanged();
    void isPermanentAddressChanged();
    void canChangeTxTypeChanged();
    void offlinePaymentsChanged();
    void isNonInteractiveChanged();
    void availableChanged();
    void sendMoneyVerified();
    void cantSendToExpired();
    void canSendChanged();
    void isEnoughChanged();
    void secondCurrencyUnitNameChanged();
    void secondCurrencyRateChanged();
    void receiverAddressChanged();
    void receiverIdentityChanged();
    void tokenGeneratebByNewAppVersion();
    void isTokenChanged();
    void hasAddressChanged();
    void isShieldedTxChanged();
    void isNeedExtractShieldedCoinsChanged();

public slots:
    void onChangeCalculated(beam::Amount change);
    void onShieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes);
    void onNeedExtractShieldedCoins(bool val);
    void onGetAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments);

private:
    void extractParameters();
    void resetMinimalFee();

    beam::Amount _feeGrothes;
    beam::Amount _sendAmountGrothes;
    beam::Amount _changeGrothes;

    QString _comment;
    QString _receiverTA;
    QString _receiverAddress;
    beam::wallet::WalletID _receiverWalletID = beam::Zero;
    beam::wallet::PeerID _receiverIdentity = beam::Zero;
    QString _receiverIdentityStr;
    bool _isPermanentAddress = false;
    bool _canChangeTxType = true;

    bool _isNonInteractive = false;
    bool _isToken = false;
    boost::optional<beam::wallet::WalletAddress> _receiverWalletAddress;
    int _offlinePayments = 0;

    WalletModel& _walletModel;
    ExchangeRatesManager _exchangeRatesManager;
    beam::wallet::TxParameters _txParameters;

    QString _tokenGeneratebByNewAppVersionMessage = "";

    bool _isShieldedTx = false;
    bool _isNeedExtractShieldedCoins = false;
    beam::Amount _minimalFeeGrothes;
    beam::Amount _shieldedInputsFee;
    bool _feeChangedByUi = false;
    bool _maxAvailable = false;
};
