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

#include "tx_object.h"
#include "viewmodel/helpers/list_model.h"
#include "assets_manager.h"
#include <QLocale>

class TxObjectList : public ListModel<std::shared_ptr<TxObject>>
{
    Q_OBJECT
public:
    enum class Roles: int
    {
        TimeCreated = Qt::UserRole + 1,
        TimeCreatedSort,
        AmountGeneral,
        AmountGeneralSort,
        AmountSecondCurrency,
        AmountSecondCurrencySort,
        Rate,
        AddressFrom,
        AddressTo,
        Status,
        StatusSort,
        Fee,
        FeeRate,
        Comment,
        TxID,
        KernelID,
        FailureReason,
        IsCancelAvailable,
        IsDeleteAvailable,
        IsSelfTransaction,
        IsIncome,
        IsInProgress,
        IsPending,
        IsCompleted,
        IsCanceled,
        IsFailed,
        IsExpired,
        HasPaymentProof,
        RawTxID,
        Search,
        StateDetails,
        Token,
        SenderIdentity,
        ReceiverIdentity, 
        IsShieldedTx,
        IsOfflineToken,
        AssetNames,
        AssetNamesSort,
        IsSent,
        IsReceived,
        IsPublicOffline,
        IsMaxPrivacy,
        IsContractTx,
        IsMultiAsset,
        AssetFilter,
        AssetIcons,
        AssetAmounts,
        AssetAmountsIncome,
        AssetRates,
        AssetIDs,
        IsDexTx,
        CidsStr,
        Source,
        SourceSort,
        MinConfirmations,
        ConfirmationsProgress,
        IsDappTx,
    };
    Q_ENUM(Roles)

    TxObjectList();

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private slots:
    void onAssetInfo(beam::Asset::ID assetId);

private:
    AssetsManager::Ptr _amgr;
    QLocale m_locale;
};
