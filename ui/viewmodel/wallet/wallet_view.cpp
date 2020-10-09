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

WalletViewModel::WalletViewModel()
    : _model(*AppModel::getInstance().getWallet())
    , _settings(AppModel::getInstance().getSettings())
    , _selectedAsset(-1)
{
    connect(&_model, &WalletModel::walletStatusChanged, this, &WalletViewModel::beamChanged);
    connect(&_model, &WalletModel::walletStatusChanged, this, &WalletViewModel::assetChanged);

    connect(&_exchangeRatesManager1, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyUnitNameChanged()));
    connect(&_exchangeRatesManager1, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
}

QString WalletViewModel::beamAvailable() const
{
    return beamui::AmountToUIString(_model.getAvailable(beam::Asset::s_BeamID));
}

QString WalletViewModel::beamReceiving() const
{
    const auto id = beam::Asset::s_BeamID;
    return beamui::AmountToUIString(_model.getReceivingChange(id) + _model.getReceivingIncoming(id));
}

QString WalletViewModel::beamSending() const
{
    return beamui::AmountToUIString(_model.getSending(beam::Asset::s_BeamID));
}

QString WalletViewModel::beamReceivingChange() const
{
     return beamui::AmountToUIString(_model.getReceivingChange(beam::Asset::s_BeamID));
}

QString WalletViewModel::beamReceivingIncoming() const
{
    return beamui::AmountToUIString(_model.getReceivingIncoming(beam::Asset::s_BeamID));
}

QString WalletViewModel::beamLocked() const
{
    return beamLockedMaturing();
}

QString WalletViewModel::beamLockedMaturing() const
{
    return beamui::AmountToUIString(_model.getMaturing(beam::Asset::s_BeamID));
}

QString WalletViewModel::getSecondCurrencyUnitName() const
{
    return beamui::getCurrencyUnitName(_exchangeRatesManager1.getRateUnitRaw());
}

QString WalletViewModel::getSecondCurrencyRate() const
{
    auto rate = _exchangeRatesManager1.getRate(beam::wallet::ExchangeRate::Currency::Beam);
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
    return _selectedAsset;
}

void WalletViewModel::setSelectedAsset(int val)
{
    _selectedAsset = beam::Asset::ID(val);
    emit assetChanged();
}

QString WalletViewModel::assetAvailable() const
{
    return beamui::AmountToUIString(_model.getAvailable(_selectedAsset));
}

QString WalletViewModel::assetReceiving() const
{
    const auto id = _selectedAsset;
    return beamui::AmountToUIString(_model.getReceivingChange(id) + _model.getReceivingIncoming(id));
}

QString WalletViewModel::assetSending() const
{
    return beamui::AmountToUIString(_model.getSending(_selectedAsset));
}

QString WalletViewModel::assetReceivingChange() const
{
     return beamui::AmountToUIString(_model.getReceivingChange(_selectedAsset));
}

QString WalletViewModel::assetReceivingIncoming() const
{
    return beamui::AmountToUIString(_model.getReceivingIncoming(_selectedAsset));
}

QString WalletViewModel::assetLocked() const
{
    return assetLockedMaturing();
}

QString WalletViewModel::assetLockedMaturing() const
{
    return beamui::AmountToUIString(_model.getMaturing(_selectedAsset));
}

QString WalletViewModel::beamIcon() const
{
    return "qrc:/assets/icon-beam.svg";
}

QString WalletViewModel::assetIcon() const
{
    return _selectedAsset == 0 ? beamIcon() : "qrc:/assets/asset-0.svg";
}

QString WalletViewModel::assetUnitName() const
{
    return _selectedAsset == 0 ? "BEAM" : "COIN1";
}

QString WalletViewModel::beamName() const
{
    return "Beam";
}

QString WalletViewModel::assetName() const
{
    return _selectedAsset == 0 ? beamName() : "Coin One";
}