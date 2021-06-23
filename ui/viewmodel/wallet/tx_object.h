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
#include "viewmodel/payment_item.h"
#include "viewmodel/ui_helpers.h"
#include "wallet/client/extensions/news_channels/interface.h"
#include "bvm/ManagerStd.h"
#include "assets_list.h"

class TxObject: public QObject
{
    Q_OBJECT
public:
    explicit TxObject(beam::wallet::TxDescription tx, QObject* parent = nullptr);
    TxObject(beam::wallet::TxDescription tx, beam::wallet::Currency secondCurrency, QObject* parent = nullptr);
    bool operator==(const TxObject& other) const;

    beam::Timestamp timeCreated() const;
    beam::wallet::TxID getTxID() const;
    QString getAmountGeneral() const;
    QString getRate() const;
    QString getRate(beam::Asset::ID) const;
    const QString& getComment() const;
    QString getAddressFrom() const;
    QString getAddressTo() const;
    virtual QString getFee() const;
    QString getKernelID() const;
    QString getTransactionID() const ;
    bool hasPaymentProof() const;
    virtual QString getStatus() const;
    virtual QString getFailureReason() const;
    virtual QString getStateDetails() const;
    QString getToken() const;
    QString getSenderIdentity() const;
    QString getReceiverIdentity() const;
    QString getFeeRate() const;
    QString getAmountSecondCurrency();
    QString getCidsStr() const;
    QString getSource() const;
    uint32_t getMinConfirmations() const;
    QString getConfirmationProgress() const;
    QString getAppId();
    bool isDappTx();

    const std::vector<beam::Asset::ID>& getAssetsList() const;
    const std::vector<QString>& getAssetAmounts() const;
    const std::vector<bool>& getAssetAmountsIncome() const;
    const std::vector<QString>& getAssetRates() const;
    const std::vector<QString>& getAssetIds() const;

    bool isIncome() const;
    bool isSelfTx() const;
    bool isShieldedTx() const;
    bool isContractTx() const;
    bool isDexTx() const;
    beam::wallet::TxAddressType getAddressType();
    bool isSent() const;
    bool isReceived() const;
    virtual bool isCancelAvailable() const;
    virtual bool isDeleteAvailable() const;
    virtual bool isInProgress() const;
    virtual bool isPending() const;
    virtual bool isExpired() const;
    virtual bool isCompleted() const;
    virtual bool isCanceled() const;
    virtual bool isFailed() const;
    virtual bool isMultiAsset() const;

protected:
    [[nodiscard]] const beam::wallet::TxDescription& getTxDescription() const;
    [[nodiscard]] QString getReasonString(beam::wallet::TxFailureReason reason) const;

    beam::wallet::TxDescription _tx;
    uint32_t _minConfirmations = 0;
    beam::wallet::Currency _secondCurrency;
    QString _amountSecondCurrency;

    mutable QString _kernelIDStr;
    mutable QString _comment;
    boost::optional<beam::wallet::TxAddressType> _addressType;
    boost::optional<QString> _appid;

    beam::Amount _contractFee = 0UL;
    beam::bvm2::FundsMap _contractSpend;
    QString _contractCids;
    QString _source;

    std::vector<beam::Asset::ID> _assetsList;
    std::vector<QString>         _assetAmounts;
    std::vector<bool>            _assetAmountsIncome;
    std::vector<QString>         _assetRates;
    std::vector<QString>         _assetIDs;
};
