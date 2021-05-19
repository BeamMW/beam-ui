// Copyright 2019 The Beam Team
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

#include "swap_offers_list.h"

SwapOffersList::SwapOffersList()
{
}

QHash<int, QByteArray> SwapOffersList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        { static_cast<int>(Roles::TimeCreated), "timeCreated" },
        { static_cast<int>(Roles::TimeCreatedSort), "timeCreatedSort" },
        { static_cast<int>(Roles::AmountSend), "amountSend" },
        { static_cast<int>(Roles::AmountSendSort), "amountSendSort" },
        { static_cast<int>(Roles::AmountReceive), "amountReceive" },
        { static_cast<int>(Roles::AmountReceiveSort), "amountReceiveSort" },
        { static_cast<int>(Roles::Rate), "rate" },
        { static_cast<int>(Roles::RateSort), "rateSort" },
        { static_cast<int>(Roles::Expiration), "expiration" },
        { static_cast<int>(Roles::ExpirationSort), "expirationSort" },
        { static_cast<int>(Roles::SwapCoin), "swapCoin" },
        { static_cast<int>(Roles::IsOwnOffer), "isOwnOffer" },
        { static_cast<int>(Roles::IsSendBeam), "isSendBeam" },
        { static_cast<int>(Roles::RawTxID), "rawTxID" },
        { static_cast<int>(Roles::RawTxParameters), "rawTxParameters" },
        { static_cast<int>(Roles::Pair), "pair" }
        
    };
    return roles;
}

QVariant SwapOffersList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
       return QVariant();
    }

    auto& value = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::TimeCreated:
            return value->timeCreated().toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        case Roles::TimeCreatedSort:
            return value->timeCreated();

        case Roles::AmountSend:
            return value->amountSend();

        case Roles::AmountSendSort:
            return static_cast<qulonglong>(value->rawAmountSend());

        case Roles::AmountReceive:
            return value->amountReceive();

        case Roles::AmountReceiveSort:
            return static_cast<qulonglong>(value->rawAmountReceive());

        case Roles::Rate:
        case Roles::RateSort:
            return value->rate();

        case Roles::Expiration:
            return value->timeExpiration().toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        case Roles::ExpirationSort:
            return value->timeExpiration();

        case Roles::SwapCoin:
            return value->getSwapCoinName();

        case Roles::IsOwnOffer:
            return value->isOwnOffer();

        case Roles::IsSendBeam:
            return value->isSendBeam();

        case Roles::RawTxID:
            return QVariant::fromValue(value->getTxID());

        case Roles::RawTxParameters:
            return QVariant::fromValue(value->getTxParameters());

        case Roles::Pair:
        {
            auto swapCoin = value->getSwapCoinName();
            const QString beam = "beam";
            return  value->isSendBeam() ? beam + '-' + swapCoin : swapCoin + '-' + beam;
        }
        default:
            return QVariant();
    }
}
