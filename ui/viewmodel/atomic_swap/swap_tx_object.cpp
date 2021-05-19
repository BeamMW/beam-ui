// Copyright 2019 The Beam Team
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

#include "swap_tx_object.h"
#include "wallet/transactions/swaps/common.h"
#include "wallet/transactions/swaps/swap_transaction.h"
#include "core/ecc.h"
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"
#include "viewmodel/fee_helpers.h"

#include <qdebug.h>

using namespace beam;

namespace
{
    constexpr uint32_t kSecondsPerHour = 60 * 60;

    QString getWaitingPeerStr(const beam::wallet::SwapTxDescription& tx, Height currentHeight)
    {
        auto minHeight = tx.getMinHeight();
        auto responseTime = tx.getResponseTime();

        QString time = beamui::convertBeamHeightDiffToTime(minHeight + responseTime - currentHeight);
        //% "If nobody accepts the offer in %1 at most, the offer will be automatically canceled"
        return qtTrId("swap-tx-state-initial").arg(time);
    }

    QString getInProgressNormalStr(const beam::wallet::SwapTxDescription& tx, Height currentHeight)
    {
        if (tx.isRedeemTxRegistered())
        {
            return "";
        }
        
        QString time = "";
        auto minHeightRefund = tx.getMinRefundTxHeight();
        if (minHeightRefund)
        {
            if (currentHeight < *minHeightRefund)
            {
                time = beamui::convertBeamHeightDiffToTime(*minHeightRefund - currentHeight);
                //% "The swap is expected to complete in %1 at most."
                return qtTrId("swap-tx-state-in-progress-normal").arg(time);
            }
        }
        else {
            auto maxHeightLockTx = tx.getMaxLockTxHeight();
            if (maxHeightLockTx && currentHeight < *maxHeightLockTx)
            {
                time = beamui::convertBeamHeightDiffToTime(*maxHeightLockTx - currentHeight);
                //% "If the other side will not sign the transaction in %1, the offer will be canceled automatically."
                return qtTrId("swap-tx-state-in-progress-negotiation").arg(time);
            }
        }
        return "";
    }

    QString getInProgressRefundingStr(const beam::wallet::SwapTxDescription& tx, double blocksPerHour, Height currentBeamHeight)
    {
        
        if (tx.isRefundTxRegistered())
        {
            //% "Swap failed, the money is being released back to your wallet"
            return qtTrId("swap-tx-state-refunding");
        }

        QString time;
        QString coin;
        if (tx.isBeamSide())
        {
            auto refundMinHeight = tx.getMinRefundTxHeight();
            if (refundMinHeight && currentBeamHeight < *refundMinHeight)
            {
                time = beamui::convertBeamHeightDiffToTime(*refundMinHeight - currentBeamHeight);
                coin = "beam";
            }
        }
        else
        {
            auto currentCoinHeight = tx.getExternalHeight();
            auto lockTime = tx.getExternalLockTime();
            if (lockTime && currentCoinHeight && *currentCoinHeight < *lockTime && blocksPerHour)
            {
                double beamBlocksPerBlock = (kSecondsPerHour / beam::Rules().DA.Target_s) / blocksPerHour;
                double beamBlocks = (*lockTime - *currentCoinHeight) * beamBlocksPerBlock;
                time = beamui::convertBeamHeightDiffToTime(static_cast<int32_t>(std::round(beamBlocks)));
                coin = beamui::toString(beamui::convertSwapCoinToCurrency(tx.getSwapCoin()));
            }
        }
        if (time.isEmpty() || coin.isEmpty())
        {
            return "";
        }

        //% "Swap failed: the refund of your %2 will start in %1. The refund duration depends on the transaction fee you specified for %2."
        return qtTrId("swap-tx-state-in-progress-refunding").arg(time).arg(coin);
    }
}  // namespace

SwapTxObject::SwapTxObject(const beam::wallet::TxDescription& tx, uint32_t lockTxMinConfirmations,
    uint32_t withdrawTxMinConfirmations, double blocksPerHour, QObject* parent/* = nullptr*/)
        : TxObject(tx, parent),
          m_swapTx(tx),
          m_lockTxMinConfirmations(lockTxMinConfirmations),
          m_withdrawTxMinConfirmations(withdrawTxMinConfirmations),
          m_blocksPerHour(blocksPerHour)
{
}

bool SwapTxObject::operator==(const SwapTxObject& other) const
{
    return getTxID() == other.getTxID();
}

auto SwapTxObject::isBeamSideSwap() const -> bool
{
    return m_swapTx.isBeamSide();
}

bool SwapTxObject::isExpired() const
{
    return m_swapTx.isExpired();
}

bool SwapTxObject::isInProgress() const
{
    return _tx.m_status == wallet::TxStatus::Pending ||
           _tx.m_status == wallet::TxStatus::Registering ||
           _tx.m_status == wallet::TxStatus::InProgress;
}

bool SwapTxObject::isPending() const
{
    return _tx.m_status == wallet::TxStatus::Pending;
}

bool SwapTxObject::isCompleted() const
{
    return _tx.m_status == wallet::TxStatus::Completed;
}

bool SwapTxObject::isCanceled() const
{
    return _tx.m_status == wallet::TxStatus::Canceled;
}

bool SwapTxObject::isFailed() const
{
    return m_swapTx.isFailed();
}

bool SwapTxObject::isCancelAvailable() const
{
    return m_swapTx.isCancelAvailable();
}

bool SwapTxObject::isDeleteAvailable() const
{
    return _tx.m_status == wallet::TxStatus::Completed ||
           _tx.m_status == wallet::TxStatus::Canceled ||
           _tx.m_status == wallet::TxStatus::Failed;
}

auto SwapTxObject::getSwapCoinName() const -> QString
{
    return toString(beamui::convertSwapCoinToCurrency(m_swapTx.getSwapCoin()));
}

QString SwapTxObject::getSentAmountWithCurrency() const
{
    if (_tx.m_txType == beam::wallet::TxType::AtomicSwap)
    {
        return getSwapAmountWithCurrency(true);
    }
    return _tx.m_sender ? getAmountWithCurrency() : "";
}

QString SwapTxObject::getAmountWithCurrency() const
{
    return AmountToUIString(_tx.m_amount, beamui::Currencies::Beam);
}

QString SwapTxObject::getSentAmount() const
{
    QString amount = beamui::AmountToUIString(getSentAmountValue());
    return amount == "0" ? "" : amount;
}

beam::Amount SwapTxObject::getSentAmountValue() const
{
    if (_tx.m_txType == beam::wallet::TxType::AtomicSwap)
    {
        return getSwapAmountValue(true);
    }

    return _tx.m_sender ? _tx.m_amount : 0;
}

QString SwapTxObject::getReceivedAmountWithCurrency() const
{
    if (_tx.m_txType == beam::wallet::TxType::AtomicSwap)
    {
        return getSwapAmountWithCurrency(false);
    }
    return !_tx.m_sender ? getAmountWithCurrency() : "";
}

QString SwapTxObject::getReceivedAmount() const
{
    QString amount = beamui::AmountToUIString(getReceivedAmountValue());
    return amount == "0" ? "" : amount;
}

beam::Amount SwapTxObject::getReceivedAmountValue() const
{
    if (_tx.m_txType == beam::wallet::TxType::AtomicSwap)
    {
        return getSwapAmountValue(false);
    }
    return !_tx.m_sender ? _tx.m_amount : 0;
}

QString SwapTxObject::getSwapAmountWithCurrency(bool sent) const
{
    bool isBeamSide = m_swapTx.isBeamSide();
    bool s = sent ? !isBeamSide : isBeamSide;
    if (s)
    {
        return AmountToUIString(m_swapTx.getSwapAmount(), beamui::convertSwapCoinToCurrency(m_swapTx.getSwapCoin()));
    }
    return getAmountWithCurrency();
}

beam::Amount SwapTxObject::getSwapAmountValue(bool sent) const
{
    bool isBeamSide = m_swapTx.isBeamSide();
    bool s = sent ? !isBeamSide : isBeamSide;
    if (s)
    {
        return m_swapTx.getSwapAmount();
    }
    return _tx.m_amount;
}

QString SwapTxObject::getFee() const
{
    auto fee = m_swapTx.getFee();
    if (fee)
    {
        return beamui::AmountInGrothToUIString(*fee);
    }
    return QString();
}

QString SwapTxObject::getSwapCoinFeeRate() const
{
    auto feeRate = m_swapTx.getSwapCoinFeeRate();
    if (feeRate)
    {
        QString value = QString::number(*feeRate);
        QString rateMeasure = beamui::getFeeRateLabel(beamui::convertSwapCoinToCurrency(m_swapTx.getSwapCoin()));
        return value + " " + rateMeasure;
    }
    return QString();
}

QString SwapTxObject::getSwapCoinFee() const
{
    auto feeRate = m_swapTx.getSwapCoinFeeRate();
    if (!feeRate)
    {
        return QString();
    }

    const auto coinTypeQt = convertSwapCoinToCurrency(m_swapTx.getSwapCoin());
    return calcWithdrawTxFee(coinTypeQt, *feeRate);
}

QString SwapTxObject::getFailureReason() const
{
    if (m_swapTx.isRefunded())
    {
        //% "Refunded"
        return qtTrId("swap-tx-failure-refunded");
    }
    auto failureReason = m_swapTx.getFailureReason();
    return failureReason ? getReasonString(*failureReason) : QString();
}

QString SwapTxObject::getStateDetails() const
{
    if (getTxDescription().m_txType == beam::wallet::TxType::AtomicSwap)
    {
        switch (getTxDescription().m_status)
        {
        case beam::wallet::TxStatus::Pending:
        case beam::wallet::TxStatus::InProgress:
        {
            Height currentHeight = AppModel::getInstance().getWalletModel()->getCurrentHeight();
            auto state = m_swapTx.getState();
            if (state)
            {
                switch (*state)
                {
                case wallet::AtomicSwapTransaction::State::Initial:
                    return getWaitingPeerStr(m_swapTx, currentHeight);
                case wallet::AtomicSwapTransaction::State::BuildingBeamLockTX:
                case wallet::AtomicSwapTransaction::State::BuildingBeamRefundTX:
                case wallet::AtomicSwapTransaction::State::BuildingBeamRedeemTX:
                case wallet::AtomicSwapTransaction::State::HandlingContractTX:
                case wallet::AtomicSwapTransaction::State::SendingBeamLockTX:
                    return getInProgressNormalStr(m_swapTx, currentHeight);
                case wallet::AtomicSwapTransaction::State::SendingRedeemTX:
                case wallet::AtomicSwapTransaction::State::SendingBeamRedeemTX:
                    return getInProgressNormalStr(m_swapTx, currentHeight);
                case wallet::AtomicSwapTransaction::State::SendingRefundTX:
                case wallet::AtomicSwapTransaction::State::SendingBeamRefundTX:
                    return getInProgressRefundingStr(m_swapTx, m_blocksPerHour, currentHeight);
                default:
                    break;
                }
            }
            else
            {
                return getWaitingPeerStr(m_swapTx, currentHeight);
            }
            break;
        }
        default:
            break;
        }
    }
    return "";
}

beam::wallet::AtomicSwapCoin SwapTxObject::getSwapCoinType() const
{
    return m_swapTx.getSwapCoin();
}

auto SwapTxObject::getStatus() const -> QString
{
    beam::wallet::SwapTxStatusInterpreter interpreter(getTxDescription());
    return interpreter.getStatus().c_str();
}

namespace
{
    template<beam::wallet::SubTxIndex SubTxId>
    QString getSwapCoinTxId(const beam::wallet::SwapTxDescription& swapTxDescription)
    {
        if (auto res = swapTxDescription.getSwapCoinTxId<SubTxId>(); res)
        {
            return QString::fromStdString(*res);
        }
        else return QString();
    }
    
    template<beam::wallet::SubTxIndex SubTxId>
    QString getSwapCoinTxConfirmations(const beam::wallet::SwapTxDescription& swapTxDescription, uint32_t minTxConfirmations)
    {
        if (auto res = swapTxDescription.getSwapCoinTxConfirmations<SubTxId>(); res)
        {
            std::string result;
            if (minTxConfirmations)
            {
                result = (*res > minTxConfirmations) ? std::to_string(minTxConfirmations) : std::to_string(*res);
                result += "/" + std::to_string(minTxConfirmations);
            }
            else
            {
                result = std::to_string(*res);
            }
            return QString::fromStdString(result);
        }
        return QString();
    }

    template<beam::wallet::SubTxIndex SubTxId>
    QString getBeamTxKernelId(const beam::wallet::SwapTxDescription& swapTxDescription)
    {
        if (auto res = swapTxDescription.getBeamTxKernelId<SubTxId>(); res)
        {
            return QString::fromStdString(*res);
        }
        return QString();
    }
}

QString SwapTxObject::getToken() const
{
    auto swapToken = m_swapTx.getToken();
    if (swapToken)
    {
        return QString::fromStdString(*swapToken);
    }
    return QString();
}

bool SwapTxObject::isLockTxProofReceived() const
{
    return m_swapTx.isLockTxProofReceived();
}

bool SwapTxObject::isRefundTxProofReceived() const
{
    return m_swapTx.isRefundTxProofReceived();
}

QString SwapTxObject::getSwapCoinLockTxId() const
{
    return getSwapCoinTxId<beam::wallet::SubTxIndex::LOCK_TX>(m_swapTx);
}

QString SwapTxObject::getSwapCoinRedeemTxId() const
{
    return getSwapCoinTxId<beam::wallet::SubTxIndex::REDEEM_TX>(m_swapTx);
}

QString SwapTxObject::getSwapCoinRefundTxId() const
{
    return getSwapCoinTxId<beam::wallet::SubTxIndex::REFUND_TX>(m_swapTx);
}

QString SwapTxObject::getSwapCoinLockTxConfirmations() const
{
    return getSwapCoinTxConfirmations<beam::wallet::SubTxIndex::LOCK_TX>(m_swapTx, m_lockTxMinConfirmations);
}

QString SwapTxObject::getSwapCoinRedeemTxConfirmations() const
{
    return getSwapCoinTxConfirmations<beam::wallet::SubTxIndex::REDEEM_TX>(m_swapTx, m_withdrawTxMinConfirmations);
}

QString SwapTxObject::getSwapCoinRefundTxConfirmations() const
{
    return getSwapCoinTxConfirmations<beam::wallet::SubTxIndex::REFUND_TX>(m_swapTx, m_withdrawTxMinConfirmations);
}

QString SwapTxObject::getBeamLockTxKernelId() const
{
    return getBeamTxKernelId<beam::wallet::SubTxIndex::BEAM_LOCK_TX>(m_swapTx);
}

QString SwapTxObject::getBeamRedeemTxKernelId() const
{
    return getBeamTxKernelId<beam::wallet::SubTxIndex::REDEEM_TX>(m_swapTx);
}

QString SwapTxObject::getBeamRefundTxKernelId() const
{
    return getBeamTxKernelId<beam::wallet::SubTxIndex::REFUND_TX>(m_swapTx);
}
