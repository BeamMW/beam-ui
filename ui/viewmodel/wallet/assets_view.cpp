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

AssetsViewModel::AssetsViewModel()
{
    auto asset0 = std::make_shared<AssetObject>(0, "CA0", 0);
    auto asset1 = std::make_shared<AssetObject>(1, "CA1", 11);
    auto asset2 = std::make_shared<AssetObject>(2, "CA2", 22);
    auto asset3 = std::make_shared<AssetObject>(3, "CA3", 32);
    auto asset4 = std::make_shared<AssetObject>(4, "CA3", 42);
    auto asset5 = std::make_shared<AssetObject>(5, "CA3", 52);

    _assets.insert(asset0);
    _assets.insert(asset1);
    _assets.insert(asset2);
    _assets.insert(asset3);
    _assets.insert(asset4);
    _assets.insert(asset5);

    emit assetsChanged();
}

QAbstractItemModel* AssetsViewModel::getAssets()
{
    return &_assets;
}
