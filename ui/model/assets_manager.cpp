// Copyright 2020-2024 The Beam Team
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

#ifdef BEAM_ASSET_SWAP_SUPPORT
#include "model/app_model.h"
#endif  // BEAM_ASSET_SWAP_SUPPORT

namespace
{
    static constexpr uint8_t ACAlpha = 252;

    beam::Asset::ID GetBeamXID()
    {
        using namespace beam;
        switch (Rules::get().m_Network)
        {
        case Rules::Network::mainnet:
            return 7;
        case Rules::Network::testnet:
            return 12;
        default:
            ;
        }
        return 31;
    }
}

AssetsManager::AssetsManager(WalletModel::Ptr wallet, ExchangeRatesManager::Ptr rates)
    : _wallet(std::move(wallet))
    , _rates(std::move(rates))
{
    connect(_wallet, &WalletModel::assetInfoChanged, this, &AssetsManager::onAssetInfo);
    connect(_rates.get(),  &ExchangeRatesManager::rateUnitChanged,   this,  &AssetsManager::assetsListChanged);
    connect(_rates.get(),  &ExchangeRatesManager::activeRateChanged, this,  &AssetsManager::assetsListChanged);
    connect(_wallet, &WalletModel::verificationInfoUpdate, this, &AssetsManager::onAssetVerification);
    _wallet->getAsync()->getVerificationInfo();
    
    static const auto predefined_color_strings = 
    { 
        "#72fdff", "#2acf1d", "#ffbb54", "#d885ff", "#008eff", "#ff746b", "#91e300", "#ffe75a", "#9643ff", "#395bff",
        "#ff3b3b", "#73ff7c", "#ffa86c", "#ff3abe", "#00aee1", "#ff5200", "#6464ff", "#ff7a21", "#63afff", "#c81f68"
    };

    uint32_t i = 0;
    for (auto it= predefined_color_strings.begin(); it != predefined_color_strings.end(); ++it, ++i)
    {
        _icons[i] = QString::fromStdString(std::format("qrc:/assets/asset-{}.svg", i));
        
        _colors[i] = QColor(*it);
        _colors[i].setAlpha(ACAlpha);
    }
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
            if(const auto iconCheck = QString(":/assets/") + QString::fromStdString(it->second.m_icon); QFile::exists(iconCheck))
            {
                return QString("qrc") + iconCheck;
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
        unitName = QString::fromStdString(meta->GetUnitName());
    }

    if (unitName.isEmpty())
    {
        unitName = "ASSET";
    }

    static constexpr auto kMaxUnitLen = 6;
    if (shorten != NoShorten && unitName.length() > kMaxUnitLen)
    {
        unitName = unitName.left(kMaxUnitLen) + ((shorten == ShortenHtml)? "&#2026;": "\u2026");
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
        name = QString::fromStdString(meta->GetName());
    }

    if (name.isEmpty())
    {
        name = QString::fromStdString(std::format("Asset {}", id));
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
        name = QString::fromStdString(meta->GetNthUnitName());
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
        desc = QString::fromStdString(meta->GetShortDesc());
    }

    if (desc.isEmpty() && id == GetBeamXID())
    {
        desc = "BeamX DAO governance token";
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
        desc = QString::fromStdString(meta->GetLongDesc());
    }

    if (desc.isEmpty() && id == GetBeamXID())
    {
        desc =  "BEAMX token is a Confidential Asset issued on top of the Beam blockchain with a fixed emission of 100,000,000 units "
                "(except for the lender of a \"last resort\" scenario). BEAMX is the governance token for the BeamX DAO, managed by "
                "the BeamX DAO Core contract. Holders can earn BeamX tokens by participating in the DAO activities: providing "
                "liquidity to the DeFi applications governed by the DAO or participating in the governance process.";
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
        desc = QString::fromStdString(meta->GetSiteUrl());
    }

    if (desc.isEmpty() && id == GetBeamXID())
    {
        desc = "https://www.beamxdao.org/";
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
        desc = QString::fromStdString(meta->GetPaperUrl());
    }

    if (desc.isEmpty() && id == GetBeamXID())
    {
        desc = "https://beam.mw/docs/ecosystem/overview/beamx-tokenomics";
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
    auto unitName = getUnitName(assetId, AssetsManager::NoShorten);
    asset.insert("isBEAM",     isBEAM);
    asset.insert("unitName",   unitName);
    asset.insert("unitNameWithId", isBEAM ? unitName : QString("%1 <font color='#8da1ad'>(%2)</font>").arg(unitName).arg(assetId));
    asset.insert("rate",       beamui::AmountToUIString(_rates->getRate(assetCurr)));
    asset.insert("rateUnit",   getRateUnit());
    asset.insert("assetId",    static_cast<int>(assetId));
    asset.insert("icon",       getIcon(assetId));
    asset.insert("iconWidth",  22);
    asset.insert("iconHeight", 22);
    asset.insert("decimals",   static_cast<uint8_t>(std::log10(beam::Rules::Coin)));
    asset.insert("verified",   isVerified(assetId));

#ifdef BEAM_ASSET_SWAP_SUPPORT
    asset.insert("allowed", _allowedAssets.contains(assetId));
    if (assetId)
    {
        const auto it = _info.find(assetId);
        asset.insert("emission", 
                     beamui::AmountBigToUIString(it != _info.end() ? _info[assetId].first->m_Value : beam::Zero));
    }
#endif  // BEAM_ASSET_SWAP_SUPPORT

    return asset;
}

QList<QMap<QString, QVariant>> AssetsManager::getAssetsListFull()
{
    const auto assets = _wallet->getAssetsFull();

#ifdef BEAM_ASSET_SWAP_SUPPORT
    auto& settings = AppModel::getInstance().getSettings();
    _allowedAssets = settings.getAllowedAssets();
#endif  // BEAM_ASSET_SWAP_SUPPORT

    QList<QMap<QString, QVariant>> result;

    for(auto assetId: assets)
    {
        result.push_back(getAssetProps(assetId));
    }

    return result;
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

bool AssetsManager::isKnownAsset(beam::Asset::ID assetId) const
{
    const auto assets = _wallet->getAssetsFull();
    return assets.find(assetId) != assets.end();
}
