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
#include "tx_object.h"
#include "viewmodel/ui_helpers.h"
#include "wallet/core/common.h"
#include "wallet/core/simple_transaction.h"
#include "wallet/core/strings_resources.h"
#include "model/app_model.h"

using namespace beam;
using namespace beam::wallet;
using namespace beamui;

namespace
{
    QString getWaitingPeerStr(const beam::wallet::TxParameters& txParameters, bool isSender)
    {
        auto minHeight = txParameters.GetParameter<beam::Height>(TxParameterID::MinHeight);
        auto responseTime = txParameters.GetParameter<beam::Height>(TxParameterID::PeerResponseTime);
        QString time = "";
        if (minHeight && responseTime)
        {
            time = convertBeamHeightDiffToTime(*minHeight + *responseTime - AppModel::getInstance().getWalletModel()->getCurrentHeight());
        }
        if (isSender)
        {
            //% "If the receiver doesn't get online in %1, the transaction will be canceled"
            return qtTrId("tx-state-initial-sender").arg(time);
        }
        //% "If the sender doesn't get online in %1, the transaction will be canceled"
        return qtTrId("tx-state-initial-receiver").arg(time);
    }

    QString getInProgressStr(const beam::wallet::TxParameters& txParameters)
    {
        const Height kNormalTxConfirmationDelay = 10;
        auto maxHeight = txParameters.GetParameter<beam::Height>(TxParameterID::MaxHeight);
        QString time = "";
        if (!maxHeight)
        {
            return "";
        }

        auto currentHeight = AppModel::getInstance().getWalletModel()->getCurrentHeight();
        if (currentHeight >= *maxHeight)
        {
            return "";
        }

        Height delta =  *maxHeight - currentHeight;
        auto lifetime = txParameters.GetParameter<beam::Height>(TxParameterID::Lifetime);
        if (!lifetime || *lifetime < delta)
        {
            return "";
        }

        if (*lifetime - delta <= kNormalTxConfirmationDelay)
        {
            //% "The transaction is usually expected to complete in a few minutes."
            return qtTrId("tx-state-in-progress-normal");
        }

        time = beamui::convertBeamHeightDiffToTime(delta);
        if (time.isEmpty())
        {
            return "";
        }
        //% "It is taking longer than usual. In case the transaction could not be completed it will be canceled automatically in %1."
        return qtTrId("tx-state-in-progress-long").arg(time);
    }
}


TxObject::TxObject( const TxDescription& tx,
                    QObject* parent/* = nullptr*/)
        : TxObject(tx, beam::wallet::ExchangeRate::Currency::Unknown, parent)
{
}

TxObject::TxObject( const TxDescription& tx,
                    beam::wallet::ExchangeRate::Currency secondCurrency,
                    QObject* parent/* = nullptr*/)
        : QObject(parent)
        , m_tx(tx)
        , m_type(*m_tx.GetParameter<TxType>(TxParameterID::TransactionType))
        , m_secondCurrency(secondCurrency)
{
    m_kernelID = QString::fromStdString(to_hex(tx.m_kernelID.m_pData, tx.m_kernelID.nBytes));
    if (m_tx.m_txType == TxType::Contract)
    {
        _contractAmount = 0;
        visitContractData([this](const beam::bvm2::ContractInvokeData &data) {
            for (const auto &spend: data.m_Spend)
            {
                _contractAssets.insert(spend.first);
                _contractAmount -= spend.second;
                _contractFee += data.m_Fee;
            }
        });

        if (_contractAmount > 0) {
            _contractAmount -= _contractFee;
        }
    }
}

bool TxObject::operator==(const TxObject& other) const
{
    return getTxID() == other.getTxID();
}

auto TxObject::timeCreated() const -> beam::Timestamp
{
    return m_tx.m_createTime;
}

auto TxObject::getTxID() const -> beam::wallet::TxID
{
    return m_tx.m_txId;
}

bool TxObject::isIncome() const
{
    if (isContractTx())
    {
        return _contractAmount > 0;
    }
    else
    {
        return m_tx.m_sender == false;
    }
}

QString TxObject::getComment() const
{
    std::string str{m_tx.m_message.begin(), m_tx.m_message.end()};
    auto comment = QString(str.c_str()).trimmed();

    if (isContractTx())
    {
        //% "Contract transaction"
        return comment.isEmpty() ? qtTrId("tx-contract-default-comment") : comment;
    }
    else if (isDexTx())
    {
        // TODO:DEX just temporary
        return "DEX transaction";
    }
    else
    {
        return comment;
    }
}

QString TxObject::getAmount() const
{
    if (isContractTx())
    {
        return AmountToUIString(std::abs(_contractAmount));
    }
    else
    {
        return AmountToUIString(m_tx.m_amount);
    }
}

beam::Amount TxObject::getAmountValue() const
{
    if (isContractTx())
    {
        return std::abs(_contractAmount);
    }
    else
    {
        return m_tx.m_amount;
    }
}

QString TxObject::getRate() const
{
    if (m_tx.m_assetId != Asset::s_BeamID)
    {
        return "0";
    }

    auto exchangeRatesOptional = getTxDescription().GetParameter<std::vector<ExchangeRate>>(TxParameterID::ExchangeRates);
    if (exchangeRatesOptional)
    {
        std::vector<ExchangeRate>& rates = *exchangeRatesOptional;
        auto secondCurrency = m_secondCurrency;
        auto search = std::find_if(std::begin(rates),
                                   std::end(rates),
                                   [secondCurrency](const ExchangeRate& r)
                                   {
                                       return r.m_currency == ExchangeRate::Currency::Beam
                                           && r.m_unit == secondCurrency;
                                   });
        if (search != std::cend(rates))
        {
            return AmountToUIString(search->m_rate);
        }
    }

    return "0";
}

QString TxObject::getStatus() const
{
    if (m_tx.m_txType == wallet::TxType::Simple)
    {
        SimpleTxStatusInterpreter interpreter(m_tx);
        return interpreter.getStatus().c_str();
    }
    else if (m_tx.m_txType == wallet::TxType::PushTransaction)
    {
        MaxPrivacyTxStatusInterpreter interpreter(m_tx);
        return interpreter.getStatus().c_str();
    }
    else if (m_tx.m_txType >= wallet::TxType::AssetIssue && m_tx.m_txType <= wallet::TxType::AssetInfo)
    {
        AssetTxStatusInterpreter interpreter(m_tx);
        return interpreter.getStatus().c_str();
    }
    else if (m_tx.m_txType == wallet::TxType::Contract)
    {
        ContractTxStatusInterpreter interpreter(m_tx);
        return interpreter.getStatus().c_str();
    }
    else if (m_tx.m_txType == wallet::TxType::DexSimpleSwap)
    {
        // TODO:DEX implement
        return "NOT IMPLEMENTED";
    }
    else
    {
        BOOST_ASSERT_MSG(false, kErrorUnknownTxType);
        return "unknown";
    }
}

bool TxObject::isCancelAvailable() const
{
    return m_tx.canCancel();
}

bool TxObject::isDeleteAvailable() const
{
    return m_tx.canDelete();
}

QString TxObject::getAddressFrom() const
{
    if (m_tx.m_txType == wallet::TxType::PushTransaction && !m_tx.m_sender)
    {
        return getSenderIdentity();
    }
    return toString(m_tx.m_sender ? m_tx.m_myId : m_tx.m_peerId);
}

QString TxObject::getAddressTo() const
{
    if (m_tx.m_sender)
    {
        auto token = getToken();
        if (token.isEmpty())
            return toString(m_tx.m_peerId);

        return token;
    }
    return toString(m_tx.m_myId);
}

QString TxObject::getFee() const
{
    if (isContractTx())
    {
        if (_contractFee)
        {
            return AmountInGrothToUIString(_contractFee);
        }
    }
    else if (m_tx.m_fee)
    {
        Amount shieldedFee = GetShieldedFee(m_tx);
        return AmountInGrothToUIString(shieldedFee + m_tx.m_fee);
    }
    return QString{};
}

const beam::wallet::TxDescription& TxObject::getTxDescription() const
{
    return m_tx;
}

QString TxObject::getKernelID() const
{
    return m_kernelID;
}

QString TxObject::getTransactionID() const
{
    return QString::fromStdString(to_hex(m_tx.m_txId.data(), m_tx.m_txId.size()));
}

QString TxObject::getReasonString(beam::wallet::TxFailureReason reason) const
{
    // clang doesn't allow to make 'auto reasons' so for the moment assertions below are a bit pointles
    // let's wait until they fix template arg deduction and restore it back
    static const std::array<QString, TxFailureReason::Count> reasons = {
        //% "Unexpected reason, please send wallet logs to Beam support" 
        qtTrId("tx-failure-undefined"),
        //% "Transaction cancelled"
        qtTrId("tx-failure-cancelled"),
        //% "Receiver signature in not valid, please send wallet logs to Beam support"
        qtTrId("tx-failure-receiver-signature-invalid"),
        //% "Failed to register transaction with the blockchain, see node logs for details"
        qtTrId("tx-failure-not-registered-in-blockchain"),
        //% "Transaction is not valid, please send wallet logs to Beam support"
        qtTrId("tx-failure-not-valid"),
        //% "Invalid kernel proof provided"
        qtTrId("tx-failure-kernel-invalid"),
        //% "Failed to send Transaction parameters"
        qtTrId("tx-failure-parameters-not-sended"),
        //% "Not enough inputs to process the transaction"
        qtTrId("tx-failure-no-inputs"),
        //% "Address is expired"
        qtTrId("tx-failure-addr-expired"),
        //% "Failed to get transaction parameters"
        qtTrId("tx-failure-parameters-not-readed"),
        //% "Transaction timed out"
        qtTrId("tx-failure-time-out"),
        //% "Payment not signed by the receiver, please send wallet logs to Beam support"
        qtTrId("tx-failure-not-signed-by-receiver"),
        //% "Kernel maximum height is too high"
        qtTrId("tx-failure-max-height-to-high"),
        //% "Transaction has invalid state"
        qtTrId("tx-failure-invalid-state"),
        //% "Subtransaction has failed"
        qtTrId("tx-failure-subtx-failed"),
        //% "Contract's amount is not valid"
        qtTrId("tx-failure-invalid-contract-amount"),
        //% "Side chain has invalid contract"
        qtTrId("tx-failure-invalid-sidechain-contract"),
        //% "Side chain bridge has internal error"
        qtTrId("tx-failure-sidechain-internal-error"),
        //% "Side chain bridge has network error"
        qtTrId("tx-failure-sidechain-network-error"),
        //% "Side chain bridge has response format error"
        qtTrId("tx-failure-invalid-sidechain-response-format"),
        //% "Invalid credentials of Side chain"
        qtTrId("tx-failure-invalid-side-chain-credentials"),
        //% "Not enough time to finish btc lock transaction"
        qtTrId("tx-failure-not-enough-time-btc-lock"),
        //% "Failed to create multi-signature"
        qtTrId("tx-failure-create-multisig"),
        //% "Fee is too small"
        qtTrId("tx-failure-fee-too-small"),
        //% "Fee is too large"
        qtTrId("tx-failure-fee-too-large"),
        //% "Kernel's min height is unacceptable"
        qtTrId("tx-failure-kernel-min-height"),
        //% "Not a loopback transaction"
        qtTrId("tx-failure-loopback"),
        //% "Key keeper is not initialized"
        qtTrId("tx-failure-key-keeper-no-initialized"),
        //% "No valid asset id/asset owner id"
        qtTrId("tx-failure-invalid-asset-id"),
        //% "No asset info or asset info is not valid"
        qtTrId("tx-failure-asset-invalid-info"),
        //% "No asset metadata or asset metadata is not valid"
        qtTrId("tx-failure-asset-invalid-metadata"),
        //% "Invalid asset id"
        qtTrId("tx-failure-asset-invalid-id"),
        //% "Failed to receive asset confirmation"
        qtTrId("tx-failure-asset-confirmation"),
        //% "Asset is still in use (issued amount > 0)"
        qtTrId("tx-failure-asset-in-use"),
        //% "Asset is still locked"
        qtTrId("tx-failure-asset-locked"),
        //% "Asset registration fee is too small"
        qtTrId("tx-failure-asset-small-fee"),
        //% "Cannot issue/consume more than MAX_INT64 asset groth in one transaction"
        qtTrId("tx-failure-invalid-asset-amount"),
        //% "Some mandatory data for payment proof is missing"
        qtTrId("tx-failure-invalid-data-for-payment-proof"),
        //%  "Master key is needed for this transaction, but unavailable"
        qtTrId("tx-failure-there-is-no-master-key"),
        //% "Key keeper malfunctioned"
        qtTrId("tx-failure-keeper-malfunctioned"),
        //% "Aborted by the user"
        qtTrId("tx-failure-aborted-by-user"),
        //% "Asset has been already registered"
        qtTrId("tx-failure-asset-exists"),
        //% "Invalid asset owner id"
        qtTrId("tx-failure-asset-invalid-owner-id"),
        //% "Asset transactions are disabled in the wallet"
        qtTrId("tx-failure-assets-disabled"),
        //% "No voucher, no address to receive it"
        qtTrId("tx-failure-no-vouchers"),
        //% "Asset transactions are not available until fork2"
        qtTrId("tx-failure-assets-fork2"),
        //% "Key keeper out of slots"
        qtTrId("tx-failure-out-of-slots"),
        //% "Cannot extract shielded coin, fee is too big."
        qtTrId("tx-failure-shielded-coin-fee"),
        //% "Asset transactions are disabled in the receiver wallet"
        qtTrId("tx-failure-assets-disabled-receiver"),
        //% "Asset transactions are disabled in blockchain configuration"
        qtTrId("tx-failure-assets-disabled-blockchain"),
        //% "Peer Identity required"
        qtTrId("tx-failure-identity-required"),
        //% "The sender cannot get vouchers for max privacy transaction"
        qtTrId("tx-failure-cannot-get-vouchers")
    };

    // ensure QString
    static_assert(std::is_same<decltype(reasons)::value_type, QString>::value);
    // ensure that we have all reasons, otherwise it would be runtime crash
    static_assert(std::tuple_size<decltype(reasons)>::value == static_cast<size_t>(TxFailureReason::Count));

    assert(reasons.size() > static_cast<size_t>(reason));
    if (static_cast<size_t>(reason) >= reasons.size())
    {
        LOG_WARNING()  << "Unknown failure reason code " << reason << ". Defaulting to 0";
        reason = TxFailureReason::Unknown;
    }

    return reasons[reason];
}

QString TxObject::getFailureReason() const
{
    if (getTxDescription().m_status == wallet::TxStatus::Failed)
    {
        return getReasonString(getTxDescription().m_failureReason);
    }

    return QString();
}

QString TxObject::getStateDetails() const
{
    auto& tx = getTxDescription();
    if (tx.m_txType == beam::wallet::TxType::Simple)
    {
        switch (tx.m_status)
        {
        case beam::wallet::TxStatus::Pending:
        case beam::wallet::TxStatus::InProgress:
        {
            auto state = tx.GetParameter<wallet::SimpleTransaction::State>(TxParameterID::State);
            if (state)
            {
                switch (*state)
                {
                case wallet::SimpleTransaction::Initial:
                case wallet::SimpleTransaction::Invitation:
                    return getWaitingPeerStr(tx, tx.m_sender);
                default:
                    break;
                }
            }
            return getWaitingPeerStr(tx, tx.m_sender);
        }
        case beam::wallet::TxStatus::Registering:
            return getInProgressStr(tx);
        default:
            break;
        }
    }

    if (tx.m_txType == beam::wallet::TxType::PushTransaction &&
        (tx.m_status == beam::wallet::TxStatus::InProgress || tx.m_status == beam::wallet::TxStatus::Registering))
    {
        //% "The transaction is usually expected to complete in a few minutes."
        return qtTrId("tx-state-in-progress-normal");
    }
    return "";
}

QString TxObject::getToken() const
{
    const auto& tx = getTxDescription();
    return QString::fromStdString(tx.getToken());
}

QString TxObject::getSenderIdentity() const
{
    return QString::fromStdString(m_tx.getSenderIdentity());
}

QString TxObject::getReceiverIdentity() const
{
    return QString::fromStdString(m_tx.getReceiverIdentity());
}

std::set<beam::Asset::ID> TxObject::getAssetsList() const
{
    // TODO:DEX implement
    if (isContractTx())
    {
        return _contractAssets;
    }

    std::set<beam::Asset::ID> alist = {m_tx.m_assetId};
    return alist;
}

bool TxObject::hasPaymentProof() const
{
    return !isIncome() && m_tx.m_status == wallet::TxStatus::Completed 
        && (m_tx.m_txType == TxType::Simple || m_tx.m_txType == TxType::PushTransaction);
}

bool TxObject::isInProgress() const
{
    switch (m_tx.m_status)
    {
        case wallet::TxStatus::Pending:
        case wallet::TxStatus::InProgress:
        case wallet::TxStatus::Registering:
            return true;
        default:
            return false;
    }
}

bool TxObject::isPending() const
{
    return m_tx.m_status == wallet::TxStatus::Pending;
}

bool TxObject::isCompleted() const
{
    return m_tx.m_status == wallet::TxStatus::Completed;
}

bool TxObject::isSelfTx() const
{
    return m_tx.m_selfTx;
}

bool TxObject::isShieldedTx() const
{
    return m_tx.m_txType == TxType::PushTransaction;
}

bool TxObject::isContractTx() const
{
    return m_tx.m_txType == TxType::Contract;
}

bool TxObject::isDexTx() const
{
    return m_tx.m_txType == TxType::DexSimpleSwap;
}

beam::wallet::TxAddressType TxObject::getAddressType()
{
    restoreAddressType();

    if (m_addressType)
        return *m_addressType;

    return TxAddressType::Unknown;
}

bool TxObject::isSent() const
{
    return isCompleted() && !isIncome();
}

bool TxObject::isReceived() const
{
    return isCompleted() && isIncome();
}

bool TxObject::isCanceled() const
{
    return m_tx.m_status == wallet::TxStatus::Canceled;
}

bool TxObject::isFailed() const
{
    return m_tx.m_status == wallet::TxStatus::Failed;
}

bool TxObject::isExpired() const
{
    return isFailed() && m_tx.m_failureReason == TxFailureReason::TransactionExpired;
}

void TxObject::restoreAddressType()
{
    auto storedType = m_tx.GetParameter<TxAddressType>(TxParameterID::AddressType);
    if (storedType)
    {
        m_addressType = storedType;
        return;
    }

    if (!m_tx.m_sender || m_addressType)
        return;

    m_addressType = GetAddressType(m_tx);
    m_tx.SetParameter(TxParameterID::AddressType, *m_addressType);
}

void TxObject::visitContractData(const CDVisitor& visitor) const
{
    std::vector<beam::bvm2::ContractInvokeData> vData;
    if(m_tx.GetParameter(TxParameterID::ContractDataPacked, vData))
    {
        for (const auto& data: vData)
        {
            visitor(data);
        }
    }
}
