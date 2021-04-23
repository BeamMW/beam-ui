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
#include "swap_token_item.h"
#include "viewmodel/ui_helpers.h"

using namespace beam;
using namespace beamui;

SwapTokenInfoItem::SwapTokenInfoItem(QObject* parent /* = nullptr */)
        : SwapOfferItem(parent)
{
}

QString SwapTokenInfoItem::getExpirationTime() const
{
    return m_expirationTime;
}

void SwapTokenInfoItem::setExpirationTime(const QString& value)
{
    if (value != m_expirationTime)
    {
        m_expirationTime = value;
        emit tokenChanged();
    }
}

QString SwapTokenInfoItem::getToken() const
{
    return m_token;
}

void SwapTokenInfoItem::setToken(const QString& token)
{
    auto trimmed = token.trimmed();
    if (trimmed != m_token)
    {
        auto p = wallet::ParseParameters(trimmed.toStdString());
        if (p)
        {
            m_parameters = *p;
            beam::wallet::SwapOffer offer = m_parameters;
            offer.m_isOwn = true;
            reset(offer);

            m_token = trimmed;
        }
        else
        {
            m_parameters = {};
        }
        
        emit tokenChanged();
    }
}
