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
#include "receive_view.h"
#include "ui_helpers.h"
#include "model/qr.h"
#include "model/app_model.h"

ReceiveViewModel::ReceiveViewModel()
    : _walletModel(*AppModel::getInstance().getWalletModel())
    , _amgr(AppModel::getInstance().getAssets())
{
    connect(&_walletModel,  &WalletModel::newAddressFailed,    this,  &ReceiveViewModel::newAddressFailed);
    connect(_amgr.get(),    &AssetsManager::assetsListChanged, this,  &ReceiveViewModel::assetsListChanged);
}

void ReceiveViewModel::initialize(const QString& address)
{
    beam::wallet::WalletID walletID;
    if (address.isEmpty() || !walletID.FromHex(address.toStdString()))
    {
        _walletModel.getAsync()->generateNewAddress([this](const auto& addr){
            _receiverAddress = addr;
            setComment(QString::fromStdString(addr.m_label));
            updateToken();
        });
    }
    else
    {
        _walletModel.getAsync()->getAddress(walletID, [this](const boost::optional<beam::wallet::WalletAddress>& address, size_t offlineCount) {
            _receiverAddress = *address;
            setComment(QString::fromStdString(address->m_label));
            updateToken();
        });
    }
}

void ReceiveViewModel::updateToken()
{
    using namespace beam::wallet;
    if (_maxp)
    {
        _walletModel.getAsync()->generateVouchers(_receiverAddress.m_OwnID, 1, [this](const ShieldedVoucherList& v)
        {
            if (!v.empty())
            {
                auto address = GenerateMaxPrivacyToken(_receiverAddress, _amount, _assetId, v[0], AppModel::getMyVersion());
                setToken(QString::fromStdString(address));
            }
        });
    }
    else
    {
        _walletModel.getAsync()->generateVouchers(_receiverAddress.m_OwnID, 1, [this](const ShieldedVoucherList& v)
        {
            if (!v.empty())
            {
                auto address = GenerateChoiceToken(_receiverAddress, _amount, _assetId, v[0], AppModel::getMyVersion());
                setToken(QString::fromStdString(address));
            }
        });
    }
}

QString ReceiveViewModel::getAmount() const
{
    return beamui::AmountToUIString(_amount);
}

void ReceiveViewModel::setAmount(const QString& value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amount)
    {
        _amount = amount;
        emit amountChanged();
        updateToken();
    }
}

QString ReceiveViewModel::getComment() const
{
    return _comment;
}

void ReceiveViewModel::setToken(const QString& value)
{
    if (_token != value)
    {
        _token = value;
        emit tokenChanged();
    }
}

QString ReceiveViewModel::getToken() const
{
    return _token;
}

bool ReceiveViewModel::getCommentValid() const
{
    return _walletModel.isOwnAddress(_receiverAddress.m_walletID) || !_walletModel.isAddressWithCommentExist(_comment.toStdString());
}

void ReceiveViewModel::setComment(const QString& value)
{
    auto trimmed = value.trimmed();
    if (_comment != trimmed)
    {
        _comment = trimmed;
        emit commentChanged();
        emit commentValidChanged();
    }
}

void ReceiveViewModel::saveAddress()
{
    using namespace beam::wallet;

    if (getCommentValid())
    {
        _receiverAddress.m_label = _comment.toStdString();
        _receiverAddress.setExpiration(WalletAddress::ExpirationStatus::Auto);
        _walletModel.getAsync()->saveAddress(_receiverAddress, true);
    }
}

bool ReceiveViewModel::getIsMaxPrivacy() const
{
    return _maxp;
}

void ReceiveViewModel::setIsMaxPrivacy(bool value)
{
    if (_maxp != value)
    {
        _maxp = value;
        emit isMaxPrivacyChanged();
        updateToken();
    }
}

QString ReceiveViewModel::getMPTimeLimit() const
{
    const auto& settings = AppModel::getInstance().getSettings();
    auto mpLockTimeLimit = settings.getMaxPrivacyLockTimeLimitHours();
    return QString::number(mpLockTimeLimit);
}

QList<QMap<QString, QVariant>> ReceiveViewModel::getAssetsList() const
{
    return _amgr->getAssetsList();
}

void ReceiveViewModel::setAssetId(int value)
{
    auto valueId = value < 0 ? beam::Asset::s_BeamID : static_cast<beam::Asset::ID>(value);
    if (_assetId != valueId)
    {
        LOG_INFO () << "Selected asset id" << value;
        _assetId = valueId;
        emit assetIdChanged();
        updateToken();
    }
}

int ReceiveViewModel::getAssetId() const
{
    return _assetId;
}
