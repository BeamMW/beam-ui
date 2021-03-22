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
#include "qml_globals.h"

ReceiveViewModel::ReceiveViewModel()
    : _walletModel(*AppModel::getInstance().getWalletModel())
    , _amgr(AppModel::getInstance().getAssets())
{
    connect(&_walletModel,  &WalletModel::newAddressFailed,    this,  &ReceiveViewModel::newAddressFailed);
    connect(_amgr.get(),    &AssetsManager::assetsListChanged, this,  &ReceiveViewModel::assetsListChanged);
    updateToken();
}

void ReceiveViewModel::updateToken()
{
    using namespace beam::wallet;
    auto generateToken = [this] () {
        if (_maxp)
        {
            _walletModel.getAsync()->generateVouchers(_receiverAddress.m_OwnID, 1, [this](const ShieldedVoucherList& v)
            {
                if (!v.empty())
                {
                    _receiverAddress.m_Address = GenerateMaxPrivacyToken(_receiverAddress, _amount, _assetId, v[0], AppModel::getMyVersion());
                    emit tokenChanged();
                }
            });
        }
        else
        {
            _walletModel.getAsync()->generateVouchers(_receiverAddress.m_OwnID, 1, [this](const ShieldedVoucherList& v)
            {
                if (!v.empty())
                {
                    _receiverAddress.m_Address = GenerateOfflineToken(_receiverAddress, _amount, _assetId, v, AppModel::getMyVersion());
                    emit tokenChanged();
                }
            });
        }
    };

    if (_receiverAddress.m_walletID == Zero)
    {
         _walletModel.getAsync()->generateNewAddress([generateToken, this](const auto& addr){
            _receiverAddress = addr;
            setComment(QString::fromStdString(addr.m_label));
            generateToken();
        });
    }
    else
    {
        generateToken();
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
    return QString::fromStdString(_receiverAddress.m_label);
}

void ReceiveViewModel::setToken(const QString& value)
{
    if (value.isEmpty())
    {
        return;
    }

    const char* error = "Unknown value passed to ReceiveViewModel::setToken";
    beam::wallet::WalletID walletID;

    if (QMLGlobals::isAddress(value))
    {
        if (!walletID.FromHex(value.toStdString()))
        {
            throw std::runtime_error(error);
        }
    }
    else if(QMLGlobals::isTransactionToken(value))
    {
        auto txParameters = beam::wallet::ParseParameters(value.toStdString());
        if (!txParameters)
        {
            throw std::runtime_error(error);
        }

        if (!txParameters->GetParameter(beam::wallet::TxParameterID::PeerID, walletID))
        {
            throw std::runtime_error(error);
        }
    }
    else
    {
        throw std::runtime_error(error);
    }

    _walletModel.getAsync()->getAddress(walletID, [this, error](const boost::optional<beam::wallet::WalletAddress>& address, size_t offlineCount) {
        if (!address)
        {
            throw std::runtime_error(error);
        }
        _receiverAddress = *address;
        setComment(QString::fromStdString(address->m_label));
        updateToken();
    });
}

QString ReceiveViewModel::getToken() const
{
    return QString::fromStdString(_receiverAddress.m_Address);
}

bool ReceiveViewModel::getCommentValid() const
{
    return _walletModel.isOwnAddress(_receiverAddress.m_walletID) || !_walletModel.isAddressWithCommentExist(_receiverAddress.m_label);
}

void ReceiveViewModel::setComment(const QString& value)
{
    auto trimmed = value.trimmed().toStdString();
    if (_receiverAddress.m_label != trimmed)
    {
        _receiverAddress.m_label = trimmed;
        emit commentChanged();
        emit commentValidChanged();
    }
}

void ReceiveViewModel::saveAddress()
{
    using namespace beam::wallet;

    if (getCommentValid())
    {
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
