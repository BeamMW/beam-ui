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
    auto asset0  = std::make_shared<AssetObject>(0, "CA0", 0);
    auto asset1  = std::make_shared<AssetObject>(1, "CA1", 11);
    auto asset2  = std::make_shared<AssetObject>(2, "CA2", 22);
    auto asset3  = std::make_shared<AssetObject>(3, "CA3", 32);
    auto asset4  = std::make_shared<AssetObject>(4, "CA4", 42);
    auto asset5  = std::make_shared<AssetObject>(5, "CA5", 52);
    auto asset6  = std::make_shared<AssetObject>(6, "CA6", 62);
    auto asset7  = std::make_shared<AssetObject>(7, "CA7", 72);
    auto asset8  = std::make_shared<AssetObject>(8, "CA8", 82);
    auto asset9  = std::make_shared<AssetObject>(9, "CA9", 92);
    auto asset10 = std::make_shared<AssetObject>(10, "CA10", 102);
    auto asset11 = std::make_shared<AssetObject>(11, "CA11", 112);

    _assets.insert(asset0);
    _assets.insert(asset1);
    _assets.insert(asset2);
    _assets.insert(asset3);
    _assets.insert(asset4);
    _assets.insert(asset5);
    _assets.insert(asset5);
    _assets.insert(asset6);
    _assets.insert(asset7);
    _assets.insert(asset8);
    _assets.insert(asset9);
    _assets.insert(asset10);
    _assets.insert(asset11);

    emit assetsChanged();
}

QAbstractItemModel* AssetsViewModel::getAssets()
{
    return &_assets;
}

