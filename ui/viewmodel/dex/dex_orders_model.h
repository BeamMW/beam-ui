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
#pragma once

#include <QObject>
#include "model/wallet_model.h"
#include "dex_orders_list.h"

class DexOrdersModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* orders READ getOrders NOTIFY ordersChanged)

public:
    DexOrdersModel();
    ~DexOrdersModel();

    QAbstractItemModel* getOrders();
    Q_INVOKABLE void cancelOrder(QString orderId);

signals:
    void ordersChanged();

public slots:
    void onDexOrdersChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::DexOrder>&);

private:
    WalletModel::Ptr _walletModel;
    DexOrdersList _orders;
};
