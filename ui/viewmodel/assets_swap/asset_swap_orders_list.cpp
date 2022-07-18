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

#include "asset_swap_orders_list.h"

#include "viewmodel/qml_globals.h"
#include "viewmodel/ui_helpers.h"
#include <QDateTime>

namespace
{
    const uint kPrecission = 9;
} // namespace

AssetSwapOrdersList::AssetSwapOrdersList()
{
}

QHash<int, QByteArray> AssetSwapOrdersList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        {static_cast<int>(Roles::RId),         "id"},
        {static_cast<int>(Roles::RSend),       "send"},
        {static_cast<int>(Roles::RReceive),    "receive"},
        {static_cast<int>(Roles::RRate),       "rate"},
        {static_cast<int>(Roles::RIsMine),     "isMine"},
        {static_cast<int>(Roles::RCreateTime), "created"},
        {static_cast<int>(Roles::RExpireTime), "expiration"},
    };
    return roles;
}

QVariant AssetSwapOrdersList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
        return QVariant();
    }

    const auto row = index.row();
    const auto& order = m_list[row];

    switch (static_cast<Roles>(role))
    {
        case Roles::RId:
        {
            return QString::fromStdString(order.getID().to_string());
        }
        case Roles::RSend:
        {
            return beamui::AmountToUIString(order.getSendAmount()) + " " + QString::fromStdString(order.getSendAssetSName());
        }
        case Roles::RReceive:
        {
            return beamui::AmountToUIString(order.getReceiveAmount()) + " " + QString::fromStdString(order.getReceiveAssetSName());
        }
        case Roles::RRate:
        {
            return QMLGlobals::divideWithPrecision(
                beamui::AmountToUIString(order.getReceiveAmount()),
                beamui::AmountToUIString(order.getSendAmount()),
                kPrecission);
        }
        case Roles::RIsMine:
        {
            return order.isMine();
        }
        case Roles::RCreateTime:
        {
            QDateTime datetime;
            datetime.setTime_t(order.getCreation());
            return datetime.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        }
        case Roles::RExpireTime:
        {
            QDateTime datetime;
            datetime.setTime_t(order.getExpiration());
            return datetime.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        }

        default:
        {
            return QVariant();
        }
    }
}
