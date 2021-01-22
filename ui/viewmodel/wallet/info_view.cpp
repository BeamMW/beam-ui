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
#include "info_view.h"
#include "model/app_model.h"

namespace {
    inline beam::Asset::ID assetIdxToId(int idx) {
        return idx < 1 ? beam::Asset::s_BeamID : beam::Asset::ID(idx);
    }
}

InfoViewModel::InfoViewModel()
    : _wallet(*AppModel::getInstance().getWalletModel())
    , _selectedAssetID(-1)
{
    connect(&_wallet,  &WalletModel::walletStatusChanged,        this,  &InfoViewModel::onWalletStatus);
    connect(&_amgr,    &AssetsManager::assetInfo,                this,  &InfoViewModel::onAssetInfo);
    connect(&_ermgr,   &ExchangeRatesManager::rateUnitChanged,   this,  &InfoViewModel::assetChanged);
    connect(&_ermgr,   &ExchangeRatesManager::activeRateChanged, this,  &InfoViewModel::assetChanged);

    updateProgress();
    _wallet.getAsync()->getWalletStatus();
}

InfoViewModel::~InfoViewModel()
{
}

QString InfoViewModel::assetAvailable() const
{
    const auto amount = _wallet.getAvailable(assetIdxToId(_selectedAssetID));
    return beamui::AmountBigToUIString(amount);
}

QString InfoViewModel::assetIcon() const
{
    return _amgr.getIcon(assetIdxToId(_selectedAssetID));
}

QString InfoViewModel::assetUnitName() const
{
    return _amgr.getUnitName(assetIdxToId(_selectedAssetID), false);
}

QString InfoViewModel::assetName() const
{
    return _amgr.getName(assetIdxToId(_selectedAssetID));
}

void InfoViewModel::onAssetInfo(beam::Asset::ID assetId)
{
    bool found = std::find_if(_progress.begin(), _progress.end(), [&assetId](const auto& inp) {
        return inp.assetId == assetId;
    }) != _progress.end();

    if (assetId == beam::Asset::ID(_selectedAssetID)  || found)
    {
        updateProgress();
        emit assetChanged();
    }
}

QString InfoViewModel::getRateUnit() const
{
    return _selectedAssetID < 1 ? beamui::getCurrencyUnitName(_ermgr.getRateUnitRaw()) : "";
}

QString InfoViewModel::getRate() const
{
    auto rate = _ermgr.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return _selectedAssetID < 1 ? beamui::AmountToUIString(rate) : "0";
}

int InfoViewModel::getSelectedAsset() const
{
    return _selectedAssetID;
}

void InfoViewModel::setSelectedAsset(int newAsset)
{
    if (_selectedAssetID != newAsset)
    {
        _selectedAssetID = newAsset;
        _amgr.collectAssetInfo(assetIdxToId(_selectedAssetID));
        updateProgress();
        emit assetChanged();
    }
}

void InfoViewModel::onWalletStatus()
{
    updateProgress();
    emit assetChanged();
}

QList<InProgress> InfoViewModel::getProgress() const
{
    return _progress;
}

InProgress InfoViewModel::getProgressTotal() const
{
    return _progressTotals;
}

void InfoViewModel::updateProgress()
{
    using namespace beam::wallet;

    _progress.clear();
    _progressTotals = InProgress();

    beam::AmountBig::Type sendingTotal    = Zero;
    beam::AmountBig::Type receivingTotal  = Zero;
    beam::AmountBig::Type changeTotal     = Zero;
    beam::AmountBig::Type incomingTotal   = Zero;
    beam::AmountBig::Type lockedTotal     = Zero;
    beam::AmountBig::Type maturingTotal   = Zero;
    beam::AmountBig::Type maturingMPTotal = Zero;
    QString receivingUnit;

    auto assets = _wallet.getAssetsNZ();
    for(auto& asset: assets)
    {
        InProgress progress;
        progress.assetId = asset;

        auto sending    = _wallet.getSending(asset);
        auto receiving  = _wallet.getReceiving(asset);
        auto change     = _wallet.getReceivingChange(asset);
        auto incoming   = _wallet.getReceivingIncoming(asset);
        auto maturing   = _wallet.getMaturing(asset);
        auto maturingMP = _wallet.getMatutingMP(asset);
        auto locked     = maturing; locked += maturingMP;

        if (sending != Zero || receiving != Zero || change != Zero || incoming != Zero || locked != Zero || maturing != Zero || maturingMP != Zero)
        {
            progress.sending           = beamui::AmountBigToUIString(sending);
            progress.receiving         = beamui::AmountBigToUIString(receiving);
            progress.receivingChange   = beamui::AmountBigToUIString(change);
            progress.receivingIncoming = beamui::AmountBigToUIString(incoming);
            progress.locked            = beamui::AmountBigToUIString(locked);
            progress.lockedMaturing    = beamui::AmountBigToUIString(maturing);
            progress.lockedMaturingMP  = beamui::AmountBigToUIString(maturingMP);
            progress.icon              = _amgr.getIcon(asset);
            progress.unitName          = _amgr.getUnitName(asset, false);

            if (asset == 0)
            {
                progress.rate =  beamui::AmountToUIString(_ermgr.getRate(ExchangeRate::Currency::Beam));
                progress.rateUnit = beamui::getCurrencyUnitName(_ermgr.getRateUnitRaw());
            }
            else
            {
                progress.rate = "0";
            }

            _progress.push_back(progress);
            sendingTotal    += sending;

            if (receiving != Zero)
            {
                receivingUnit = receivingTotal == Zero ? progress.unitName : "ASSETS";
            }

            receivingTotal  += receiving;
            changeTotal     += change;
            incomingTotal   += incoming;
            lockedTotal     += locked;
            maturingTotal   += maturing;
            maturingMPTotal += maturingMP;
        }
    }

    _progressTotals.sending           = beamui::AmountBigToUIString(sendingTotal);
    _progressTotals.receiving         = beamui::AmountBigToUIString(receivingTotal);
    _progressTotals.receivingChange   = beamui::AmountBigToUIString(changeTotal);
    _progressTotals.receivingIncoming = beamui::AmountBigToUIString(incomingTotal);
    _progressTotals.locked            = beamui::AmountBigToUIString(lockedTotal);
    _progressTotals.lockedMaturing    = beamui::AmountBigToUIString(maturingTotal);
    _progressTotals.lockedMaturingMP  = beamui::AmountBigToUIString(maturingMPTotal);
    _progressTotals.receivingUnit     = receivingUnit;
    _progressTotals.unitName          = _progress.length() == 1 ? _progress[0].unitName : "ASSETS";
    _progressTotals.rate              = _progress.length() == 1 ? _progress[0].rate : "0";
    _progressTotals.icon              = _progress.length() == 1 ? _progress[0].icon : "";
}
