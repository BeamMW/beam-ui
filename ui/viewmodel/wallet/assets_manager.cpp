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

AssetsManager::AssetsManager (WalletModel::Ptr wallet)
    : _wallet(wallet)
{
    qRegisterMetaType<beam::Asset::ID>("beam::wallet::AssetID");
    connect(_wallet.get(), &WalletModel::assetInfoChanged, this, &AssetsManager::onAssetInfo);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::rateUnitChanged,   this,  &AssetsManager::assetsListChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged, this,  &AssetsManager::assetsListChanged);

    _icons[0]  = "qrc:/assets/asset-0.svg";
    _icons[1]  = "qrc:/assets/asset-1.svg";
    _icons[2]  = "qrc:/assets/asset-2.svg";
    _icons[3]  = "qrc:/assets/asset-3.svg";
    _icons[4]  = "qrc:/assets/asset-4.svg";
    _icons[5]  = "qrc:/assets/asset-5.svg";
    _icons[6]  = "qrc:/assets/asset-6.svg";
    _icons[7]  = "qrc:/assets/asset-7.svg";
    _icons[8]  = "qrc:/assets/asset-8.svg";
    _icons[9]  = "qrc:/assets/asset-9.svg";
    _icons[10]  = "qrc:/assets/asset-10.svg";
    _icons[11]  = "qrc:/assets/asset-11.svg";
    _icons[12]  = "qrc:/assets/asset-12.svg";
    _icons[13]  = "qrc:/assets/asset-13.svg";
    _icons[14]  = "qrc:/assets/asset-14.svg";
    _icons[15]  = "qrc:/assets/asset-15.svg";
    _icons[16]  = "qrc:/assets/asset-16.svg";
    _icons[17]  = "qrc:/assets/asset-17.svg";
    _icons[18]  = "qrc:/assets/asset-18.svg";
    _icons[19]  = "qrc:/assets/asset-19.svg";

    const unsigned char alpha = 252;
    _colors[0] = QColor("#72fdff"); _colors[0].setAlpha(alpha);
    _colors[1] = QColor("#2acf1d"); _colors[1].setAlpha(alpha);
    _colors[2] = QColor("#ffbb54"); _colors[2].setAlpha(alpha);
    _colors[3] = QColor("#d885ff"); _colors[3].setAlpha(alpha);
    _colors[4] = QColor("#008eff"); _colors[4].setAlpha(alpha);
    _colors[5] = QColor("#ff746b"); _colors[5].setAlpha(alpha);
    _colors[6] = QColor("#91e300"); _colors[6].setAlpha(alpha);
    _colors[7] = QColor("#ffe75a"); _colors[7].setAlpha(alpha);
    _colors[8] = QColor("#9643ff"); _colors[8].setAlpha(alpha);
    _colors[9] = QColor("#395bff"); _colors[9].setAlpha(alpha);
    _colors[10] = QColor("#ff3b3b"); _colors[10].setAlpha(alpha);
    _colors[11] = QColor("#73ff7c"); _colors[11].setAlpha(alpha);
    _colors[12] = QColor("#ffa86c"); _colors[12].setAlpha(alpha);
    _colors[13] = QColor("#ff3abe"); _colors[13].setAlpha(alpha);
    _colors[14] = QColor("#00aee1"); _colors[14].setAlpha(alpha);
    _colors[15] = QColor("#ff5200"); _colors[15].setAlpha(alpha);
    _colors[16] = QColor("#6464ff"); _colors[16].setAlpha(alpha);
    _colors[17] = QColor("#ff7a21"); _colors[17].setAlpha(alpha);
    _colors[18] = QColor("#63afff"); _colors[18].setAlpha(alpha);
    _colors[19] = QColor("#c81f68"); _colors[19].setAlpha(alpha);
}

void AssetsManager::collectAssetInfo(beam::Asset::ID assetId)
{
    if (assetId < 1)
    {
        emit assetInfo(assetId);
    }
    else
    {
        // don't request info multiple times
        if (_requested.find(assetId) == _requested.end())
        {
            _requested.insert(assetId);
            _wallet->getAsync()->getAssetInfo(assetId);
        }
    }
}

void AssetsManager::onAssetInfo(beam::Asset::ID id, const beam::wallet::WalletAsset& asset)
{
    _requested.erase(id);

    if (asset.m_ID == beam::Asset::s_InvalidID)
    {
        // Bad info, erase any previously stored and if we had something stored notify about change
        const auto it = _info.find(id);
        if (it != _info.end())
        {
            _info.erase(it);
            emit assetInfo(id);
        }
    }
    else
    {
        // Good info came, save and notify about change
        AssetPtr aptr = std::make_shared<beam::wallet::WalletAsset>(asset);
        _info[id] = std::make_pair(aptr, MetaPtr());

        emit assetInfo(id);
    }
}

AssetsManager::MetaPtr AssetsManager::getAsset(beam::Asset::ID id)
 {
    const auto it = _info.find(id);
    if (it != _info.end())
    {
        if (it->second.second == nullptr)
        {
            MetaPtr mptr = std::make_shared<beam::wallet::WalletAssetMeta>(*it->second.first);
            it->second.second = mptr;
        }
        return it->second.second;
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

QString AssetsManager::getUnitName(beam::Asset::ID id, Shorten shorten)
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
        unitName = "ASSET";
    }

    const int kMaxUnitLen = 6;
    if (shorten != NoShorten && unitName.length() > kMaxUnitLen)
    {
        if (shorten == ShortenHtml)
        {
            unitName = unitName.left(kMaxUnitLen) + "&#2026;";
        }
        else
        {
            unitName = unitName.left(kMaxUnitLen) + u8"\u2026";
        }
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

QString AssetsManager::getSmallestUnitName(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "GROTH";
    }

    QString name;
    if (auto meta = getAsset(id))
    {
        name = meta->GetNthUnitName().c_str();
    }

    if (name.isEmpty())
    {
        name = "AGROTH";
    }

    return name;
}

QString AssetsManager::getShortDesc(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "";
    }

    QString desc;
    if (auto meta = getAsset(id))
    {
        desc = meta->GetShortDesc().c_str();
    }
    return desc;
}

QString AssetsManager::getLongDesc(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "";
    }

    QString desc;
    if (auto meta = getAsset(id))
    {
        desc = meta->GetLongDesc().c_str();
    }
    return desc;
}

QString AssetsManager::getSiteUrl(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "";
    }

    QString desc;
    if (auto meta = getAsset(id))
    {
        desc = meta->GetSiteUrl().c_str();
    }
    return desc;
}

QString AssetsManager::getPaperUrl(beam::Asset::ID id)
{
    if (id < 1)
    {
        return "";
    }

    QString desc;
    if (auto meta = getAsset(id))
    {
        desc = meta->GetPaperUrl().c_str();
    }

    return desc;
}

bool AssetsManager::hasAsset(beam::Asset::ID assetId) const
{
    const auto assets = _wallet->getAssetsNZ();
    return assets.find(assetId) != assets.end();
}

QColor AssetsManager::getColor(beam::Asset::ID id)
{
    if (id < 1)
    {
        return QColor( 0, 246, 210, 252);
    }

    if (auto meta = getAsset(id))
    {
        auto color = meta->GetColor();
        if (!color.empty())
        {
            return QString::fromStdString(color);
        }

        auto idx = static_cast<int>(id % _colors.size());
        return _colors[idx];
    }

    auto errColor = QColor("#8192a3"); errColor.setAlpha(252);
    return errColor;
}

QColor AssetsManager::getSelectionColor(beam::Asset::ID id)
{
    return getColor(id);
}

QList<QMap<QString, QVariant>> AssetsManager::getAssetsList()
{
    const auto assets   = _wallet->getAssetsNZ();
    const auto beamRate = beamui::AmountToUIString(_exchangeRatesManager.getRate(beam::wallet::Currency::BEAM()));
    const auto rateUnit = beamui::getCurrencyUnitName(_exchangeRatesManager.getRateCurrency());
    QList<QMap<QString, QVariant>> result;

    for(auto assetId: assets)
    {
        QMap<QString, QVariant> asset;

        const bool isBeam = assetId == beam::Asset::s_BeamID;
        asset.insert("isBEAM",      isBeam);
        asset.insert("unitName",   getUnitName(assetId, AssetsManager::NoShorten));
        asset.insert("rate",       isBeam ? beamRate : "0");
        asset.insert("rateUnit",    rateUnit);
        asset.insert("assetId",     static_cast<int>(assetId));
        asset.insert("icon",       getIcon(assetId));
        asset.insert("iconWidth",  22);
        asset.insert("iconHeight", 22);

        result.push_back(asset);
    }

    return result;
}
