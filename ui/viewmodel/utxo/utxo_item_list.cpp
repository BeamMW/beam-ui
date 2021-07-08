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

#include "utxo_item_list.h"
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"

UtxoItemList::UtxoItemList()
    : _amgr(AppModel::getInstance().getAssets())
{
    connect(_amgr.get(), &AssetsManager::assetInfo, this,  &UtxoItemList::onAssetInfo);
}

QHash<int, QByteArray> UtxoItemList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        { static_cast<int>(Roles::Amount), "amount" },
        { static_cast<int>(Roles::AmountSort), "amountSort" },
        { static_cast<int>(Roles::Maturity), "maturity" },
        { static_cast<int>(Roles::MaturitySort), "maturitySort" },
        { static_cast<int>(Roles::Status), "status" },
        { static_cast<int>(Roles::StatusSort), "statusSort" },
        { static_cast<int>(Roles::Type), "type" },
        { static_cast<int>(Roles::TypeSort), "typeSort" },
        { static_cast<int>(Roles::MaturityPercentage), "maturityPercentage" },
        { static_cast<int>(Roles::MaturityPercentageSort), "maturityPercentageSort" },
        { static_cast<int>(Roles::MaturityTimeLeft), "maturityTimeLeft" },
        { static_cast<int>(Roles::MaturityTimeLeftSort), "maturityTimeLeftSort" },
        { static_cast<int>(Roles::UnitName), "unitName" },
        { static_cast<int>(Roles::IconSource), "iconSource" }
    };
    return roles;
}

auto UtxoItemList::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
       return QVariant();
    }
    
    auto& value = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::Amount:
            return beamui::AmountToUIString(value->rawAmount(), QString());
        case Roles::AmountSort:
            return static_cast<qulonglong>(value->rawAmount());
        case Roles::Maturity:
            return value->maturity();
        case Roles::MaturitySort:
            return static_cast<qulonglong>(value->rawMaturity());
        case Roles::Status:
        case Roles::StatusSort:
            return value->status();
        case Roles::Type:
        case Roles::TypeSort:
            return value->type();
        case Roles::MaturityPercentage:
        case Roles::MaturityPercentageSort:
            return value->maturityPercentage();
        case Roles::MaturityTimeLeft:
            return value->maturityTimeLeft();
        case Roles::MaturityTimeLeftSort:
            return value->rawMaturityTimeLeft();
        case Roles::UnitName:
            return _amgr->getUnitName(value->getAssetId(), AssetsManager::NoShorten);
        case Roles::IconSource:
            return _amgr->getIcon(value->getAssetId());
        default:
            return QVariant();
    }
}

void UtxoItemList::onAssetInfo(beam::Asset::ID assetId)
{
    touch(assetId);
}

void UtxoItemList::touch(beam::Asset::ID id)
{
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if ((*it)->getAssetId() == id) {
           const auto idx = it - m_list.begin();
           ListModel::touch(idx);
        }
    }
}
