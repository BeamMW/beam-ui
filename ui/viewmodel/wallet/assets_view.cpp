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

namespace
{
    typedef std::vector<std::shared_ptr<AssetObject>> VAssets;
}

AssetsViewModel::AssetsViewModel()
    : _wallet (*AppModel2::getInstance().getWalletModel())
{
    connect(&_wallet, &WalletModel::walletStatusChanged, this, &AssetsViewModel::onWalletStatus);

    auto assetBEAM  = std::make_shared<AssetObject>(0);

    VAssets all = {assetBEAM};
    _assets.reset(all);

    formAssetsList();
    emit assetsChanged();
}

QAbstractItemModel* AssetsViewModel::getAssets()
{
    return &_assets;
}

void AssetsViewModel::formAssetsList()
{
    VAssets all;

    const auto assets = _wallet.getAssetsNZ();
    for (auto assetId: assets)
    {
        /* bool found = false;
        for (const auto& asset: _assets)
        {
            if((found = asset->id() == assetId)) {
                break;
            }
        }

        if(!found)
        {
            _assets.insert(std::make_shared<AssetObject>(assetId));
        }
        */

        all.insert(all.begin(),std::make_shared<AssetObject>(assetId));
    }

    _assets.reset(all);
}

void AssetsViewModel::onWalletStatus()
{
    formAssetsList();
    emit assetsChanged();
}
