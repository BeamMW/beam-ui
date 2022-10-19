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

#include "dex_orders_list.h"

#include "model/app_model.h"
#include "viewmodel/qml_globals.h"
#include "viewmodel/ui_helpers.h"
#include <QDateTime>

DexOrdersList::DexOrdersList()
    : m_amgr(AppModel::getInstance().getAssets())
    , m_wallet(AppModel::getInstance().getWalletModel())
{
    connect(m_wallet.get(), &WalletModel::fullAssetsListLoaded, this, &DexOrdersList::assetsListChanged);
    m_wallet->getAsync()->loadFullAssetsList();
}

QHash<int, QByteArray> DexOrdersList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        {static_cast<int>(Roles::RId),                 "id"},
        {static_cast<int>(Roles::RIdSort),             "idSort"},
        {static_cast<int>(Roles::RSend),               "send"},
        {static_cast<int>(Roles::RSendSort),           "sendSort"},
        {static_cast<int>(Roles::RReceive),            "receive"},
        {static_cast<int>(Roles::RReceiveSort),        "receiveSort"},
        {static_cast<int>(Roles::RRate),               "rate"},
        {static_cast<int>(Roles::RRateSort),           "rateSort"},
        {static_cast<int>(Roles::RIsMine),             "isMine"},
        {static_cast<int>(Roles::RIsMineSort),         "isMineSort"},
        {static_cast<int>(Roles::RCreateTime),         "created"},
        {static_cast<int>(Roles::RCreateTimeSort),     "createdSort"},
        {static_cast<int>(Roles::RExpireTime),         "expiration"},
        {static_cast<int>(Roles::RExpireTimeSort),     "expirationSort"},
        {static_cast<int>(Roles::RCoins),              "coins"},
        {static_cast<int>(Roles::RCoinsSort),          "coinsSort"},
        {static_cast<int>(Roles::RHasAssetToSend),     "hasAssetToSend"},
        {static_cast<int>(Roles::RHasAssetToSendSort), "hasAssetToSendSort"},
        {static_cast<int>(Roles::RAssetsFilter),       "assetsFilter"},
        
    };
    return roles;
}

QVariant DexOrdersList::data(const QModelIndex &index, int role) const
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
        case Roles::RIdSort:
        {
            return QString::fromStdString(order.getID().to_string());
        }
        case Roles::RSend:
        {
            return beamui::AmountToUIString(order.getSendAmount()) + " " + QString::fromStdString(order.getSendAssetSName());
        }
        case Roles::RSendSort:
        {
            return QVariant::fromValue(order.getSendAmount());
        }
        case Roles::RReceive:
        {
            return beamui::AmountToUIString(order.getReceiveAmount()) + " " + QString::fromStdString(order.getReceiveAssetSName());
        }
        case Roles::RReceiveSort:
        {
            return QVariant::fromValue(order.getReceiveAmount());
        }
        case Roles::RRate:
        case Roles::RRateSort:
        {
            return QMLGlobals::divideWithPrecision(
                beamui::AmountToUIString(order.getReceiveAmount()),
                beamui::AmountToUIString(order.getSendAmount()),
                beam::wallet::kDexOrderRatePrecission);
        }
        case Roles::RIsMine:
        case Roles::RIsMineSort:
        {
            return order.isMine();
        }
        case Roles::RCreateTime:
        {
            QDateTime datetime;
            datetime.setTime_t(order.getCreation());
            return datetime.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        }
        case Roles::RCreateTimeSort:
        {
            return QVariant::fromValue(order.getCreation());
        }
        case Roles::RExpireTime:
        {
            QDateTime datetime;
            datetime.setTime_t(order.getExpiration());
            return datetime.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        }
        case Roles::RExpireTimeSort:
        {
            return QVariant::fromValue(order.getExpiration());
        }
        case Roles::RCoins:
        {
            if (!m_amgr->isKnownAsset(order.getSendAssetId()) || !m_amgr->isKnownAsset(order.getReceiveAssetId()))
                m_wallet->getAsync()->loadFullAssetsList();

            QVariantMap res;
            res.insert("sendIcon", m_amgr->getIcon(order.getSendAssetId()));
            res.insert("receiveIcon", m_amgr->getIcon(order.getReceiveAssetId()));
            return QVariant::fromValue(res);
        }
        case Roles::RCoinsSort:
        {
            uint64_t res = static_cast<uint64_t>(order.getSendAssetId()) << 32;
            res += order.getReceiveAssetId();
            return QVariant::fromValue(res);

        }
        case Roles::RHasAssetToSend:
        case Roles::RHasAssetToSendSort:
        {
            if (!m_amgr->hasAsset(order.getSendAssetId())) return false;

            auto availableAmount = m_wallet->getAvailable(order.getSendAssetId());
            return order.getSendAmount() <= beam::AmountBig::get_Lo(availableAmount) &&
                !beam::AmountBig::get_Hi(availableAmount);
        }

        case Roles::RAssetsFilter:
        {
            QString res;
            res.append(QString::number(order.getSendAssetId()));
            res.append(",");
            res.append(QString::number(order.getReceiveAssetId()));
            return res;
        }

        default:
        {
            return QVariant();
        }
    }
}

void DexOrdersList::assetsListChanged()
{
    emit layoutChanged();
}
