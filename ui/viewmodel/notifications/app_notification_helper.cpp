// Copyright 2021 The Beam Team
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

#include "app_notification_helper.h"
#include "model/app_model.h"
#include "utility/hex.h"

AppNotificationHelper::AppNotificationHelper()
{
    auto walletPtr = AppModel::getInstance().getWalletModel().get();

    const auto averageBlockTime = walletPtr->getAverageBlockTime();
    const auto now = beam::getTimestamp();
    auto timeFromLastBlock = now - walletPtr->getLastBlockTime();
    m_estimateBlockTime = timeFromLastBlock > averageBlockTime ? 1 : averageBlockTime - timeFromLastBlock;

    connect(
        walletPtr,
        &WalletModel::transactionsChanged,
        this,
        &AppNotificationHelper::onTransactionsChanged);
}

QString AppNotificationHelper::getTxId() const
{
    return std::to_string(m_txId).c_str();
}

void AppNotificationHelper::setTxId(QString txID)
{
    auto txIdVec = beam::from_hex(txID.toStdString());
    std::copy_n(txIdVec.begin(), 16, m_txId.begin());
    emit txIdChanged();
}

qlonglong AppNotificationHelper::getEstimateBlockTime() const
{
    return static_cast<qlonglong>(m_estimateBlockTime);
}

void AppNotificationHelper::onTransactionsChanged(ChangeAction action, const std::vector<TxDescription>& items)
{
    if (action == ChangeAction::Updated)
    {
        for (const auto& tx : items)
        {
            const auto txType = tx.GetParameter<TxType>(TxParameterID::TransactionType);
            if (txType == TxType::Contract)
            {
                auto txId = tx.GetTxID();
                if (txId && *txId == m_txId)
                {
                    auto status = tx.GetParameter<TxStatus>(TxParameterID::Status);
                    if (status && (*status == TxStatus::Failed ||
                                   *status == TxStatus::Canceled ||
                                   *status == TxStatus::Completed))
                    {
                        emit txFinished();
                    }
                }
            }
        }
    }
}
