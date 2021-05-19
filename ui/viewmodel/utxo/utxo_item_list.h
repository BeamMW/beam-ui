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

#include "utxo_item.h"
#include "viewmodel/helpers/list_model.h"
#include "viewmodel/wallet/assets_manager.h"

class UtxoItemList : public ListModel<std::shared_ptr<BaseUtxoItem>>
{
    Q_OBJECT
public:
    enum class Roles
    {
        Amount = Qt::UserRole + 1,
        AmountSort,
        Maturity,
        MaturitySort,
        Status,
        StatusSort,
        Type,
        TypeSort,
        MaturityPercentage,
        MaturityPercentageSort,
        MaturityTimeLeft,
        MaturityTimeLeftSort,
        UnitName,
    };

    UtxoItemList();
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public slots:
    void onAssetInfo(beam::Asset::ID assetId);

private:
    void touch(beam::Asset::ID id);
    AssetsManager::Ptr _amgr;
};
