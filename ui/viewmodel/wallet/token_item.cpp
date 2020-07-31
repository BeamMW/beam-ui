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

using namespace beam;
using namespace beam::wallet;
using namespace beamui;

TokenInfoItem::TokenInfoItem(QObject* parent /* = nullptr */)
        : QObject(parent)
{
}

bool TokenInfoItem::isPermanent() const
{
    return false;
}

QString TokenInfoItem::getTransactionType() const
{
    auto p = m_parameters.GetParameter<TxType>(TxParameterID::TransactionType);
    if (p)
    {
        switch (*p)
        {
        case TxType::PushTransaction:
            {
                auto vouchers = m_parameters.GetParameter<ShieldedVoucherList>(TxParameterID::ShieldedVoucherList);
                if (vouchers && getAddress().isEmpty())
                {
                    //% "Non-interactive"
                    return qtTrId("tx-non-interactive");
                }
            }

            return qtTrId("tx-max-privacy");
        case TxType::Simple:
            return qtTrId("tx-regular");
        default:
            break;
        }
    }
    return qtTrId("tx-regular");;
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

QString TokenInfoItem::getToken() const
{
    return m_token;
}

void TokenInfoItem::setToken(const QString& token)
{
    auto trimmed = token.trimmed();
    if (trimmed != m_token)
    {
        auto p = wallet::ParseParameters(trimmed.toStdString());
        if (p)
        {
            m_parameters = *p;
            m_token = trimmed;
        }
        else
        {
            m_parameters = {};
        }
        
        emit tokenChanged();
    }
}
