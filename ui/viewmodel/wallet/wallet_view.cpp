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
{
    connect(&_model, SIGNAL(availableChanged()), this, SIGNAL(beamAvailableChanged()));
    connect(&_model, SIGNAL(receivingChanged()), this, SIGNAL(beamReceivingChanged()));
    connect(&_model, SIGNAL(sendingChanged()), this, SIGNAL(beamSendingChanged()));
    connect(&_model, SIGNAL(maturingChanged()), this, SIGNAL(beamLockedChanged()));
    connect(&_model, SIGNAL(receivingChangeChanged()), this, SIGNAL(beamReceivingChanged()));
    connect(&_model, SIGNAL(receivingIncomingChanged()), this, SIGNAL(beamReceivingChanged()));
    connect(&_exchangeRatesManager1, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyLabelChanged()));
    connect(&_exchangeRatesManager1, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
}

QString WalletViewModel::beamAvailable() const
{
    return beamui::AmountToUIString(_model.getAvailable());
}

QString WalletViewModel::beamReceiving() const
{
    return beamui::AmountToUIString(_model.getReceivingChange() + _model.getReceivingIncoming());
}

QString WalletViewModel::beamSending() const
{
    return beamui::AmountToUIString(_model.getSending());
}

QString WalletViewModel::beamReceivingChange() const
{
     return beamui::AmountToUIString(_model.getReceivingChange());
}

QString WalletViewModel::beamReceivingIncoming() const
{
    return beamui::AmountToUIString(_model.getReceivingIncoming());
}

QString WalletViewModel::beamLocked() const
{
    return beamui::AmountToUIString(_model.getMaturing() + _model.getMaturingMP());
}

QString WalletViewModel::beamLockedMaturing() const
{
    return beamui::AmountToUIString(_model.getMaturing());
}

QString WalletViewModel::beamLockedMaturingMP() const
{
    return beamui::AmountToUIString(_model.getMaturingMP());;
}

QString WalletViewModel::getSecondCurrencyLabel() const
{
    return beamui::getCurrencyLabel(_exchangeRatesManager1.getRateUnitRaw());
}

QString WalletViewModel::getSecondCurrencyRateValue() const
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
