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
#include "viewmodel/qml_globals.h"
#include "wallet/core/common.h"
#include "wallet/core/simple_transaction.h"
#include "wallet/core/strings_resources.h"
#include "model/app_model.h"

using namespace beam;
using namespace beamui;

namespace
{
    QString getWaitingPeerStr(const beam::wallet::TxParameters& txParameters, bool isSender)
    {
        using namespace beam::wallet;

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
        using namespace beam::wallet;

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

TxObject::TxObject(beam::wallet::TxDescription tx, QObject* parent)
    : TxObject(std::move(tx), beam::wallet::Currency::UNKNOWN(), parent)
{
}

TxObject::TxObject(beam::wallet::TxDescription tx, beam::wallet::Currency secondCurrency, QObject* parent)
    : QObject(parent)
    , _tx(std::move(tx))
    , _secondCurrency(std::move(secondCurrency))
{
    Height h = tx.m_minHeight;
    _contractFee = std::max(_tx.m_fee, Transaction::FeeSettings::get(h).get_DefaultStd());

    if (_tx.m_txType == beam::wallet::TxType::Contract)
    {
        beam::bvm2::ContractInvokeData vData;
        if(_tx.GetParameter(beam::wallet::TxParameterID::ContractDataPacked, vData))
        {
            _contractFee = beam::bvm2::getFullFee(vData, h);
            _contractSpend = beam::bvm2::getFullSpend(vData);
        }

        if (!vData.empty())
        {
            std::stringstream ss;
            ss << vData[0].m_Cid.str();

            if (vData.size() > 1)
            {
                ss << " +" << vData.size() - 1;
            }

            _contractCids = QString::fromStdString(ss.str());
        }

        for (const auto& info: _contractSpend)
        {
            auto amount = info.second;
            if (info.first == beam::Asset::s_BeamID)
            {
                if (amount < 0)
                {
                    amount += _contractFee;
                }
            }
            _assetAmounts.emplace_back(AmountToUIString(std::abs(amount)));
            _assetsList.push_back(info.first);
            _assetAmountsIncome.push_back(amount <= 0);
            _assetRates.emplace_back(getRate(info.first));

            std::stringstream ss;
            ss << info.first;
            _assetIDs.emplace_back(QString::fromStdString(ss.str()));
        }
    }

    if (auto strdesc = _tx.GetParameter<std::string>(beam::wallet::TxParameterID::AppName); strdesc)
    {
        _source = QString::fromStdString(*strdesc);
    }
    else
    {
        //% "Wallet"
        _source = qtTrId("source-wallet");
    }

    if (_assetsList.empty())
    {
        _assetsList.push_back(_tx.m_assetId);
        _assetAmounts.emplace_back(AmountToUIString(_tx.m_amount));
        _assetAmountsIncome.push_back(!_tx.m_sender);
        _assetRates.emplace_back(getRate(_tx.m_assetId));

        std::stringstream ss;
        ss << _tx.m_assetId;
        _assetIDs.emplace_back(QString::fromStdString(ss.str()));
    }

    _minConfirmations = AppModel::getInstance().getSettings().getMinConfirmations();
}

bool TxObject::operator==(const TxObject& other) const
{
    return getTxID() == other.getTxID();
}

beam::Timestamp TxObject::timeCreated() const
{
    return _tx.m_createTime;
}

QString TxObject::getSource() const
{
    return _source;
}

uint32_t TxObject::getMinConfirmations() const
{
    return _minConfirmations;
}

QString TxObject::getConfirmationProgress() const
{
    if (_minConfirmations)
    {
        auto currHeight = AppModel::getInstance().getWalletModel()->getCurrentHeight();
        if (currHeight && _minConfirmations)
        {
            if (auto proofHeight = _tx.GetParameter<Height>(wallet::TxParameterID::KernelProofHeight); proofHeight)
            {
                std::stringstream ss;
                auto blocksAfter = (currHeight - *proofHeight);
                ss << (blocksAfter > _minConfirmations ? _minConfirmations : blocksAfter) << "/" << _minConfirmations;
                return QString::fromStdString(ss.str());
            }
            else
            {
                std::stringstream ss;
                ss << 0 << "/" << _minConfirmations;
                return QString::fromStdString(ss.str());
            }
        }
    }

    return "unknown";
}

beam::wallet::TxID TxObject::getTxID() const
{
    return _tx.m_txId;
}

bool TxObject::isIncome() const
{
    if (isContractTx())
    {
        for (const auto& info : _contractSpend) {
            if (info.second > 0)
                return false;
        }

        return true;
    }
    else
    {
        return !_tx.m_sender;
    }
}

const QString& TxObject::getComment() const
{
    if (_comment.isEmpty())
    {
        std::string str{_tx.m_message.begin(), _tx.m_message.end()};
        auto comment = QString(str.c_str()).trimmed();

        if (isContractTx())
        {
            if (comment.isEmpty())
            {
                //% "Contract transaction"
                comment = qtTrId("tx-contract-default-comment");
            }
        }
        else if (isDexTx())
        {
            // TODO:DEX just temporary
            comment = "DEX transaction";
        }

        _comment = comment;
    }

    return _comment;
}

QString TxObject::getRate(beam::Asset::ID assetId) const
{
    using namespace beam::wallet;

    auto rate = getTxDescription().getExchangeRate(_secondCurrency);
    return rate ? AmountToUIString(rate) : "0";
}

QString TxObject::getFeeRate() const
{
    return getRate(beam::Asset::s_BeamID);
}

QString TxObject::getCidsStr() const
{
    return _contractCids;
}

QString TxObject::getRate() const
{
    return getRate(_tx.m_assetId);
}

bool TxObject::isActive() const
{
    using namespace beam::wallet;

    return _tx.m_status == TxStatus::Pending ||
           _tx.m_status == TxStatus::InProgress ||
           _tx.m_status == TxStatus::Registering ||
           _tx.m_status == TxStatus::Confirming;
}

QString TxObject::getStatus() const
{
    std::unique_ptr<beam::wallet::TxStatusInterpreter> statusInterpreter;
    if (_tx.m_txType == wallet::TxType::Simple)
    {
        statusInterpreter = std::make_unique<beam::wallet::SimpleTxStatusInterpreter>(_tx);
    }
    else if (_tx.m_txType == wallet::TxType::PushTransaction)
    {
        statusInterpreter = std::make_unique<beam::wallet::MaxPrivacyTxStatusInterpreter>(_tx);
    }
    else if (_tx.m_txType >= wallet::TxType::AssetIssue && _tx.m_txType <= wallet::TxType::AssetInfo)
    {
        statusInterpreter = std::make_unique<beam::wallet::AssetTxStatusInterpreter>(_tx);
    }
    else if (_tx.m_txType == wallet::TxType::Contract)
    {
        statusInterpreter = std::make_unique<beam::wallet::ContractTxStatusInterpreter>(_tx);
    }
    else if (_tx.m_txType == wallet::TxType::DexSimpleSwap)
    {
        // TODO:DEX implement
        return "NOT IMPLEMENTED";
    }
    else
    {
        BOOST_ASSERT_MSG(false, kErrorUnknownTxType);
        return "unknown";
    }

    return statusInterpreter ? statusInterpreter->getStatus().c_str() : "unknown";
}

bool TxObject::isCancelAvailable() const
{
    return _tx.canCancel();
}

bool TxObject::isDeleteAvailable() const
{
    return _tx.canDelete();
}

QString TxObject::getAddressFrom() const
{
    if (_tx.m_txType == wallet::TxType::PushTransaction && !_tx.m_sender)
    {
        return getSenderIdentity();
    }
    return toString(_tx.m_sender ? _tx.m_myId : _tx.m_peerId);
}

QString TxObject::getAddressTo() const
{
    if (_tx.m_sender)
    {
        auto token = getToken();
        if (token.isEmpty())
            return toString(_tx.m_peerId);

        return token;
    }
    return toString(_tx.m_myId);
}

QString TxObject::getAmountGeneral() const
{
    if (isContractTx())
    {
        if (_assetAmounts.empty())
        {
            return QString("0");
        }

        return *_assetAmounts.begin();
    }
    else
    {
        return AmountToUIString(_tx.m_amount);
    }
}


QString TxObject::getFee() const
{
    if (isContractTx())
    {
        if (_contractFee)
        {
            return AmountToUIString(_contractFee);
        }
    }
    else if (_tx.m_fee)
    {
        // TODO(zavarza) no fee for shielded inputs after HF3
        Amount shieldedInputsFee = 0;
        std::vector<wallet::IPrivateKeyKeeper2::ShieldedInput> inputsShielded;
        if (_tx.GetParameter(wallet::TxParameterID::InputCoinsShielded, inputsShielded))
        {
            for (const auto& inputShielded : inputsShielded)
            {
                shieldedInputsFee += inputShielded.m_Fee;
            }
        }
        return AmountToUIString(shieldedInputsFee + _tx.m_fee);
    }
    return QString{};
}

const beam::wallet::TxDescription& TxObject::getTxDescription() const
{
    return _tx;
}

QString TxObject::getKernelID() const
{
    if (_kernelIDStr.isEmpty())
    {
        _kernelIDStr = QString::fromStdString(to_hex(_tx.m_kernelID.m_pData, _tx.m_kernelID.nBytes));
    }
    return _kernelIDStr;
}

QString TxObject::getTransactionID() const
{
    return QString::fromStdString(to_hex(_tx.m_txId.data(), _tx.m_txId.size()));
}

QString TxObject::getReasonString(beam::wallet::TxFailureReason reason) const
{
    // clang doesn't allow to make 'auto reasons' so for the moment assertions below are a bit pointles
    // let's wait until they fix template arg deduction and restore it back
    static const std::array<QString, beam::wallet::TxFailureReason::Count> reasons = {
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
    static_assert(std::tuple_size<decltype(reasons)>::value == static_cast<size_t>(beam::wallet::TxFailureReason::Count));

    assert(reasons.size() > static_cast<size_t>(reason));
    if (static_cast<size_t>(reason) >= reasons.size())
    {
        LOG_WARNING()  << "Unknown failure reason code " << reason << ". Defaulting to 0";
        reason = beam::wallet::TxFailureReason::Unknown;
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
            auto state = tx.GetParameter<wallet::SimpleTransaction::State>(beam::wallet::TxParameterID::State);
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

    if (tx.m_txType == beam::wallet::TxType::PushTransaction)
    {
        if (tx.m_status == beam::wallet::TxStatus::InProgress)
        {
            return getWaitingPeerStr(tx, tx.m_sender);
        }
        else if (tx.m_status == beam::wallet::TxStatus::Registering)
        {
            //% "The transaction is usually expected to complete in a few minutes."
            return qtTrId("tx-state-in-progress-normal");
        }
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
    return QString::fromStdString(_tx.getSenderIdentity());
}

QString TxObject::getReceiverIdentity() const
{
    return QString::fromStdString(_tx.getReceiverIdentity());
}

bool TxObject::isMultiAsset() const
{
    return _assetsList.size() > 1;
}

bool TxObject::hasPaymentProof() const
{
    return !isIncome()
           && _tx.m_status == beam::wallet::TxStatus::Completed
           && (_tx.m_txType == beam::wallet::TxType::Simple || _tx.m_txType == beam::wallet::TxType::PushTransaction);
}

bool TxObject::isInProgress() const
{
    switch (_tx.m_status)
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
    return _tx.m_status == wallet::TxStatus::Pending;
}

bool TxObject::isCompleted() const
{
    return _tx.m_status == wallet::TxStatus::Completed || _tx.m_status == wallet::TxStatus::Confirming;
}

bool TxObject::isSelfTx() const
{
    return _tx.m_selfTx;
}

bool TxObject::isShieldedTx() const
{
    return _tx.m_txType == beam::wallet::TxType::PushTransaction;
}

bool TxObject::isContractTx() const
{
    return _tx.m_txType == beam::wallet::TxType::Contract;
}

bool TxObject::isDexTx() const
{
    return _tx.m_txType == beam::wallet::TxType::DexSimpleSwap;
}

QString TxObject::getAppId()
{
    if (!_appid.is_initialized())
    {
        if(auto appid = _tx.GetParameter<std::string>(beam::wallet::TxParameterID::AppID))
        {
            _appid = QString::fromStdString(*appid);
        }
        else
        {
            _appid = "";
        }
    }
    return *_appid;
}

bool TxObject::isDappTx()
{
    return !getAppId().isEmpty();
}

beam::wallet::TxAddressType TxObject::getAddressType()
{
    if (!_addressType)
    {
        _addressType = _tx.GetParameter<beam::wallet::TxAddressType>(beam::wallet::TxParameterID::AddressType);
    }

    return _addressType ? *_addressType : beam::wallet::TxAddressType::Unknown;
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
    return _tx.m_status == wallet::TxStatus::Canceled;
}

bool TxObject::isFailed() const
{
    return _tx.m_status == wallet::TxStatus::Failed;
}

bool TxObject::isExpired() const
{
    return isFailed() && _tx.m_failureReason == beam::wallet::TxFailureReason::TransactionExpired;
}

const std::vector<beam::Asset::ID>& TxObject::getAssetsList() const
{
    return _assetsList;
}

const std::vector<QString>& TxObject::getAssetAmounts() const
{
    return _assetAmounts;
}

const std::vector<bool>& TxObject::getAssetAmountsIncome() const
{
    return _assetAmountsIncome;
}

const std::vector<QString>& TxObject::getAssetRates() const
{
    return _assetRates;
}

const std::vector<QString>& TxObject::getAssetIds() const
{
    return _assetIDs;
}

QString TxObject::getAmountSecondCurrency()
{
    if (_amountSecondCurrency.isEmpty())
    {
        // TODO: support multiple assets
        if (_assetsList.size() == 1)
        {
            _amountSecondCurrency = QMLGlobals::calcAmountInSecondCurrency(
                _assetAmounts[0],
                _assetRates[0],
                QString::fromStdString(_secondCurrency.m_value).toUpper());
        }
    }

    if (_amountSecondCurrency.isEmpty())
    {
        _amountSecondCurrency = "0";
    }

    return _amountSecondCurrency;
}
