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

class TxObject : public QObject
{
    Q_OBJECT

public:
    TxObject(const beam::wallet::TxDescription& tx,
             QObject* parent = nullptr);
    TxObject(const beam::wallet::TxDescription& tx,
             beam::wallet::ExchangeRate::Currency secondCurrency,
             QObject* parent = nullptr);
    bool operator==(const TxObject& other) const;

    beam::Timestamp timeCreated() const;
    beam::wallet::TxID getTxID() const;
    beam::Amount getAmountValue() const;
    QString getAmount() const;
    QString getRate() const;
    QString getComment() const;
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
    std::set<beam::Asset::ID> getAssetsList() const;

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

protected:
    const beam::wallet::TxDescription& getTxDescription() const;
    QString getReasonString(beam::wallet::TxFailureReason reason) const;
    QString getIdentity(bool isSender) const;
    void restoreAddressType();
 
    beam::wallet::TxDescription m_tx;
    QString m_kernelID;
    beam::wallet::TxType m_type;
    beam::wallet::ExchangeRate::Currency m_secondCurrency;
    mutable  boost::optional<bool> m_hasVouchers;
    boost::optional<beam::wallet::TxAddressType> m_addressType;

    //
    // Contracts handling
    //
    typedef std::function<void (const beam::bvm2::ContractInvokeData& data)> CDVisitor;
    void visitContractData(const CDVisitor&) const;
    beam::AmountSigned _contractAmount = 0;
    beam::Amount _contractFee = 0U;
    std::set<beam::Asset::ID> _contractAssets;
};
