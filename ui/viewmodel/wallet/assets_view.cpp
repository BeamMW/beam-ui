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
#include "assets_view.h"
#include "model/app_model.h"

AssetsViewModel::AssetsViewModel()
    : _wallet(AppModel::getInstance().getWalletModel())
    , _assets(AppModel::getInstance().getMyAssets())
    , _settings (AppModel::getInstance().getSettings())
{
    connect(_wallet.get(), &WalletModel::normalCoinsChanged,  this, &AssetsViewModel::onNormalCoinsChanged);
    connect(_wallet.get(), &WalletModel::shieldedCoinChanged, this, &AssetsViewModel::onShieldedCoinChanged);
    _selectedAssets = _settings.getLastAssetSelection();
    emit selectedAssetChanged();
}

QAbstractItemModel* AssetsViewModel::getAssets()
{
    return _assets.get();
}

QVector<beam::Asset::ID> AssetsViewModel::getSelectedAssets()
{
    return _selectedAssets;
}

void AssetsViewModel::addAssetToSelected(int assetId)
{
    if (assetId < 0)
        return;

    auto id = _selectedAssets.indexOf(static_cast<beam::Asset::ID>(assetId));
    if(id == -1)
    {
        _selectedAssets.push_back(static_cast<beam::Asset::ID>(assetId));
    }
    else
    {
        _selectedAssets.remove(id);
    }
    _settings.setLastAssetSelection(_selectedAssets);
    emit selectedAssetChanged();
}

void AssetsViewModel::clearSelectedAssets()
{
    _selectedAssets.clear();
    _settings.setLastAssetSelection(_selectedAssets);
    emit selectedAssetChanged();
}

bool AssetsViewModel::getShowFaucetPromo()
{
    return _settings.showFaucetPromo() && !hasBeamAmount();
}

void AssetsViewModel::setShowFaucetPromo(bool value)
{
    _settings.setShowFacetPromo(value);
    emit showFaucetPromoChanged();
}

bool AssetsViewModel::getShowValidationPromo() const
{
    const bool hide = getCanHideValidationPromo() && _settings.hideSeedValidationPromo();
    return !hide;
}

void AssetsViewModel::setShowValidationPromo(bool value)
{
    _settings.setHideSeedValidationPromo(!value);
    emit showValidationPromoChanged();
}

bool AssetsViewModel::getCanHideValidationPromo() const
{
    auto availableL = beam::AmountBig::get_Lo(_wallet->getAvailable(beam::Asset::s_BeamID));
    return availableL < 1000000000;
}

void AssetsViewModel::onNormalCoinsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Coin>& utxos)
{
    emit showFaucetPromoChanged();
    emit showValidationPromoChanged();
    emit canHideValidationPromoChanged();
}

void AssetsViewModel::onShieldedCoinChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::ShieldedCoin>& items)
{
    emit showFaucetPromoChanged();
    emit showValidationPromoChanged();
    emit canHideValidationPromoChanged();
}

bool AssetsViewModel::hasBeamAmount() const
{
    return _wallet->getAvailable(beam::Asset::s_BeamID) != beam::Zero
        || _wallet->getAvailableRegular(beam::Asset::s_BeamID) != beam::Zero
        || _wallet->getAvailableShielded(beam::Asset::s_BeamID) != beam::Zero
        || _wallet->getMaturing(beam::Asset::s_BeamID) != beam::Zero
        || _wallet->getMatutingMP(beam::Asset::s_BeamID) != beam::Zero;
}
