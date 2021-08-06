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
    : _settings (AppModel::getInstance().getSettings())
{
    _selectedAsset = _settings.getLastAssetSelection();
    emit selectedAssetChanged();
}

QAbstractItemModel* AssetsViewModel::getAssets()
{
    return &_assets;
}

int AssetsViewModel::getSelectedAsset() const
{
    return _selectedAsset.is_initialized() ? static_cast<int>(*_selectedAsset) : -1;
}

void AssetsViewModel::setSelectedAsset(int assetId)
{
    auto newSelection = decltype(_selectedAsset)(boost::none);
    if (assetId >= 0)
    {
        newSelection = static_cast<beam::Asset::ID>(assetId);
    }

    if (_selectedAsset != newSelection)
    {
        _selectedAsset = newSelection;
        _settings.setLastAssetSelection(_selectedAsset);
        emit selectedAssetChanged();
    }
}

bool AssetsViewModel::getShowFaucetPromo()
{
    return _settings.showFaucetPromo() && !_assets.hasBeamAmount();
}

void AssetsViewModel::setShowFaucetPromo(bool value)
{
    _settings.setShowFacetPromo(value);
    emit showFaucetPromoChanged();
}
