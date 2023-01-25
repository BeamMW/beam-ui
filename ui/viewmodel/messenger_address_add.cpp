// Copyright 2022 The Beam Team
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

#include "messenger_address_add.h"

#include "model/app_model.h"

MessengerAddressAdd::MessengerAddressAdd() :
    _walletModel(AppModel::getInstance().getWalletModel())
{
    connect(_walletModel.get(),
            SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)),
            SLOT(onAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));

    _walletModel->getAsync()->getAddresses(false);
}

const QString& MessengerAddressAdd::getAddress() const
{
    return _address;
}

const QString& MessengerAddressAdd::getName() const
{
    return _name;
}

bool MessengerAddressAdd::getError() const
{
    return !_address.isEmpty() ? _error : false;
}

QString MessengerAddressAdd::getPeerID() const
{
    return !_error && _peerID.IsValid() ? QString::fromStdString(std::to_string(_peerID)) : "";
}

void MessengerAddressAdd::setAddress(const QString& addr)
{
    if (_address != addr)
    {
        _address = addr;
        emit addressChanged();

        auto p = beam::wallet::ParseParameters(_address.toStdString());
        _error = !p;
        emit errorChanged();
        if (_error)
            return;

        auto peerID = p->GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerID);
        if (peerID)
        {
            _peerID = *peerID;
            auto fRes = std::find_if(
                _contacts.begin(), _contacts.end(),
                [peerID] (const beam::wallet::WalletAddress& addr) 
                {
                    return addr.m_BbsAddr == *peerID;
                });
            if (fRes != _contacts.end())
            {
                _name = QString::fromStdString(fRes->m_label);
                emit nameChanged();
            }
        }
        else
        {
            _error = true;
        }
    }
}

void MessengerAddressAdd::setName(const QString& name)
{
    if (_name != name)
    {
        _name = name;
        emit nameChanged();
    }
}

void MessengerAddressAdd::saveAddress()
{
    if (!_error && _peerID.IsValid())
    {
        beam::wallet::WalletAddress address;
        address.m_BbsAddr   = _peerID;
        address.m_createTime = beam::getTimestamp();
        address.m_label      = _name.trimmed().toStdString();
        address.m_duration   = beam::wallet::WalletAddress::AddressExpirationNever;
        address.m_Token    = _address.toStdString();
        _walletModel->getAsync()->saveAddress(address);
    }
}

void MessengerAddressAdd::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    if (!own)
        _contacts = addresses;
}
