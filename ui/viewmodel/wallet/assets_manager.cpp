// Copyright 2020 The Beam Team
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
#include "assets_manager.h"
#include "model/app_model.h"

AssetsManager::AssetsManager ()
    : _wallet (*AppModel::getInstance().getWalletModel())
{
    qRegisterMetaType<beam::Asset::ID>("beam::wallet::AssetID");
    connect(&_wallet, &WalletModel::assetInfoChanged, this, &AssetsManager::onAssetInfo);

    _icons[0]  = "qrc:/assets/asset-0.svg";
    _icons[1]  = "qrc:/assets/asset-1.svg";
    _icons[2]  = "qrc:/assets/asset-2.svg";
    _icons[3]  = "qrc:/assets/asset-3.svg";

    _colors[0] = QColor(115, 255, 124, 252);
    _colors[1] = QColor(216, 133, 255, 252);
    _colors[2] = QColor(255, 231, 90, 252);
    _colors[3] = QColor(79, 165, 255, 252);
}

void AssetsManager::collectAssetInfo(beam::Asset::ID assetId)
{
    if (assetId < 1)
    {
        emit assetInfo(assetId);
    }
    else
    {
        _wallet.getAsync()->getAssetInfo(assetId);
    }
}

void AssetsManager::onAssetInfo(beam::Asset::ID id, const beam::wallet::WalletAsset& info)
{
    emit assetInfo(id);
    _info[id] = info;
}

AssetsManager::MetaPtr AssetsManager::getAsset(beam::Asset::ID id)
 {
    const auto it = _info.find(id);
    if (it != _info.end())
    {
        auto mptr = std::make_unique<beam::wallet::WalletAssetMeta>(it->second);
        return mptr;
    }

    collectAssetInfo(id);
    return MetaPtr();
 }

QString AssetsManager::getIcon(beam::Asset::ID id)
{
     if (id < 1)
     {
         return "qrc:/assets/icon-beam.svg";
     }

     const auto it = _info.find(id);
     if (it != _info.end())
     {
         auto idx = static_cast<int>(id % _icons.size());
         return _icons[idx];
     }

     return "qrc:/assets/asset-err.svg";
}

QString AssetsManager::getUnitName(beam::Asset::ID id, bool shorten)
{
    if (id < 1)
    {
        return "BEAM";
    }

    QString unitName;
    if (auto meta = getAsset(id))
    {
        unitName = meta->GetUnitName().c_str();
    }

    if (unitName.isEmpty())
    {
        std::ostringstream ss;
        ss << "ASSET" << static_cast<int>(id);
        unitName = ss.str().c_str();
        return unitName;
    }

    const int kMaxUnitLen = 6;
    if (shorten && unitName.length() > kMaxUnitLen)
    {
        unitName = unitName.left(kMaxUnitLen) + u8"\u2026";
    }

    return unitName;
}

QString AssetsManager::getName(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "Beam";
    }

    QString name;
    if (auto meta = getAsset(id))
    {
        name = meta->GetName().c_str();
    }

    if (name.isEmpty())
    {
        std::ostringstream ss;
        ss << "Asset " << static_cast<int>(id);
        name = QString(ss.str().c_str());
    }

    return name;
}

QColor AssetsManager::getColor(beam::Asset::ID id)
{
    if (id < 1)
    {
        return QColor( 0, 246, 210, 252);
    }

    const auto it = _info.find(id);
     if (it != _info.end())
     {
         auto idx = static_cast<int>(id % _colors.size());
         return _colors[idx];
     }

    return QColor( 255, 116, 107, 252);
}

QColor AssetsManager::getSelectionColor(beam::Asset::ID id)
{
    return getColor(id);
}
