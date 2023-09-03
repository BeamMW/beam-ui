// Copyright 2023 The Beam Team
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

#include "address_item.h"

AddressItem::AddressItem(beam::wallet::WalletAddress address)
    : m_walletAddress(std::move(address))
{

}

QString AddressItem::getToken() const
{
    return QString::fromStdString(m_walletAddress.m_Token);
}

QString AddressItem::getWalletID() const
{
    return QString::fromStdString(std::to_string(m_walletAddress.m_BbsAddr));
}

QString AddressItem::getName() const
{
    return QString::fromStdString(m_walletAddress.m_label);
}

QString AddressItem::getCategory() const
{
    return QString::fromStdString(m_walletAddress.m_category);
}

QString AddressItem::getIdentity() const
{
    return QString::fromStdString(std::to_base58(m_walletAddress.m_Endpoint));
}

QDateTime AddressItem::getExpirationDate() const
{
    QDateTime datetime;
    datetime.setTime_t(m_walletAddress.getExpirationTime());
    
    return datetime;
}

QDateTime AddressItem::getCreateDate() const
{
    QDateTime datetime;
    datetime.setTime_t(m_walletAddress.getCreateTime());
    
    return datetime;
}

bool AddressItem::isNeverExpired() const
{
    return (m_walletAddress.m_duration == 0);
}

QString AddressItem::getStrView() const
{
    return m_walletAddress.m_label.empty() ? getToken() : getName() + ": " + getToken();
}

bool AddressItem::isExpired() const
{
    return m_walletAddress.isExpired();
}

beam::Timestamp AddressItem::getCreateTimestamp() const
{
    return m_walletAddress.getCreateTime();
}

beam::Timestamp AddressItem::getExpirationTimestamp() const
{
    return m_walletAddress.getExpirationTime();
}
