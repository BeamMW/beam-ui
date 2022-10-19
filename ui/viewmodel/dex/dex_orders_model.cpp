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

#include "dex_orders_model.h"

#include "model/app_model.h"

#include <qdebug.h>

DexOrdersModel::DexOrdersModel()
    : _walletModel(AppModel::getInstance().getWalletModel())
{
    connect(_walletModel.get(), &WalletModel::dexOrdersChanged, this, &DexOrdersModel::onDexOrdersChanged);

    _walletModel->getAsync()->getDexOrders();
}

DexOrdersModel::~DexOrdersModel()
{
}

QAbstractItemModel* DexOrdersModel::getOrders()
{
    return &_orders;
}

void DexOrdersModel::cancelOrder(QString orderId)
{
    beam::wallet::DexOrderID id;
    id.FromHex(orderId.toStdString());

    _walletModel->getAsync()->cancelDexOrder(id);
}

void DexOrdersModel::updateAssets()
{
    _walletModel->getAsync()->loadFullAssetsList();
}

void DexOrdersModel::onDexOrdersChanged(
    beam::wallet::ChangeAction action, const std::vector<beam::wallet::DexOrder>& orders)
{
    using ChangeAction = beam::wallet::ChangeAction;

    switch (action)
    {
        case ChangeAction::Reset:
            _orders.reset(orders);
            break;

        case ChangeAction::Added:
            _orders.insert(orders);
            break;

        case ChangeAction::Removed:
            _orders.remove(orders);
            break;

        case ChangeAction::Updated:
            _orders.update(orders);
            break;

        default:
            assert(false);
            break;
    }

    emit ordersChanged();
}
