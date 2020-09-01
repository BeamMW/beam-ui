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
#include "token_item.h"
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"

using namespace beam;
using namespace beam::wallet;
using namespace beamui;

TokenInfoItem::TokenInfoItem(QObject* parent /* = nullptr */)
        : QObject(parent)
{
    connect(AppModel::getInstance().getWallet().get(), &WalletModel::getAddressReturned, this, &TokenInfoItem::onGetAddressReturned);
}

bool TokenInfoItem::isPermanent() const
{
    auto p = m_parameters.GetParameter<bool>(TxParameterID::IsPermanentPeerID);
    if (p)
    {
        return *p;
    }
    if (m_defaultPermanent)
    {
        return *m_defaultPermanent;
    }
    return false;
}

bool TokenInfoItem::hasAddressType() const
{
    return m_defaultPermanent.is_initialized() || m_parameters.GetParameter<bool>(TxParameterID::IsPermanentPeerID);
}

QString TokenInfoItem::getTransactionType() const
{
    auto p = m_parameters.GetParameter<TxType>(TxParameterID::TransactionType);
    if (p)
    {
        switch (*p)
        {
        case TxType::PushTransaction:
            return qtTrId("tx-max-privacy");
        case TxType::Simple:
            return qtTrId("tx-regular");
        default:
            break;
        }
    }
    return qtTrId("tx-regular");
}

QString TokenInfoItem::getAmount() const
{
    auto p = m_parameters.GetParameter<Amount>(TxParameterID::Amount);
    if (p)
    {
        return AmountToUIString(*p, Currencies::Beam);
    }
    return "";
}

QString TokenInfoItem::getAmountValue() const
{
    auto p = m_parameters.GetParameter<Amount>(TxParameterID::Amount);
    if (p)
    {
        return AmountToUIString(*p, Currencies::Unknown);
    }
    return "";
}

QString TokenInfoItem::getAddress() const
{
    auto p = m_parameters.GetParameter<WalletID>(TxParameterID::PeerID);
    if (p)
    {
        return toString(*p);
    }
    return "";
}

QString TokenInfoItem::getIdentity() const
{
    auto p = m_parameters.GetParameter<PeerID>(TxParameterID::PeerWalletIdentity);
    if (p)
    {
        return toString(*p);
    }
    return "";
}

QString TokenInfoItem::getTokenType() const
{
    auto p = m_parameters.GetParameter<TxType>(TxParameterID::TransactionType);
    if (p)
    {
        switch (*p)
        {
        case TxType::PushTransaction:
        {
            auto vouchers = m_parameters.GetParameter<ShieldedVoucherList>(TxParameterID::ShieldedVoucherList);
            if (vouchers && !vouchers->empty())
            {
                int offlinePayments = getIgnoreStoredVouchers() ? static_cast<int>(vouchers->size()) : getOfflinePayments();
                //% "Offline (%1)"
                return qtTrId("tx-address-offline-count").arg(offlinePayments);
            }
            //% "Online"
            return qtTrId("tx-address-online");
        }
        default:
            break;
        }
    }
    return "";
}

QString TokenInfoItem::getToken() const
{
    return m_token;
}

void TokenInfoItem::setToken(const QString& token)
{
    auto trimmed = token.trimmed();
    if (trimmed != m_token)
    {
        m_token = trimmed;
        auto p = wallet::ParseParameters(trimmed.toStdString());
        if (p)
        {
            m_parameters = *p;
        }
        else
        {
            m_parameters = {};
        }
        
        emit tokenChanged();
        emit offlinePaymentsChanged();
        auto peerID = m_parameters.GetParameter<WalletID>(TxParameterID::PeerID);
        if (peerID)
        {
            AppModel::getInstance().getWallet()->getAsync()->getAddress(*peerID);
        }
    }
}

int TokenInfoItem::getOfflinePayments() const
{
    return m_offlinePayments;
}

void TokenInfoItem::setOfflinePayments(int value)
{
    if (m_offlinePayments != value)
    {
        m_offlinePayments = value;
        emit offlinePaymentsChanged();
    }
}

bool TokenInfoItem::getDefaultPermanent() const
{
    return *m_defaultPermanent;
}

void TokenInfoItem::setDefaultPermanent(bool value)
{
    if (m_defaultPermanent != value)
    {
        m_defaultPermanent = value;
        emit defaultPermanentChanged();
    }
}

void TokenInfoItem::onGetAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments)
{
    auto p = m_parameters.GetParameter<WalletID>(TxParameterID::PeerID);
    if (p && *p == id)
    {
        setOfflinePayments(offlinePayments);
    }
}

bool TokenInfoItem::getIgnoreStoredVouchers() const
{
    return m_ignoreStoredVouchers;
}

void TokenInfoItem::setIgnoreStoredVouchers(bool value)
{
    if (m_ignoreStoredVouchers != value)
    {
        m_ignoreStoredVouchers = value;
        emit ignoreStoredVouchersChanged();
    }
}
