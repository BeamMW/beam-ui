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
    using namespace beam::wallet;

    connect(&_walletModel,  &WalletModel::newAddressFailed,    this,  &ReceiveViewModel::newAddressFailed);
    connect(_amgr.get(),    &AssetsManager::assetsListChanged, this,  &ReceiveViewModel::assetsListChanged);
    updateToken();
}

void ReceiveViewModel::updateToken()
{
    using namespace beam::wallet;
    QPointer<ReceiveViewModel> guard = this;

    auto generateToken = [guard, this] () {
        if (!guard) return;
        assert(_receiverAddress.is_initialized());

        if (_maxp)
        {
            _walletModel.getAsync()->generateVouchers(_receiverAddress->m_OwnID, 1, [guard, this](const ShieldedVoucherList& v)
            {
                if (!guard) return;
                if (!v.empty())
                {
                    _receiverAddress->m_Address = GenerateMaxPrivacyToken(*_receiverAddress, _amount, _assetId, v[0], AppModel::getMyVersion());
                    emit tokenChanged();
                }
            });
        }
        else
        {
            _walletModel.getAsync()->generateVouchers(_receiverAddress->m_OwnID, 1, [guard, this](const ShieldedVoucherList& v)
            {
                if (!guard) return;
                if (!v.empty())
                {
                    _receiverAddress->m_Address = GenerateOfflineToken(*_receiverAddress, _amount, _assetId, v, AppModel::getMyVersion());
                    emit tokenChanged();
                }
            });
        }
    };

    if (!_receiverAddress)
    {
        _walletModel.getAsync()->generateNewAddress([guard, generateToken, this](const auto& addr){
            if (!guard) return;
            _receiverAddress = addr;
            _receiverAddress->setExpirationStatus(WalletAddress::ExpirationStatus::Auto);
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
    return QString::fromStdString(_receiverAddress ? _receiverAddress->m_label : std::string());
}

void ReceiveViewModel::setToken(const QString& token)
{
    using namespace beam::wallet;

    if (token.isEmpty())
    {
        return;
    }

    _originalToken = token;
    emit tokenChanged();

    QPointer<ReceiveViewModel> guard = this;
    _walletModel.getAsync()->getAddressByToken(token.toStdString(),
        [guard, this, token](const boost::optional<beam::wallet::WalletAddress>& address, size_t offlineCount) {
            if (!guard) return;

            if (!address)
            {
                throw std::runtime_error("Unknown value passed to ReceiveViewModel::setToken");
            }

            _receiverAddress = address;
            emit commentChanged();
            emit commentValidChanged();

            _maxp = GetAddressType(token.toStdString()) == TxAddressType::MaxPrivacy;
            emit isMaxPrivacyChanged();

            if (auto params = ParseParameters(token.toStdString()))
            {
                if (auto amount = params->GetParameter<beam::Amount>(TxParameterID::Amount))
                {
                    _amount = *amount;
                    emit amountChanged();
                }

                if (auto assetId = params->GetParameter<beam::Asset::ID>(TxParameterID::AssetID))
                {
                    _assetId = *assetId;
                    emit assetIdChanged();
                }
            }

            updateToken();
        }
    );
}

QString ReceiveViewModel::getToken() const
{
    return _receiverAddress ? QString::fromStdString(_receiverAddress->m_Address) : _originalToken;
}

QString ReceiveViewModel::getSbbsAddress() const
{
    return _receiverAddress ? beamui::toString(_receiverAddress->m_walletID) : _originalToken;
}

bool ReceiveViewModel::getCommentValid() const
{
    if (_receiverAddress)
    {
        return _walletModel.isOwnAddress(_receiverAddress->m_walletID) ||
               !_walletModel.isAddressWithCommentExist(_receiverAddress->m_label);
    }
    else
    {
        return true;
    }
}

void ReceiveViewModel::setComment(const QString& value)
{
    if (_receiverAddress)
    {
        auto trimmed = value.trimmed().toStdString();
        if (_receiverAddress->m_label != trimmed)
        {
            _receiverAddress->m_label = trimmed;
            emit commentChanged();
            emit commentValidChanged();
        }
    }
}

void ReceiveViewModel::saveAddress()
{
    // If you open receive view from address book and
    // hit copy and close fast it might happen that
    // address still not received from another thread
    if (_receiverAddress)
    {
        if (getCommentValid())
        {
            _walletModel.getAsync()->saveAddress(*_receiverAddress);
        }
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
