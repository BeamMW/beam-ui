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
#pragma once

#include "viewmodel/wallet/tx_object.h"
#include "wallet/transactions/swaps/swap_tx_description.h"

class SwapTxObject : public TxObject
{
    // TODO: consider remove inheritance of TxObject
    Q_OBJECT

public:
    SwapTxObject(const beam::wallet::TxDescription& tx, uint32_t lockTxMinConfirmations,
        uint32_t withdrawTxMinConfirmations, double blocksPerHour, QObject* parent = nullptr);
    bool operator==(const SwapTxObject& other) const;

    auto getSentAmountWithCurrency() const -> QString;
    auto getSentAmount() const-> QString;
    auto getSentAmountValue() const -> beam::Amount;
    auto getReceivedAmountWithCurrency() const-> QString;
    auto getReceivedAmount() const -> QString;
    auto getReceivedAmountValue() const -> beam::Amount;
    auto getToken() const -> QString;
    auto getSwapCoinLockTxId() const -> QString;
    auto getSwapCoinLockTxConfirmations() const -> QString;
    auto getSwapCoinRedeemTxId() const -> QString;
    auto getSwapCoinRedeemTxConfirmations() const -> QString;
    auto getSwapCoinRefundTxId() const -> QString;
    auto getSwapCoinRefundTxConfirmations() const -> QString;
    auto getBeamLockTxKernelId() const -> QString;
    auto getBeamRedeemTxKernelId() const -> QString;
    auto getBeamRefundTxKernelId() const -> QString;
    auto getSwapCoinName() const -> QString;
    auto getSwapCoinFeeRate() const -> QString;
    auto getSwapCoinFee() const -> QString;
    auto getFee() const -> QString override;
    auto getFailureReason() const -> QString override;
    QString getStateDetails() const override;
    beam::wallet::AtomicSwapCoin getSwapCoinType() const;
    auto getStatus() const -> QString override;
    QString getAmountWithCurrency() const;

    bool isLockTxProofReceived() const;
    bool isRefundTxProofReceived() const;
    bool isBeamSideSwap() const;
    
    bool isCancelAvailable() const override;
    bool isDeleteAvailable() const override;
    bool isInProgress() const override;
    bool isPending() const override;
    bool isExpired() const override;
    bool isCompleted() const override;
    bool isCanceled() const override;
    bool isFailed() const override;

signals:

private:
    auto getSwapAmountValue(bool sent) const -> beam::Amount;
    auto getSwapAmountWithCurrency(bool sent) const -> QString;

    beam::wallet::SwapTxDescription m_swapTx;
    uint32_t m_lockTxMinConfirmations = 0;
    uint32_t m_withdrawTxMinConfirmations = 0;
    double m_blocksPerHour = 0;
};
