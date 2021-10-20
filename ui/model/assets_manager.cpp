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
#include "viewmodel/ui_helpers.h"

namespace
{
    const unsigned char ACAlpha = 252;
}

AssetsManager::AssetsManager(WalletModel::Ptr wallet, ExchangeRatesManager::Ptr rates)
    : _wallet(std::move(wallet))
    , _rates(std::move(rates))
{
    connect(_wallet.get(), &WalletModel::assetInfoChanged, this, &AssetsManager::onAssetInfo);
    connect(_rates.get(),  &ExchangeRatesManager::rateUnitChanged,   this,  &AssetsManager::assetsListChanged);
    connect(_rates.get(),  &ExchangeRatesManager::activeRateChanged, this,  &AssetsManager::assetsListChanged);
    connect(_wallet.get(), &WalletModel::verificationInfoUpdate, this, &AssetsManager::onAssetVerification);
    _wallet->getAsync()->getVerificationInfo();

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

    _colors[0] = QColor("#72fdff"); _colors[0].setAlpha(ACAlpha);
    _colors[1] = QColor("#2acf1d"); _colors[1].setAlpha(ACAlpha);
    _colors[2] = QColor("#ffbb54"); _colors[2].setAlpha(ACAlpha);
    _colors[3] = QColor("#d885ff"); _colors[3].setAlpha(ACAlpha);
    _colors[4] = QColor("#008eff"); _colors[4].setAlpha(ACAlpha);
    _colors[5] = QColor("#ff746b"); _colors[5].setAlpha(ACAlpha);
    _colors[6] = QColor("#91e300"); _colors[6].setAlpha(ACAlpha);
    _colors[7] = QColor("#ffe75a"); _colors[7].setAlpha(ACAlpha);
    _colors[8] = QColor("#9643ff"); _colors[8].setAlpha(ACAlpha);
    _colors[9] = QColor("#395bff"); _colors[9].setAlpha(ACAlpha);
    _colors[10] = QColor("#ff3b3b"); _colors[10].setAlpha(ACAlpha);
    _colors[11] = QColor("#73ff7c"); _colors[11].setAlpha(ACAlpha);
    _colors[12] = QColor("#ffa86c"); _colors[12].setAlpha(ACAlpha);
    _colors[13] = QColor("#ff3abe"); _colors[13].setAlpha(ACAlpha);
    _colors[14] = QColor("#00aee1"); _colors[14].setAlpha(ACAlpha);
    _colors[15] = QColor("#ff5200"); _colors[15].setAlpha(ACAlpha);
    _colors[16] = QColor("#6464ff"); _colors[16].setAlpha(ACAlpha);
    _colors[17] = QColor("#ff7a21"); _colors[17].setAlpha(ACAlpha);
    _colors[18] = QColor("#63afff"); _colors[18].setAlpha(ACAlpha);
    _colors[19] = QColor("#c81f68"); _colors[19].setAlpha(ACAlpha);
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

    emit assetsListChanged();
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

    if (auto it = m_vi.find(id); it != m_vi.end())
    {
        if(!it->second.m_icon.empty())
        {
            const auto iconCheck = QString(":/assets/") + QString::fromStdString(it->second.m_icon);
            if(QFile::exists(iconCheck))
            {
                auto iconRet = QString("qrc:/assets/") + QString::fromStdString(it->second.m_icon);
                return iconRet;
            }
        }
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
        return QColor( 0, 246, 210, ACAlpha);
    }

    if (auto it = m_vi.find(id); it != m_vi.end())
    {
        if(!it->second.m_color.empty())
        {
            auto color = QColor(it->second.m_color.c_str());
            color.setAlpha(ACAlpha);
            return color;
        }
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

beam::Amount AssetsManager::getRate(beam::Asset::ID assetId)
{
    beam::wallet::Currency assetCurr(assetId);
    return _rates->getRate(assetCurr);
}

QString AssetsManager::getRateUnit()
{
    return beamui::getCurrencyUnitName(_rates->getRateCurrency());
}

QMap<QString, QVariant> AssetsManager::getAssetProps(beam::Asset::ID assetId)
{
    QMap<QString, QVariant> asset;

    beam::wallet::Currency assetCurr(assetId);
    const bool isBEAM = assetId == beam::Asset::s_BeamID;

    asset.insert("isBEAM",     isBEAM);
    asset.insert("unitName",   getUnitName(assetId, AssetsManager::NoShorten));
    asset.insert("rate",       beamui::AmountToUIString(_rates->getRate(assetCurr)));
    asset.insert("rateUnit",   getRateUnit());
    asset.insert("assetId",    static_cast<int>(assetId));
    asset.insert("icon",       getIcon(assetId));
    asset.insert("iconWidth",  22);
    asset.insert("iconHeight", 22);
    asset.insert("decimals",   static_cast<uint8_t>(std::log10(beam::Rules::Coin)));
    asset.insert("verified",   isVerified(assetId));

    return asset;
}

QList<QMap<QString, QVariant>> AssetsManager::getAssetsList()
{
    const auto assets = _wallet->getAssetsNZ();
    QList<QMap<QString, QVariant>> result;

    for(auto assetId: assets)
    {
        result.push_back(getAssetProps(assetId));
    }

    return result;
}

QMap<QString, QVariant> AssetsManager::getAssetsMap(const std::set<beam::Asset::ID>& assets)
{
    QMap<QString, QVariant> result;

    for(auto assetId: assets)
    {
        result.insert(QString::number(assetId), getAssetProps(assetId));
    }

    return result;
}

void AssetsManager::onAssetVerification(const std::vector<beam::wallet::VerificationInfo>& changed)
{
    for (const auto& info: changed)
    {
        m_vi[info.m_assetID] = info;
        emit assetInfo(info.m_assetID);
    }
    emit assetsListChanged();
}

bool AssetsManager::isVerified(beam::Asset::ID assetId) const
{
    if (auto it = m_vi.find(assetId); it != m_vi.end())
    {
        return it->second.m_verified;
    }
    return false;
}
