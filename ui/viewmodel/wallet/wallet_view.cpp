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
#include "wallet_view.h"
#include "model/app_model.h"

namespace {
    inline beam::Asset::ID assetIdxToId(int idx) {
        return idx < 1 ? beam::Asset::s_BeamID : beam::Asset::ID(idx);
    }
}

WalletViewModel::WalletViewModel()
    : _model(*AppModel::getInstance().getWallet())
    , _settings(AppModel::getInstance().getSettings())
    , _selectedAssetID(-1)
{
    connect(&_model, &WalletModel::walletStatusChanged, this, &WalletViewModel::beamChanged);
    connect(&_model, &WalletModel::walletStatusChanged, this, &WalletViewModel::assetChanged);
    connect(&_amgr, &AssetsManager::assetInfo, this, &WalletViewModel::onAssetInfo);

    connect(&_ermgr, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyUnitNameChanged()));
    connect(&_ermgr, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
}

QString WalletViewModel::getSecondCurrencyUnitName() const
{
    return beamui::getCurrencyUnitName(_ermgr.getRateUnitRaw());
}

QString WalletViewModel::getSecondCurrencyRate() const
{
    auto rate = _ermgr.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return beamui::AmountToUIString(rate);
}

bool WalletViewModel::isAllowedBeamMWLinks() const
{
    return _settings.isAllowedBeamMWLinks();
}

void WalletViewModel::allowBeamMWLinks(bool value)
{
    _settings.setAllowedBeamMWLinks(value);
}

int WalletViewModel::getSelectedAsset() const
{
    return _selectedAssetID;
}

void WalletViewModel::setSelectedAsset(int newAsset)
{
    if (_selectedAssetID != newAsset)
    {
        _selectedAssetID = newAsset;
        _amgr.collectAssetInfo(assetIdxToId(_selectedAssetID));
        emit assetChanged();
    }
}

void WalletViewModel::onAssetInfo(beam::Asset::ID assetId)
{
    if (assetId == beam::Asset::ID(_selectedAssetID))
    {
        emit assetChanged();
    }
}

QString WalletViewModel::assetAvailable() const
{
    return beamui::AmountToUIString(_model.getAvailable(assetIdxToId(_selectedAssetID)));
}

QString WalletViewModel::assetReceiving() const
{
    const auto id = assetIdxToId(_selectedAssetID);
    return beamui::AmountToUIString(_model.getReceivingChange(id) + _model.getReceivingIncoming(id));
}

QString WalletViewModel::assetSending() const
{
    return beamui::AmountToUIString(_model.getSending(assetIdxToId(_selectedAssetID)));
}

QString WalletViewModel::assetReceivingChange() const
{
     return beamui::AmountToUIString(_model.getReceivingChange(assetIdxToId(_selectedAssetID)));
}

QString WalletViewModel::assetReceivingIncoming() const
{
    return beamui::AmountToUIString(_model.getReceivingIncoming(assetIdxToId(_selectedAssetID)));
}

QString WalletViewModel::assetLocked() const
{
    return assetLockedMaturing();
}

QString WalletViewModel::assetLockedMaturing() const
{
    return beamui::AmountToUIString(_model.getMaturing(assetIdxToId(_selectedAssetID)));
}

QString WalletViewModel::assetIcon()
{
    return _amgr.getIcon(assetIdxToId(_selectedAssetID));
}

QString WalletViewModel::assetUnitName()
{
    return _amgr.getUnitName(assetIdxToId(_selectedAssetID));
}

QString WalletViewModel::assetName()
{
    return _amgr.getName(assetIdxToId(_selectedAssetID));
}
